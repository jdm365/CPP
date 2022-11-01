#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

#include "node.hpp"


struct Tree {
	Node* root;

	// Greedy Constructor
	Tree(
			std::vector<std::vector<float>>& X,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			int&   max_depth,
			float& l2_reg,
			float& min_child_weight,
			int&   min_data_in_leaf
		);

	// Histogram Constructor
	Tree(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			int&   max_depth,
			float& l2_reg,
			int&   min_data_in_leaf,
			int&   max_bin,
			std::vector<std::vector<uint8_t>>& min_max_rem 
		);

	std::vector<float> predict(std::vector<std::vector<float>>& X_pred);
	float* predict_hist(const std::vector<std::vector<uint8_t>>& X_hist_pred); 
};
