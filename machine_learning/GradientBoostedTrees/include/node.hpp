#pragma once

#include <vector>
#include <array>
#include <unordered_map>


struct Node {
	float 	grad_sum = 0.00f;
	float 	hess_sum = 0.00f;
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
			std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			std::vector<std::vector<float>>& gradient_hist,
			std::vector<std::vector<float>>& hessian_hist,
			float& l2_reg,
			int& min_data_in_leaf,
			int& max_depth,
			int& depth,
			std::vector<std::vector<uint8_t>>& min_max_rem
		);
	void get_hist_split(
			std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			std::vector<std::vector<float>>& gradient_hist,
			std::vector<std::vector<float>>& hessian_hist,
			float& l2_reg,
			int& min_data_in_leaf,
			std::vector<std::vector<uint8_t>>& min_max_rem,
			int& max_bin,
			int& depth,
			int& max_depth
			);
	void get_greedy_split(
			std::vector<std::vector<float>>& X,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			float& l2_reg,
			float& min_child_weight,
			int& min_data_in_leaf,
			int& max_depth,
			int depth
			);

	float calc_gamma(
			std::vector<float>& gradient, 
			std::vector<float>& hessian,
			float& l2_reg
			);
	void  calc_gamma_hist(
			std::vector<float>& gradient_hist_col,
			std::vector<float>& hessian_hist_col,
			float& l2_reg
			);

	float calc_score(
			float& lgs, // Left gradient sum
			float& rgs, // Right gradient sum
			float& lhs, // Left hessian sum
			float& rhs, // Right hessian sum
			float& l2_reg
			);

	float predict_obs(std::vector<float> &obs);
	std::vector<float> predict(std::vector<std::vector<float>> &X_pred);

	float predict_obs_hist(std::vector<uint8_t> &obs);
	std::vector<float> predict_hist(std::vector<std::vector<uint8_t>> &X_hist_pred);

	std::vector<std::vector<float>> calc_bin_statistics(
			std::vector<std::vector<uint8_t>>& X_hist_child,
			std::vector<float>& stat_vector,
			int& max_bin
			);
	std::vector<std::vector<float>> calc_diff_hist(
			std::vector<std::vector<float>>& orig,
			std::vector<std::vector<float>>& child
			); 
};
