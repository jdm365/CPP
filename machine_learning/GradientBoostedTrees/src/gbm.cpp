#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <chrono>
#include <assert.h>

#include "node.hpp"
#include "tree.hpp"
#include "gbm.hpp"
#include "histogram_mapping.hpp"
#include "utils.hpp"


GBM::GBM(
		int   max_depth_new,
		float l2_reg_new,
		float lr_new,
		float min_child_weight_new,
		int   min_data_in_leaf_new,
		int   num_boosting_rounds_new,
		int   max_bin_new,
		int   max_leaves_new
		) {
	if (max_depth_new <= 0) {
		max_depth_new = 1048576;
	}
	max_depth 			= max_depth_new;
	l2_reg 				= l2_reg_new;
	lr 					= lr_new;
	min_child_weight 	= min_child_weight_new;
	min_data_in_leaf 	= min_data_in_leaf_new;
	num_boosting_rounds = num_boosting_rounds_new;
	max_bin				= max_bin_new;
	max_leaves			= 2 * max_leaves_new + 1; // This represents max nodes to
												  // get max leaves.

	trees.reserve(num_boosting_rounds);
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

	std::vector<float> round_preds;
	float* preds = (float*) malloc(sizeof(float) * n_rows);
	for (int idx = 0; idx < n_rows; ++idx) {
		preds[idx] = y_mean_train;
	}

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
	free(preds);
}


void GBM::train_hist(
		std::vector<std::vector<float>>& X, 
		std::vector<float>& y
		) {
	int n_rows = int(X[0].size());
	int n_cols = int(X.size());

	std::vector<float> gradient(n_rows, 0.00f);
	std::vector<float> hessian(n_rows, 2.00f);

	auto start_0 = std::chrono::high_resolution_clock::now();

	const std::vector<std::vector<uint8_t>> X_hist = map_hist_bins_train(X, bin_mapping, max_bin);
	const std::vector<std::vector<uint8_t>> X_hist_rowmajor = get_hist_bins_rowmajor(X_hist);

	// Get min/max bin per col to avoid unneccessary split finding. 
	std::vector<std::vector<uint8_t>> min_max_rem = get_min_max_rem(X_hist);

	auto stop_0     = std::chrono::high_resolution_clock::now();
	auto duration_0 = std::chrono::duration_cast<std::chrono::milliseconds>(stop_0 - start_0);

	std::cout << "Hist constructed in " << duration_0.count() << " milliseconds" << std::endl;

	// Add mean for better start.
	y_mean_train = get_vector_mean(y);

	float* round_preds = (float*) malloc(sizeof(float) * n_rows);
	float* preds 	   = (float*) malloc(sizeof(float) * n_rows);
	for (int idx = 0; idx < n_rows; ++idx) {
		preds[idx] = y_mean_train;
	}

	auto start_1 = std::chrono::high_resolution_clock::now();
	for (int round = 0; round < num_boosting_rounds; ++round) {
		trees.emplace_back(
					X_hist,
					gradient,
					hessian,
					max_depth,
					l2_reg,
					min_data_in_leaf,
					max_bin,
					max_leaves,
					min_max_rem	
			);


		round_preds = trees[round].predict_hist(X_hist_rowmajor);
		for (int idx = 0; idx < n_rows; ++idx) {
			preds[idx] += lr * round_preds[idx];
		}

		gradient = calculate_gradient(preds, y);
		hessian  = calculate_hessian(preds, y);

		auto stop_1 = std::chrono::high_resolution_clock::now();
		auto duration_1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop_1 - start_1);
		std::cout << "Round " << round + 1 << " MSE Loss: " << calculate_mse_loss(preds, y);
		std::cout << "       "; 
		std::cout << "Num leaves: " << (trees[round].num_leaves + 1) / 2;
		std::cout << "       "; 
		std::cout << "Time Elapsed: " << duration_1.count() << std::endl;
	}
	free(round_preds);
	free(preds);
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

float* GBM::predict_hist(std::vector<std::vector<float>>& X) {
	float* preds = (float*) malloc(sizeof(float) * X[0].size());
	float* tree_preds;

	for (int row = 0; row < int(X[0].size()); ++row) {
		preds[row] = y_mean_train;
	}

	std::vector<std::vector<uint8_t>> X_hist = map_hist_bins_inference(X, bin_mapping);
	std::vector<std::vector<uint8_t>> X_hist_rowmajor = get_hist_bins_rowmajor(X_hist);

	for (int tree_num = 0; tree_num < int(trees.size()); ++tree_num) {
		tree_preds = trees[tree_num].predict_hist(X_hist_rowmajor);
		for (int row = 0; row < int(X[0].size()); ++row) {
			preds[row] += lr * tree_preds[row];
		}
	}
	return preds;
}

std::vector<float> GBM::calculate_gradient(float* preds, std::vector<float>& y) {
	std::vector<float> gradient;
	gradient.reserve(y.size());

	// Assume MSE for now
	for (int idx = 0; idx < int(y.size()); ++idx) {
		gradient.push_back(2.00f * (preds[idx] - y[idx]));
	}
	return gradient;
}


std::vector<float> GBM::calculate_hessian(float* preds, std::vector<float>& y) {
	std::vector<float> hessian(y.size(), 2.00f);
	// Assume MSE for now
	return hessian;
}


float GBM::calculate_mse_loss(float* preds, std::vector<float>& y) {
	float loss = 0.00f;

	// Assume MSE for now
	for (int idx = 0; idx < int(y.size()); ++idx) {
		loss += 0.50f * (preds[idx] - y[idx]) * (preds[idx] - y[idx]);
	}
	loss /= float(y.size());	
	return loss;
}
