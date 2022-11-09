#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>

#include "node.hpp"
#include "tree.hpp"

// GBM -> GradientBoostingMachine

struct GBM {
	int   max_depth;
	float l2_reg;
	float lr;
	float min_child_weight;
	float y_mean_train;
	int   min_data_in_leaf;
	int   num_boosting_rounds;
	int   max_bin;
	int   max_leaves;
	std::vector<Tree> trees;
	std::vector<std::vector<float>> bin_mapping;

	GBM(
			int   max_depth_new,
			float l2_reg_new,
			float lr_new,
			float min_child_weight_new,
			int   min_data_in_leaf_new,
			int   num_boosting_rounds_new,
			int   max_bin_new,
			int   max_leaves_new
		);

	void train_greedy(
			std::vector<std::vector<float>>& X, 
			std::vector<float>& y
			);
	void train_hist(
			std::vector<std::vector<float>>& X, 
			std::vector<float>& y
			);

	std::vector<float> predict(std::vector<std::vector<float>>& X_rowmajor);
	float* predict_hist(std::vector<std::vector<float>>& X);
	std::vector<float> calculate_gradient(float* preds, std::vector<float>& y);
	std::vector<float> calculate_hessian(float* preds, std::vector<float>& y);
	float calculate_mse_loss(float* preds, std::vector<float>& y); 
};
