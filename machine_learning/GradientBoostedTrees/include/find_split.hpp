#pragma once

#include <vector>

float calc_gamma(
		std::vector<float>& gradient, 
		std::vector<float>& hessian, 
		float& l2_reg
		);

float calc_score(
		float& left_gradient_sum, 
		float& right_gradient_sum, 
		float& left_hessian_sum, 
		float& right_hessian_sum, 
		float& l2_reg,
		float& gamma
		);

std::vector<float> find_greedy_split_point(
		std::vector<std::vector<float>>& X,
		std::vector<float>& gradient,
		std::vector<float>& hessian,
		int&   min_data_in_leaf,
		float& min_child_weight,
		float& l2_reg
		);
