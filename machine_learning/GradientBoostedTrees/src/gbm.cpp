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
		int   num_boosting_rounds_new
		) {
	max_depth 			= max_depth_new;
	l2_reg 				= l2_reg_new;
	lr 					= lr_new;
	min_child_weight 	= min_child_weight_new;
	min_data_in_leaf 	= min_data_in_leaf_new;
	num_boosting_rounds = num_boosting_rounds_new;
}

void GBM::train_greedy(
		std::vector<std::vector<float>>& X, 
		std::vector<std::vector<float>>& X_rowwise, 
		std::vector<float>& y
		) {
	std::vector<float> gradient(X[0].size());
	std::vector<float> hessian(X[0].size());

	std::vector<std::vector<float>> split_vals;

	int max_bins 	 = 64;
	int n_total_bins = 0;
	for (int col = 0; col < int(X.size()); col++) {
		split_vals.push_back(get_quantiles(X[col], max_bins));
		n_total_bins += int(split_vals.size());
	}
	std::cout << "Num bins: " << n_total_bins << std::endl;

	float loss;

	std::vector<float> preds;
	trees.reserve(num_boosting_rounds);
	auto start = std::chrono::high_resolution_clock::now();
	for (int round = 0; round < num_boosting_rounds; round++) {
		trees.emplace_back(
					X,
					split_vals,
					gradient,
					hessian,
					round,
					max_depth,
					l2_reg,
					min_child_weight,
					min_data_in_leaf
			);

		std::vector<float> round_preds = trees[round].predict(X_rowwise);
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
		std::vector<std::vector<float>>& X_rowwise, 
		std::vector<float>& y
		) {
	std::vector<float> gradient(X[0].size());
	std::vector<float> hessian(X[0].size());

	int n_bins = 255;

	std::vector<std::vector<int>> X_hist = map_hist_bins(X, n_bins);

	float loss;
	std::vector<float> preds;

	trees.reserve(num_boosting_rounds);

	auto start = std::chrono::high_resolution_clock::now();
	for (int round = 0; round < num_boosting_rounds; ++round) {
		trees.emplace_back(
					X_hist,
					gradient,
					hessian,
					round,
					max_depth,
					l2_reg,
					min_child_weight,
					min_data_in_leaf,
					n_bins
			);

		std::vector<float> round_preds = trees[round].predict(X_rowwise);
		for (int idx = 0; idx < int(round_preds.size()); ++idx) {
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


std::vector<float> GBM::predict(std::vector<std::vector<float>>& X_rowwise) {
	std::vector<float> round_preds;
	std::vector<float> tree_preds;

	for (int tree_num = 0; tree_num < int(trees.size()); tree_num++) {
		tree_preds = trees[tree_num].predict(X_rowwise);
		for (int row = 0; row < int(X_rowwise.size()); row++) {
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


std::vector<float> GBM::calculate_gradient(std::vector<float>& preds, std::vector<float>& y) {
	std::vector<float> gradient;
	// Assume MSE for now
	for (int idx = 0; idx < int(y.size()); idx++) {
		gradient.push_back(2 * (preds[idx] - y[idx]));
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

std::vector<float> GBM::get_quantiles(std::vector<float> X_col, int n_bins) {
	std::vector<float> split_vals;
	std::vector<int> sorted_col_idxs(X_col.size());
	std::iota(sorted_col_idxs.begin(), sorted_col_idxs.end(), 0);

	std::stable_sort(
			sorted_col_idxs.begin(), 
			sorted_col_idxs.end(), 
			[&X_col](int i, int j) {return X_col[i] < X_col[j];}
			);
	orig_col_idxs.push_back(sorted_col_idxs);

	//  Init to random. Get unique quantiles.
	//  float last_val = INFINITY;
	int   split_idx;
	int   bin_size = int(int(X_col.size()) / n_bins);

	for (int idx = 0; idx < n_bins; idx++) {
		split_idx = idx * bin_size;

		// Only get unique quantiles.
		//if (X_col[split_idx] == last_val) {
			//continue;
		//}
		//last_val = X_col[split_idx];

		split_vals.push_back(X_col[split_idx]);
	}
	return split_vals;
}


void GBM::get_sorted_idxs(std::vector<std::vector<float>> X, int n_bins) {
	std::vector<float> X_col;
	for (int col = 0; col < int(X.size()); col++) {
		X_col = X[col];
		std::vector<int> sorted_col_idxs(X_col.size());
		std::iota(sorted_col_idxs.begin(), sorted_col_idxs.end(), 0);

		std::stable_sort(
				sorted_col_idxs.begin(), 
				sorted_col_idxs.end(), 
				[&X_col](int i, int j) {return X_col[i] < X_col[j];}
				);
		orig_col_idxs.push_back(sorted_col_idxs);
	} 
}

std::vector<std::vector<int>> GBM::map_hist_bins(
		std::vector<std::vector<float>>& X,
		int& n_bins
		) {
	int n_rows = int(X[0].size());
	int n_cols = int(X.size());

	int bin_size = std::ceil(n_rows / n_bins);

	std::vector<std::vector<int>> X_hist(n_cols, std::vector<int>(n_rows));
	std::vector<float> X_col;
	X_col.reserve(n_rows);

	for (int col = 0; col < n_cols; col++) {
		X_col = X[col];

		std::vector<int> idxs(n_rows);
		std::iota(idxs.begin(), idxs.end(), 0);
		std::stable_sort(idxs.begin(), idxs.end(), [&X_col](int i, int j){return X_col[i] < X_col[j];});

		for (int row = 0; row < n_rows; row++) {
			X_hist[col][idxs[row]] = std::min(int(row / bin_size), n_bins - 1);
		}
	}
	return X_hist;
}
