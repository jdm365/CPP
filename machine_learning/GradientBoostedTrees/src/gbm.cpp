#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
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
		int   max_bin_new,
		bool  const_hessian_new
		) {
	max_depth 			= max_depth_new;
	l2_reg 				= l2_reg_new;
	lr 					= lr_new;
	min_child_weight 	= min_child_weight_new;
	min_data_in_leaf 	= min_data_in_leaf_new;
	num_boosting_rounds = num_boosting_rounds_new;
	max_bin				= max_bin_new;
	const_hessian		= const_hessian;
}

void GBM::train_greedy(
		std::vector<std::vector<float>>& X, 
		std::vector<float>& y
		) {
	int n_rows = int(X[0].size());
	int n_cols = int(X.size());

	std::vector<float> gradient(n_rows, 0.00f);
	std::vector<float> hessian(n_rows, 2.00f);

	float loss;

	for (int row = 0; row < n_rows; ++row) {
		y_mean_train += y[row];
	}
	y_mean_train /= y.size();

	std::vector<float> preds(n_rows, y_mean_train);
	std::vector<float> round_preds;

	std::vector<std::vector<float>> X_rowmajor(
			n_rows,
			std::vector<float>(n_cols) 
			);
	for (int col = 0; col < n_cols; ++col) {
		for (int row = 0; row < n_rows; ++row) {
			X_rowmajor[row][col] = X[col][row];
		}
	}



	auto start = std::chrono::high_resolution_clock::now();
	for (int round = 0; round < num_boosting_rounds; ++round) {
		trees.emplace_back(
					X,
					gradient,
					hessian,
					max_depth,
					l2_reg,
					min_child_weight,
					min_data_in_leaf
			);

		round_preds = trees[round].predict(X_rowmajor);
		for (int row = 0; row < n_rows; ++row) {
			preds[row] += lr * round_preds[row];
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
	int n_rows = int(X[0].size());
	int n_cols = int(X.size());

	std::vector<float> gradient(n_rows, 0.00f);
	std::vector<float> hessian(n_rows, 2.00f);
	float loss;

	auto start_0 = std::chrono::high_resolution_clock::now();

	const std::vector<std::vector<uint8_t>> X_hist = map_hist_bins_train(X, max_bin);
	const std::vector<std::vector<uint8_t>> X_hist_rowmajor = get_hist_bins_rowmajor(X_hist);

	// Get min/max bin per col to avoid unneccessary split finding. 
	alignas(64) std::vector<std::vector<uint8_t>> min_max_rem;
	for (int col = 0; col < n_cols; ++col) {
		min_max_rem.push_back(
				{0, uint8_t(1 + *std::max_element(X_hist[col].begin(), X_hist[col].end()))}
				);
	}

	auto stop_0 = std::chrono::high_resolution_clock::now();
	auto duration_0 = std::chrono::duration_cast<std::chrono::milliseconds>(stop_0 - start_0);
	std::cout << "Hist constructed in " << duration_0.count() << " milliseconds" << std::endl;

	// Add mean for better start.
	y_mean_train = 0.00f;
	for (int row = 0; row < n_rows; ++row) {
		y_mean_train += y[row];
	}
	y_mean_train /= float(n_rows);

	std::vector<float> preds(n_rows, y_mean_train);
	std::vector<float> round_preds;

	round_preds.reserve(n_rows);
	trees.reserve(num_boosting_rounds);

	// Define const hessian histogram for root node.
	std::vector<std::vector<float>> hessian_hist_root(
			n_cols, 
			std::vector<float>(max_bin, 0.00f)
			);

	if (const_hessian) {
		for (int row = 0; row < n_rows; ++row) {
			for (int col = 0; col < n_cols; ++col) {
				hessian_hist_root[col][X_hist_rowmajor[row][col]] += hessian[row];
			}
		}
	}

	auto start_1 = std::chrono::high_resolution_clock::now();
	for (int round = 0; round < num_boosting_rounds; ++round) {
		if (const_hessian) {
			trees.emplace_back(
						X_hist_rowmajor,
						hessian_hist_root,
						gradient,
						hessian,
						max_depth,
						l2_reg,
						min_data_in_leaf,
						max_bin,
						min_max_rem	
				);
		}
		else {
			trees.emplace_back(
						X_hist_rowmajor,
						gradient,
						hessian,
						max_depth,
						l2_reg,
						min_data_in_leaf,
						max_bin,
						min_max_rem	
				);
		}


		round_preds = trees[round].predict_hist(X_hist_rowmajor);
		for (int idx = 0; idx < n_rows; ++idx) {
			preds[idx] += lr * round_preds[idx];
		}

		gradient = calculate_gradient(preds, y);
		hessian  = calculate_hessian(preds, y);
		loss 	 = calculate_mse_loss(preds, y);

		auto stop_1 = std::chrono::high_resolution_clock::now();
		auto duration_1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop_1 - start_1);
		std::cout << "Round " << round + 1 << " MSE Loss: " << loss;
		std::cout << "               Time Elapsed: " << duration_1.count() << std::endl;
	}
}


std::vector<float> GBM::predict(std::vector<std::vector<float>>& X) {
	int n_rows = int(X[0].size());
	int n_cols = int(X.size());

	std::vector<std::vector<float>> X_rowmajor(
			n_rows,
			std::vector<float>(n_cols) 
			);
	for (int col = 0; col < n_cols; ++col) {
		for (int row = 0; row < n_rows; ++row) {
			X_rowmajor[row][col] = X[col][row];
		}
	}

	std::vector<float> preds(n_rows, y_mean_train);
	std::vector<float> tree_preds;

	for (int tree_num = 0; tree_num < num_boosting_rounds; ++tree_num) {
		tree_preds = trees[tree_num].predict(X_rowmajor);
		for (int row = 0; row < n_rows; ++row) {
			preds[row] += lr * tree_preds[row];
		}
	}
	return preds;
}

std::vector<float> GBM::predict_hist(std::vector<std::vector<float>>& X) {
	std::vector<float> preds(X[0].size(), y_mean_train);
	std::vector<float> tree_preds;

	std::vector<std::vector<uint8_t>> X_hist = map_hist_bins_inference(X);
	std::vector<std::vector<uint8_t>> X_hist_rowmajor = get_hist_bins_rowmajor(X_hist);

	for (int tree_num = 0; tree_num < int(trees.size()); ++tree_num) {
		tree_preds = trees[tree_num].predict_hist(X_hist_rowmajor);
		for (int row = 0; row < int(preds.size()); ++row) {
			preds[row] += lr * tree_preds[row];
		}
	}
	return preds;
}

std::vector<float> GBM::calculate_gradient(std::vector<float>& preds, std::vector<float>& y) {
	std::vector<float> gradient;
	// Assume MSE for now
	for (int idx = 0; idx < int(y.size()); ++idx) {
		gradient.push_back(2.00f * (preds[idx] - y[idx]));
	}
	return gradient;
}


std::vector<float> GBM::calculate_hessian(std::vector<float>& preds, std::vector<float>& y) {
	std::vector<float> hessian;
	// Assume MSE for now
	for (int idx = 0; idx < int(y.size()); ++idx) {
		hessian.push_back(2.00f);
	}
	return hessian;
}


float GBM::calculate_mse_loss(std::vector<float>& preds, std::vector<float>& y) {
	float loss = 0.00f;
	// Assume MSE for now
	for (int idx = 0; idx < int(y.size()); ++idx) {
		loss += 0.50f * (preds[idx] - y[idx]) * (preds[idx] - y[idx]);
	}
	loss /= float(y.size());	
	return loss;
}

std::vector<std::vector<uint8_t>> GBM::map_hist_bins_train(
		std::vector<std::vector<float>>& X,
		int& max_bin
		) {
	int n_rows = int(X[0].size());
	int n_cols = int(X.size());

	int bin_size   = std::ceil(n_rows / max_bin);
	int total_bins = 0;

	std::vector<std::vector<uint8_t>> X_hist(n_cols, std::vector<uint8_t>(n_rows));
	std::vector<float> X_col;

	X_col.reserve(n_rows);
	bin_mapping.reserve(n_rows);

	for (int col = 0; col < n_cols; col++) {
		X_col = X[col];
		float 	last_X = 0.00f;
		int     cntr   = 0;
		uint8_t bin    = 0;

		std::vector<int> idxs(n_rows);
		std::iota(idxs.begin(), idxs.end(), 0);
		std::stable_sort(idxs.begin(), idxs.end(), [&X_col](int i, int j){return X_col[i] < X_col[j];});

		for (int row = 0; row < n_rows; ++row) {
			X_hist[col][idxs[row]] = std::min(bin, uint8_t(max_bin - 1));
			if (cntr >= bin_size && last_X != X_col[idxs[row]]) {
				cntr = 0;
				++bin;
				bin_mapping[col].push_back(last_X);
			}
			else {
				++cntr;
			}
			last_X = X_col[idxs[row]];
		}
		total_bins += std::min(bin, uint8_t(max_bin - 1));
	}
	std::cout << "Total bins used: " << total_bins << std::endl;
	std::cout << std::endl;
	return X_hist;
}

std::vector<std::vector<uint8_t>> GBM::map_hist_bins_inference(
		const std::vector<std::vector<float>>& X
		) {
	int n_rows = int(X[0].size());
	int n_cols = int(X.size());

	std::vector<std::vector<uint8_t>> X_hist(n_cols, std::vector<uint8_t>(n_rows));
	std::vector<float> X_col;
	X_col.reserve(n_rows);

	float 	  val;
	uint8_t   bin;
	uint8_t   max_bin_col;


	for (int col = 0; col < n_cols; col++) {
		X_col = X[col];
		max_bin_col = uint8_t(bin_mapping[col].size());
		bin = 0;

		std::vector<int> idxs(n_rows);
		std::iota(idxs.begin(), idxs.end(), 0);
		std::stable_sort(idxs.begin(), idxs.end(), [&X_col](int i, int j){return X_col[i] < X_col[j];});

		for (int row = 0; row < n_rows; ++row) {
			val = X_col[idxs[row]];
			X_hist[col][idxs[row]] = std::min(bin, max_bin_col);
			if (val >= bin_mapping[col][bin]) {
				++bin;
			}
		}
	}
	return X_hist;
}

std::vector<std::vector<uint8_t>> GBM::get_hist_bins_rowmajor(
		const std::vector<std::vector<uint8_t>>& X_hist
		) {
	std::vector<std::vector<uint8_t>> X_hist_rowmajor(
			X_hist[0].size(),
			std::vector<uint8_t>(X_hist.size())
			);

	for (int row = 0; row < int(X_hist[0].size()); ++row) {
		for (int col = 0; col < int(X_hist.size()); ++col) {
			X_hist_rowmajor[row][col] = X_hist[col][row];
		}
	}
	return X_hist_rowmajor;
}
