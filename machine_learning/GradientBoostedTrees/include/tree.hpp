#pragma once

#include <cstdlib>
#include <iostream>
#include <vector>
#include <unordered_map>

#include "node.hpp"
#include "utils.hpp"


struct Tree {
	Node* root;
	int num_leaves;

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
	~Tree() {};

	// Histogram Constructor CPU
	Tree(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			int&   max_depth,
			float& l2_reg,
			int&   min_data_in_leaf,
			int&   max_bin,
			int&   max_leaves,
			float& col_subsample_rate,
			std::vector<std::vector<uint8_t>>& min_max_rem,
			int&   tree_num
		);

	// Histogram Constructor GPU
	Tree(
			const cuda_hist& X_hist,
			thrust::device_vector<float>& gradient,
			thrust::device_vector<float>& hessian,
			int&   max_depth,
			float& l2_reg,
			int&   min_data_in_leaf,
			int&   max_bin,
			int&   max_leaves,
			float& col_subsample_rate,
			int&   tree_num
		);

	std::vector<float> predict(std::vector<std::vector<float>>& X_pred);
	std::vector<float> predict_hist(const std::vector<std::vector<uint8_t>>& X_hist_pred); 
	std::vector<int> get_subsample_cols(
			float& col_subsample_rate, 
			int n_cols, 
			int tree_num
			);
	thrust::device_vector<float> predict_hist_gpu(const cuda_hist& X_hist_pred); 
};
