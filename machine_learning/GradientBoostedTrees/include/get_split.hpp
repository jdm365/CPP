#pragma once

#include <vector>
#include <array>


struct Node {
	std::vector<std::vector<float>> X;
	int   depth;
	std::vector<float> gradient;
	std::vector<float> hessian;
	float gamma;
	float l2_reg;
	int   min_data_in_leaf;
	float min_child_weight;

	Node(
			std::vector<std::vector<float>>& X_new,
			std::vector<float>& gradient_new,
			std::vector<float>& hessian_new,
			float& l2_reg_new,
			int& min_data_in_leaf_new,
			float& min_child_weight_new,
			int depth_new = 0
		);

	float calc_gamma();

	float calc_score(
			float& left_gradient_sum, 
			float& right_gradient_sum, 
			float& left_hessian_sum, 
			float& right_hessian_sum
			);

	void get_greedy_split();
};
