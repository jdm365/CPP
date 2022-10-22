#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>

#include "node.hpp"
#include "tree.hpp"
#include "gbm.hpp"


GBM::GBM(
		int   max_depth_new,
		float l2_reg_new,
		float lr_new,
		float min_child_weight_new,
		int   min_data_in_leaf_new,
		int   num_boosting_rounds_new,
		int   max_bin_new
		) {
	max_depth 			= max_depth_new;
	l2_reg 				= l2_reg_new;
	lr 					= lr_new;
	min_child_weight 	= min_child_weight_new;
	min_data_in_leaf 	= min_data_in_leaf_new;
	num_boosting_rounds = num_boosting_rounds_new;
	max_bin				= max_bin_new;
}

void GBM::train_greedy(
		std::vector<std::vector<float>>& X, 
		std::vector<std::vector<float>>& X_rowmajor, 
		std::vector<float>& y
		) {
	std::vector<float> gradient(X[0].size());
	std::vector<float> hessian(X[0].size());

	float loss;

	std::vector<float> preds;
	trees.reserve(num_boosting_rounds);
	auto start = std::chrono::high_resolution_clock::now();
	for (int round = 0; round < num_boosting_rounds; round++) {
		trees.emplace_back(
					X,
					gradient,
					hessian,
					round,
					max_depth,
					l2_reg,
					min_child_weight,
					min_data_in_leaf
			);

		std::vector<float> round_preds = trees[round].predict(X_rowmajor);
		for (int idx = 0; idx < int(round_preds.size()); idx++) {
			if (round == 0) {
				preds.push_back(lr * round_preds[idx]);
			}
			else {
				preds[idx] += lr * round_preds[idx];
			}
		}
		gradient = calculate_gradient(preds, y);
		hessian  = calculate_hessian(preds, y);

		loss = calculate_mse_loss(preds, y);
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		std::cout << "Round " << round + 1 << " MSE Loss: " << loss;
		std::cout << "               Time Elapsed: " << duration.count() << std::endl;
	}
}


void GBM::train_hist(
		std::vector<std::vector<float>>& X, 
		std::vector<float>& y
		) {
	std::vector<float> gradient(X[0].size());
	std::vector<float> hessian(X[0].size(), 2.00f);

	auto start_0 = std::chrono::high_resolution_clock::now();
	std::vector<std::vector<int>> X_hist = map_hist_bins(X, max_bin);
	std::vector<std::vector<int>> X_hist_rowmajor = get_hist_bins_rowmajor(X_hist);

	std::vector<std::vector<int>> min_max_rem;
	for (int col = 0; col < int(X.size()); ++col) {
		min_max_rem.push_back(
				{0, 1 + *std::max_element(X_hist[col].begin(), X_hist[col].end())}
				);
	}
	auto stop_0 = std::chrono::high_resolution_clock::now();
	auto duration_0 = std::chrono::duration_cast<std::chrono::milliseconds>(stop_0 - start_0);
	std::cout << "Hist constructed in " << duration_0.count() << " milliseconds" << std::endl;

	float loss;
	std::vector<float> preds;

	// Add mean for better start.
	y_mean_train = 0.00f;
	for (int row = 0; row < int(y.size()); ++row) {
		y_mean_train += y[row];
	}
	y_mean_train /= float(y.size());

	trees.reserve(num_boosting_rounds);
	auto start = std::chrono::high_resolution_clock::now();
	for (int round = 0; round < num_boosting_rounds; ++round) {
		trees.emplace_back(
					X_hist_rowmajor,
					gradient,
					hessian,
					round,
					max_depth,
					l2_reg,
					min_child_weight,
					min_data_in_leaf,
					max_bin,
					min_max_rem	
			);

		std::vector<float> round_preds = trees[round].predict_hist(X_hist_rowmajor);

		for (int idx = 0; idx < int(round_preds.size()); ++idx) {
			if (round == 0) {
				preds.push_back(y_mean_train + lr * round_preds[idx]);
			}
			else {
				preds[idx] += lr * round_preds[idx];
			}
		}
		gradient = calculate_gradient(preds, y);
		// hessian  = calculate_hessian(preds, y); // CONST HESS

		loss = calculate_mse_loss(preds, y);
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		std::cout << "Round " << round + 1 << " MSE Loss: " << loss;
		std::cout << "               Time Elapsed: " << duration.count() << std::endl;
	}
}


std::vector<float> GBM::predict(std::vector<std::vector<float>>& X_rowmajor) {
	std::vector<float> round_preds;
	std::vector<float> tree_preds;

	for (int tree_num = 0; tree_num < int(trees.size()); tree_num++) {
		tree_preds = trees[tree_num].predict(X_rowmajor);
		for (int row = 0; row < int(X_rowmajor.size()); row++) {
			if (tree_num == 0) {
				round_preds.push_back(lr * tree_preds[row]);
			}
			else {
				round_preds[row] += lr * tree_preds[row];
			}
		}
	}
	return round_preds;
}

std::vector<float> GBM::predict_hist(std::vector<std::vector<float>>& X) {
	std::vector<float> preds;
	std::vector<float> tree_preds;

	std::vector<std::vector<int>> X_hist = map_hist_bins(X, max_bin);
	std::vector<std::vector<int>> X_hist_rowmajor = get_hist_bins_rowmajor(X_hist);

	for (int tree_num = 0; tree_num < int(trees.size()); ++tree_num) {
		tree_preds = trees[tree_num].predict_hist(X_hist_rowmajor);
		for (int row = 0; row < int(X_hist_rowmajor.size()); ++row) {
			if (tree_num == 0) {
				preds.emplace_back(y_mean_train + lr * tree_preds[row]);
			}
			else {
				preds[row] += lr * tree_preds[row];
			}
		}
	}
	return preds;
}

std::vector<float> GBM::calculate_gradient(std::vector<float>& preds, std::vector<float>& y) {
	std::vector<float> gradient;
	// Assume MSE for now
	for (int idx = 0; idx < int(y.size()); idx++) {
		gradient.emplace_back(2.00f * (preds[idx] - y[idx]));
	}
	return gradient;
}


std::vector<float> GBM::calculate_hessian(std::vector<float>& preds, std::vector<float>& y) {
	std::vector<float> hessian;
	// Assume MSE for now
	for (int idx = 0; idx < int(y.size()); idx++) {
		hessian.push_back(2.00f);
	}
	return hessian;
}


float GBM::calculate_mse_loss(std::vector<float>& preds, std::vector<float>& y) {
	float loss = 0.00f;
	// Assume MSE for now
	for (int idx = 0; idx < int(preds.size()); idx++) {
		loss += 0.50f * std::pow((preds[idx] - y[idx]), 2);
	}
	loss = loss / float(preds.size());	
	return loss;
}

std::vector<std::vector<int>> GBM::map_hist_bins(
		std::vector<std::vector<float>>& X,
		int& max_bin
		) {
	int n_rows = int(X[0].size());
	int n_cols = int(X.size());

	int bin_size   = std::ceil(n_rows / max_bin);
	int total_bins = 0;

	std::vector<std::vector<int>> X_hist(n_cols, std::vector<int>(n_rows));
	std::vector<float> X_col;
	X_col.reserve(n_rows);

	for (int col = 0; col < n_cols; col++) {
		X_col = X[col];
		float last_X = 0.00f;
		int   cntr 	 = 0;
		int   bin  	 = 0;

		std::vector<int> idxs(n_rows);
		std::iota(idxs.begin(), idxs.end(), 0);
		std::stable_sort(idxs.begin(), idxs.end(), [&X_col](int i, int j){return X_col[i] < X_col[j];});

		for (int row = 0; row < n_rows; row++) {
			X_hist[col][idxs[row]] = std::min(bin, max_bin - 1);
			if (cntr >= bin_size && last_X != X_col[idxs[row]]) {
				cntr = 0;
				bin++;
			}
			else {
				cntr++;
			}
			last_X = X_col[idxs[row]];
		}
		total_bins += std::min(bin, max_bin - 1);
	}
	std::cout << "Total bins used: " << total_bins << std::endl;
	std::cout << std::endl;
	return X_hist;
}


std::vector<std::vector<int>> GBM::get_hist_bins_rowmajor(
		std::vector<std::vector<int>>& X_hist
		) {
	std::vector<std::vector<int>> X_hist_rowmajor(
			X_hist[0].size(),
			std::vector<int>(X_hist.size())
			);

	for (int row = 0; row < int(X_hist[0].size()); ++row) {
		for (int col = 0; col < int(X_hist.size()); ++col) {
			X_hist_rowmajor[row][col] = X_hist[col][row];
		}
	}
	return X_hist_rowmajor;
}
