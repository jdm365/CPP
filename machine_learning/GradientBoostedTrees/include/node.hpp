#pragma once

#include <vector>
#include <array>
#include <unordered_map>


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
			std::vector<std::vector<float>>& gradient_hist,
			std::vector<std::vector<float>>& hessian_hist,
			float& l2_reg,
			int& min_data_in_leaf,
			int& max_depth,
			int& depth,
			std::vector<std::vector<uint8_t>>& min_max_rem
		);
	void get_hist_split(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			float& grad_sum,
			float& hess_sum,
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

	std::vector<std::vector<float>> calc_bin_statistics(
			const std::vector<std::vector<uint8_t>>& X_hist_child,
			std::vector<float>& stat_vector,
			int& max_bin
			);
	std::vector<std::vector<float>> calc_diff_hist(
			std::vector<std::vector<float>>& orig,
			std::vector<std::vector<float>>& child
			); 
};
