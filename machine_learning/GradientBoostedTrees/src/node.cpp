#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include <cmath>

#include "node.hpp"
#include "utils.hpp"


Node::Node() {
}

Node::Node(
		std::vector<std::vector<float>>& X_new,
		std::vector<std::vector<float>>& split_vals_new,
		std::vector<float>& gradient_new,
		std::vector<float>& hessian_new,
		int&   tree_num_new,
		float& l2_reg_new,
		float& min_child_weight_new,
		int&   min_data_in_leaf_new,
		int&   max_depth_new,
		int    depth_new
		) {

	X 		 		 = X_new;
	split_vals 		 = split_vals_new;
	gradient 		 = gradient_new;
	hessian  		 = hessian_new;
	tree_num		 = tree_num_new;
	l2_reg	 		 = l2_reg_new;
	min_child_weight = min_child_weight_new;
	min_data_in_leaf = min_data_in_leaf_new;
	max_depth	 	 = max_depth_new;
	depth	 		 = depth_new;
	is_leaf			 = (depth >= max_depth);
	split_val		 = 0.00f;
	split_col		 = 0;
	gamma 			 = calc_gamma();

	// Recursively finds child nodes to build tree.
	if (!is_leaf) {
		//get_greedy_split();
		get_approximate_split(127);
	}
}

float Node::calc_gamma() {
	float gradient_sum = 0.00f;
	float hessian_sum  = 0.00f;
	for (int idx = 0; idx < int(gradient.size()); idx++) {
		gradient_sum += gradient[idx];
		hessian_sum  += hessian[idx];
	}
	float gamma = -gradient_sum / (hessian_sum + l2_reg);
	return gamma;
}


float Node::calc_score(
		float& left_gradient_sum,
		float& right_gradient_sum,
		float& left_hessian_sum,
		float& right_hessian_sum
		) {
	float expr_0 = std::pow(left_gradient_sum, 2) / (left_hessian_sum + l2_reg);
	float expr_1 = std::pow(right_gradient_sum, 2) / (right_hessian_sum + l2_reg);
	float expr_2 = std::pow((left_gradient_sum + right_gradient_sum), 2) 
							 / (left_hessian_sum + right_hessian_sum + l2_reg);

	float score = 0.50f * (expr_0 + expr_1 - expr_2) - gamma;
	return score;
}

void Node::get_greedy_split() {
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
	float score;
	float best_score = -INFINITY;

	for (int col = 0; col < n_cols; col++) {
		X_col = X[col];
		for (int row = 0; row < n_rows; row++) {
			// Reset summary statistics.
			left_sum 		   = 0;
			right_sum 		   = 0;
			left_gradient_sum  = 0.00f;
			right_gradient_sum = 0.00f;
			left_hessian_sum   = 0.00f;
			right_hessian_sum  = 0.00f;

			// Look at each potential split point.
			for (int idx = 0; idx < n_rows; idx++) {
				if (X_col[idx] <= X_col[row]) {
					left_sum += 1;
					left_gradient_sum += gradient[idx];
					left_hessian_sum  += hessian[idx];
				}
				else {
					right_sum += 1;
					right_gradient_sum += gradient[idx];
					right_hessian_sum  += hessian[idx];
				}
			}
			cond_0 = (left_sum  < min_data_in_leaf);
			cond_1 = (right_sum < min_data_in_leaf);
			cond_2 = (left_hessian_sum  < min_child_weight);
			cond_3 = (right_hessian_sum < min_child_weight);

			if (cond_0 || cond_1 || cond_2 || cond_3) {
				continue;
			}

			score = calc_score(
					left_gradient_sum,
					right_gradient_sum,
					left_hessian_sum, 
					right_hessian_sum
					);
			if (score > best_score) {
				split_val  = X_col[row];
				split_col  = col;
				best_score = score;
			}
		}
	}
	if (best_score == -INFINITY) {
		// If no split was found then node is leaf.
		is_leaf = true;
		return;
	}

	std::vector<std::vector<float>> X_left;
	std::vector<std::vector<float>> X_right;
	
	std::vector<float> X_left_col;
	std::vector<float> X_right_col;

	std::vector<float> gradient_left;
	std::vector<float> gradient_right;
	std::vector<float> hessian_left;
	std::vector<float> hessian_right;

	for (int col = 0; col < n_cols; col++) {
		for (int row = 0; row < n_rows; row++) {
			if (X[split_col][row] <= split_val) {
				X_left_col.push_back(X[col][row]);
				if (col == 0) {
					gradient_left.push_back(gradient[row]);
					hessian_left.push_back(hessian[row]);
				}
			}
			else {
				X_right_col.push_back(X[col][row]);
				if (col == 0) {
					gradient_right.push_back(gradient[row]);
					hessian_right.push_back(hessian[row]);
				}
			}
		}
		X_left.push_back(X_left_col);
		X_right.push_back(X_right_col);

		// Delete all elements of col vectors.
		X_left_col.clear();
		X_right_col.clear();
	}

	left_child = new Node(
			X_left, 
			split_vals,
			gradient_left, 
			hessian_left, 
			tree_num,
			l2_reg,
			min_child_weight,
			min_data_in_leaf,
			max_depth,
			depth + 1
			);
	right_child = new Node(
			X_right, 
			split_vals,
			gradient_right, 
			hessian_right, 
			tree_num,
			l2_reg,
			min_child_weight,
			min_data_in_leaf,
			max_depth,
			depth + 1
			);
}

// Naive quantile based histogram split.
void Node::get_approximate_split(int max_bins) {
	int n_cols = int(X.size());
	int n_rows = int(X[0].size());

	std::vector<float> X_col;
	// std::vector<float> split_vals;

	float left_sum 			 = 0.00f;
	float right_sum			 = 0.00f;
	float left_gradient_sum  = 0.00f;
	float right_gradient_sum = 0.00f;
	float left_hessian_sum   = 0.00f;
	float right_hessian_sum  = 0.00f;

	bool  cond_0, cond_1, cond_2, cond_3;
	float score;
	float best_score = -INFINITY;

	for (int col = 0; col < n_cols; col++) {
		X_col = X[col];
		// split_vals = get_quantiles(X_col, max_bins);
		for (int quantile_idx = 0; quantile_idx < int(split_vals[col].size()); quantile_idx++) {
			// Reset summary statistics.
			left_sum 		   = 0;
			right_sum 		   = 0;
			left_gradient_sum  = 0.00f;
			right_gradient_sum = 0.00f;
			left_hessian_sum   = 0.00f;
			right_hessian_sum  = 0.00f;

			// Look at each potential split point.
			for (int idx = 0; idx < n_rows; idx++) {
				if (X_col[idx] <= split_vals[col][quantile_idx]) {
					left_sum += 1;
					left_gradient_sum += gradient[idx];
					left_hessian_sum  += hessian[idx];
				}
				else {
					right_sum += 1;
					right_gradient_sum += gradient[idx];
					right_hessian_sum  += hessian[idx];
				}
			}
			cond_0 = (left_sum  < min_data_in_leaf);
			cond_1 = (right_sum < min_data_in_leaf);
			cond_2 = (left_hessian_sum  < min_child_weight);
			cond_3 = (right_hessian_sum < min_child_weight);

			if (cond_0 || cond_1 || cond_2 || cond_3) {
				continue;
			}

			score = calc_score(
					left_gradient_sum,
					right_gradient_sum,
					left_hessian_sum, 
					right_hessian_sum
					);
			if (score > best_score) {
				split_val  = split_vals[col][quantile_idx];
				split_col  = col;
				best_score = score;
			}
		}
	}
	if (best_score == -INFINITY) {
		// If no split was found then node is leaf.
		is_leaf = true;
		return;
	}

	std::vector<std::vector<float>> X_left;
	std::vector<std::vector<float>> X_right;
	
	std::vector<float> X_left_col;
	std::vector<float> X_right_col;

	std::vector<float> gradient_left;
	std::vector<float> gradient_right;
	std::vector<float> hessian_left;
	std::vector<float> hessian_right;

	for (int col = 0; col < n_cols; col++) {
		for (int row = 0; row < n_rows; row++) {
			if (X[split_col][row] <= split_val) {
				X_left_col.push_back(X[col][row]);
				if (col == 0) {
					gradient_left.push_back(gradient[row]);
					hessian_left.push_back(hessian[row]);
				}
			}
			else {
				X_right_col.push_back(X[col][row]);
				if (col == 0) {
					gradient_right.push_back(gradient[row]);
					hessian_right.push_back(hessian[row]);
				}
			}
		}
		X_left.push_back(X_left_col);
		X_right.push_back(X_right_col);

		// Delete all elements of col vectors.
		X_left_col.clear();
		X_right_col.clear();
	}

	left_child = new Node(
			X_left, 
			split_vals,
			gradient_left, 
			hessian_left, 
			tree_num,
			l2_reg,
			min_child_weight,
			min_data_in_leaf,
			max_depth,
			depth + 1
			);
	right_child = new Node(
			X_right, 
			split_vals,
			gradient_right, 
			hessian_right, 
			tree_num,
			l2_reg,
			min_child_weight,
			min_data_in_leaf,
			max_depth,
			depth + 1
			);
}


float Node::predict_obs(std::vector<float>& obs) {
	if (is_leaf) {
		return gamma;
	}
	if (obs[split_col] <= split_val) {
		return (*left_child).predict_obs(obs);
	}
	return (*right_child).predict_obs(obs);
}


std::vector<float> Node::predict(std::vector<std::vector<float>>& X_pred) {
	std::vector<float> preds;
	// X_pred is rowwise storage.
	for (int row = 0; row < int(X_pred.size()); row++) {
		preds.push_back(predict_obs(X_pred[row]));
	}
	return preds;
}

std::vector<float> Node::sort_values(std::vector<float> X_col) {
	std::sort(X_col.begin(), X_col.end());
	return X_col;
}

std::vector<float> Node::get_quantiles(std::vector<float> X_col, int n_bins) {
	std::vector<float> split_vals;

	X_col = sort_values(X_col);
	// Init to random. Get unique quantiles.
	float last_val = INFINITY;
	int split_idx;
	int bin_size = int(int(X_col.size()) / n_bins);

	for (int idx = 0; idx < n_bins; idx++) {
		split_idx = idx * bin_size;

		// Only get unique quantiles.
		if (X_col[split_idx] == last_val) {
			continue;
		}

		last_val = X_col[split_idx];
		split_vals.push_back(X_col[split_idx]);
	}
	return split_vals;
}
