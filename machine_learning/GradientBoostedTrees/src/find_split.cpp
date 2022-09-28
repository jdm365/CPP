#include <iostream>
#include <vector>
#include <cmath>

#include "find_split.hpp"


float calc_gamma(
		std::vector<float>& gradient, 
		std::vector<float>& hessian, 
		float& l2_reg
		) {

	float gradient_sum = 0.00f;
	float hessian_sum = 0.00f;
	for (int idx = 0; idx < int(gradient.size()); idx++) {
		gradient_sum += gradient[idx];
		hessian_sum  += hessian[idx];
	}

	float gamma = -gradient_sum / (hessian_sum + l2_reg);
	return gamma;
}


float calc_score(
		float& left_gradient_sum,
		float& right_gradient_sum,
		float& left_hessian_sum,
		float& right_hessian_sum,
		float& l2_reg,
		float& gamma
		) {
	float expr_0 = std::pow(left_gradient_sum, 2) / (left_hessian_sum + l2_reg);
	float expr_1 = std::pow(right_gradient_sum, 2) / (right_hessian_sum + l2_reg);
	float expr_2 = std::pow((left_gradient_sum * right_gradient_sum), 2) 
							 / (left_hessian_sum + right_hessian_sum + l2_reg);

	float score = 0.5 * (expr_0 + expr_1 - expr_2) - gamma;
	return score;
}


std::vector<float> find_greedy_split_point(
		std::vector<std::vector<float>>& X,
		std::vector<float>& gradient,
		std::vector<float>& hessian,
		int&   min_data_in_leaf,
		float& min_child_weight,
		float& l2_reg
		) {
	int n_cols = int(X.size());
	int n_rows = int(X[0].size());

	std::vector<float> X_col;

	float left_sum 			 = 0.00f;
	float right_sum			 = 0.00f;
	float left_gradient_sum  = 0.00f;
	float right_gradient_sum = 0.00f;
	float left_hessian_sum   = 0.00f;
	float right_hessian_sum  = 0.00f;

	bool  cond_0, cond_1, cond_2, cond_3;
	int   split_col;
	float split_val;
	float score, best_score;
	float gamma = calc_gamma(gradient, hessian, l2_reg);

	for (int col = 0; col < n_cols; col++) {
		X_col = X[col];
		for (int row = 0; row < n_rows; row++) {
			for (int idx = 0; idx < n_rows; idx++) {
				if (X_col[idx] <= X_col[row]) {
					left_sum += X_col[idx];
					left_gradient_sum += gradient[idx];
					left_hessian_sum  += hessian[idx];
				}
				else {
					right_sum += X_col[idx];
					right_gradient_sum += gradient[idx];
					right_hessian_sum  += hessian[idx];
				}
			}
			cond_0 = (left_sum  < float(min_data_in_leaf));
			cond_1 = (right_sum < float(min_data_in_leaf));
			cond_2 = (left_hessian_sum  < min_child_weight);
			cond_3 = (right_hessian_sum < min_child_weight);

			if (cond_0 || cond_1 || cond_2 || cond_3) {
				continue;
			}

			score = calc_score(
					left_gradient_sum,
					right_gradient_sum,
					left_hessian_sum, 
					right_hessian_sum,
					l2_reg,
					gamma
					);
			if (score > best_score) {
				split_val  = X_col[row];
				split_col  = col;
				best_score = score;
			}
		}
	}
	std::vector<float> split_point = {split_val, float(split_col)};
	return split_point;
}
