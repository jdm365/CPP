#pragma once

#include <iostream>
#include <vector>

#include "node.hpp"


struct Tree {
	std::vector<std::vector<float>> X;
	std::vector<std::vector<int>> orig_col_idxs;
	std::vector<std::vector<float>> split_vals;
	std::vector<float> gradient;
	std::vector<float> hessian;
	int   tree_num;
	int   max_depth;
	int   n_bins;
	float l2_reg;
	float min_child_weight;
	int   min_data_in_leaf;

	Node* root;

	// Greedy Constructor
	Tree(
			std::vector<std::vector<float>>& X_new,
			std::vector<std::vector<float>>& split_vals_new,
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
			std::vector<std::vector<float>>& X_new,
			std::vector<std::vector<int>>& orig_col_idxs_new,
			std::vector<float>& gradient_new,
			std::vector<float>& hessian_new,
			int&   tree_num_new,
			int&   max_depth_new,
			float& l2_reg_new,
			float& min_child_weight_new,
			int&   min_data_in_leaf_new,
			int&   n_bins_new
		);

	std::vector<float> predict(std::vector<std::vector<float>>& X_pred);
};
