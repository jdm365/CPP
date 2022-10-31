#pragma once

#include <vector>
#include <array>
#include <unordered_map>

#include "histogram.hpp"

struct Node {
	float 	gamma;
	int     split_col;
	int		split_bin;
	float 	split_val;
	bool  	is_leaf;

	Node* 	left_child;
	Node* 	right_child;

	// Default Constructor
	Node();

	// Greedy Constructor
	Node(
			std::vector<std::vector<float>>& X,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			float& l2_reg,
			float& min_child_weight,
			int& min_data_in_leaf,
			int& max_depth,
			int depth
		);

	// Histogram Constructor
	Node(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			Histograms& hists,
			std::vector<int>& row_idxs,
			float& l2_reg,
			int& min_data_in_leaf,
			int& max_depth,
			int  depth,
			int& max_bin,
			std::vector<std::vector<uint8_t>>& min_max_rem
		);
	void get_hist_split(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			Histograms& hists,
			std::vector<int>& row_idxs,
			float& grad_sum,
			float& hess_sum,
			float& l2_reg,
			int& min_data_in_leaf,
			std::vector<std::vector<uint8_t>>& min_max_rem,
			int& max_bin,
			int  depth,
			int& max_depth
			);
	void get_greedy_split(
			std::vector<std::vector<float>>& X,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			float& grad_sum,
			float& hess_sum,
			float& l2_reg,
			float& min_child_weight,
			int& min_data_in_leaf,
			int& max_depth,
			int depth
			);

	void calc_gamma(
			float& grad_sum, 
			float& hess_sum,
			float& l2_reg
			);

	float calc_score(
			float& lgs, // Left gradient sum
			float& rgs, // Right gradient sum
			float& lhs, // Left hessian sum
			float& rhs, // Right hessian sum
			float& l2_reg
			);

	float predict_obs(std::vector<float>& obs);
	std::vector<float> predict(std::vector<std::vector<float>>& X_pred);

	float predict_obs_hist(const std::vector<uint8_t>& obs);
	float* predict_hist(const std::vector<std::vector<uint8_t>>& X_hist_pred);
};
