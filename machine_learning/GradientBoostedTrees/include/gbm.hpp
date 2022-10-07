#pragma once

#include <iostream>
#include <vector>

#include "node.hpp"
#include "tree.hpp"

// GBM -> GradientBoostingMachine - Not enough clarity to justify verbosity.

struct GBM {
	int   max_depth;
	float l2_reg;
	float lr;
	float min_child_weight;
	int   min_data_in_leaf;
	int   num_boosting_rounds;
	std::vector<std::vector<int>> orig_col_idxs;
	std::vector<Tree> trees;

	GBM(
			int   max_depth_new,
			float l2_reg_new,
			float lr_new,
			float min_child_weight_new,
			int   min_data_in_leaf_new,
			int   num_boosting_rounds_new
		);

	void train_greedy(
			std::vector<std::vector<float>>& X, 
			std::vector<std::vector<float>>& X_rowwise, 
			std::vector<float>& y
			);
	void train_hist(
			std::vector<std::vector<float>>& X, 
			std::vector<std::vector<float>>& X_rowwise, 
			std::vector<float>& y
			);
	std::vector<float> predict(std::vector<std::vector<float>>& X);
	std::vector<float> calculate_gradient(std::vector<float>& preds, std::vector<float>& y);
	std::vector<float> calculate_hessian(std::vector<float>& preds, std::vector<float>& y);
	float calculate_mse_loss(std::vector<float>& preds, std::vector<float>& y); 
	std::vector<float> get_quantiles(std::vector<float> X_col, int n_bins);
	std::vector<int>   get_sorted_idxs(std::vector<float> X_col, int n_bins);
};
