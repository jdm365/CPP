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
	unsigned int num_boosting_rounds;
	std::vector<Tree> trees;

	GBM(
			int&   max_depth_new,
			float& l2_reg_new,
			float& lr_new,
			float& min_child_weight_new,
			int&   min_data_in_leaf_new,
			unsigned int& num_boosting_rounds_new
		);

	void train(std::vector<std::vector<float>>& X, std::vector<float>& y);
	std::vector<float> predict(std::vector<std::vector<float>>& X, float y_mean);
	std::vector<float> calculate_gradient(std::vector<float>& preds, std::vector<float>& y);
	std::vector<float> calculate_hessian(std::vector<float>& preds, std::vector<float>& y);
};
