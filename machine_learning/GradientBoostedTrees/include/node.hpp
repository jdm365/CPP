#pragma once

#include <vector>
#include <array>
#include <unordered_map>


struct Node {
	std::vector<std::vector<float>> X;
	std::vector<std::vector<int>> X_hist;
	std::vector<std::vector<float>> split_vals;
	std::vector<float> gradient;
	std::vector<float> hessian;
	std::vector<std::vector<float>> gradient_hist;
	std::vector<std::vector<float>> hessian_hist;
	int   	tree_num;
	int   	max_depth;
	int   	depth;
	float 	gamma;
	float 	l2_reg;
	float 	min_child_weight;
	int   	min_data_in_leaf;
	int     split_col;
	int		split_bin;
	int   	n_bins;
	bool  	is_leaf;
	float 	split_val;
	Node* 	left_child;
	Node* 	right_child;

	// Default Constructor
	Node();

	// Greedy Constructor
	Node(
			std::vector<std::vector<float>>& X_new,
			std::vector<std::vector<float>>& split_vals_new,
			std::vector<float>& gradient_new,
			std::vector<float>& hessian_new,
			int&   tree_num_new,
			float& l2_reg_new,
			float& min_child_weight_new,
			int& min_data_in_leaf_new,
			int& max_depth_new,
			int depth_new
		);

	// Histogram Constructor
	Node(
			std::vector<std::vector<int>>& X_hist_new,
			std::vector<float>& gradient_new,
			std::vector<float>& hessian_new,
			std::vector<std::vector<float>>& gradient_hist_new,
			std::vector<std::vector<float>>& hessian_hist_new,
			int&   tree_num_new,
			float& l2_reg_new,
			float& min_child_weight_new,
			int& min_data_in_leaf_new,
			int& max_depth_new,
			int depth_new
		);
	void get_hist_split();

	float calc_gamma();
	float calc_score(
			float& left_gradient_sum, 
			float& right_gradient_sum, 
			float& left_hessian_sum, 
			float& right_hessian_sum
			);
	void get_greedy_split();
	void get_approximate_split();

	float predict_obs(std::vector<float> &obs);
	std::vector<float> predict(std::vector<std::vector<float>> &X_pred);
	std::vector<std::vector<float>> calc_bin_statistics(
			std::vector<std::vector<int>>& X_hist_child,
			std::vector<float>& stat_vector
			);
	std::vector<std::vector<float>> calc_diff_hist(
			std::vector<std::vector<float>>& orig,
			std::vector<std::vector<float>>& child
			); 
};
