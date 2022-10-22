#pragma once

#include <iostream>
#include <vector>

#include "node.hpp"


struct Tree {
	std::vector<std::vector<float>> X;
	std::vector<std::vector<int>> X_hist;
	std::vector<std::vector<int>> orig_col_idxs;
	std::vector<float> gradient;
	std::vector<float> hessian;
	int   tree_num;
	int   max_depth;
	int   max_bin;
	float l2_reg;
	float min_child_weight;
	int   min_data_in_leaf;

	Node* root;

	// Greedy Constructor
	Tree(
			std::vector<std::vector<float>>& X_new,
			std::vector<float>& gradient_new,
			std::vector<float>& hessian_new,
			int&   tree_num_new,
			int&   max_depth_new,
			float& l2_reg_new,
			float& min_child_weight_new,
			int&   min_data_in_leaf_new
		);

	// Histogram Constructor
	Tree(
			std::vector<std::vector<int>>& X_hist_in,
			std::vector<float>& gradient_in,
			std::vector<float>& hessian_in,
			int&   tree_num_in,
			int&   max_depth_in,
			float& l2_reg_in,
			float& min_child_weight_in,
			int&   min_data_in_leaf_in,
			int&   max_bin_in,
			std::vector<std::vector<int>>& min_max_rem_in 
		);

	std::vector<float> predict(std::vector<std::vector<float>>& X_pred);
	std::vector<float> predict_hist(std::vector<std::vector<int>>& X_hist_pred); 
};
