#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <assert.h>

#include "tree.hpp"
#include "node.hpp"
#include "utils.hpp"


/*
****************************************************************
Default Constructor
****************************************************************
*/
Node::Node() {
}

/*
****************************************************************
Greedy Constructor
****************************************************************
*/
Node::Node(
		std::vector<std::vector<float>>& X_new,
		std::vector<float>& gradient_new,
		std::vector<float>& hessian_new,
		float& l2_reg_new,
		float& min_child_weight_new,
		int&   min_data_in_leaf_new,
		int&   max_depth_new,
		int    depth_new
		) {

	X 		 		  = X_new;
	gradient 		  = gradient_new;
	hessian  		  = hessian_new;
	l2_reg	 		  = l2_reg_new;
	min_child_weight  = min_child_weight_new;
	min_data_in_leaf  = min_data_in_leaf_new;
	max_depth	 	  = max_depth_new;
	depth	 		  = depth_new;
	is_leaf			  = (depth >= max_depth);
	split_val		  = 0.00f;
	split_col		  = 0;
	gamma 			  = calc_gamma();

	// Recursively finds child nodes to build tree.
	if (!is_leaf) {
		get_greedy_split();
	}
}

/*
****************************************************************
Histogram Constructor
****************************************************************
*/
Node::Node(
		std::vector<std::vector<uint8_t>>& X_hist,
		std::vector<float>& gradient_new,
		std::vector<float>& hessian_new,
		std::vector<std::vector<float>>& gradient_hist,
		std::vector<std::vector<float>>& hessian_hist,
		float& l2_reg_new,
		int&   min_data_in_leaf_new,
		int&   max_depth_new,
		int&   depth_new,
		std::vector<std::vector<uint8_t>>& min_max_rem
		) {

	depth	= depth_new;
	is_leaf	= (depth_new >= max_depth_new) || (int(X_hist.size()) < min_data_in_leaf_new);
	max_bin	= int(gradient_hist[0].size());
	calc_gamma_hist(gradient_hist[0], hessian_hist[0], l2_reg_new);

	// Needed for prediction.
	//     1. is_leaf
	//     2. gamma
	//     3. split_col
	//     4. split_bin

	// Recursively finds child nodes to build tree.
	if (!is_leaf) {
		get_hist_split(
				X_hist,
				gradient_new,
				hessian_new,
				gradient_hist,
				hessian_hist,
				l2_reg_new,
				min_data_in_leaf_new,
				min_max_rem,
				max_bin,
				max_depth_new
				);
	}
}


float Node::calc_gamma() {
	float gradient_sum = 0.00f;
	float hessian_sum  = 0.00f;

	float eps = 0.00001f;
	for (int idx = 0; idx < int(gradient.size()); ++idx) {
		gradient_sum += gradient[idx];
		hessian_sum  += hessian[idx];
	}
	float gamma = -gradient_sum / (hessian_sum + l2_reg + eps);
	return gamma;
}


void Node::calc_gamma_hist(
		std::vector<float>& gradient_hist_col,
		std::vector<float>& hessian_hist_col,
		float& l2_reg_new
		) {
	float eps = 0.00001f;

	for (int idx = 0; idx < int(gradient_hist_col.size()); ++idx) {
		grad_sum += gradient_hist_col[idx];
		hess_sum += hessian_hist_col[idx];
	}
	gamma = -grad_sum / (hess_sum + l2_reg_new + eps);
}


float Node::calc_score(
		float& lgs,
		float& rgs,
		float& lhs,
		float& rhs
		) {
	// float expr_0 = lgs * lgs / (lhs + l2_reg);
	// float expr_1 = rgs * rgs / (rhs + l2_reg);
	// float expr_2 = (grad_sum) * (grad_sum) / (hess_sum + l2_reg);

	// float score = 0.50f * (expr_0 + expr_1 - expr_2) - gamma; // Actual score
	

	// This returns the relative score (minus constant factors). Might want
	// absolute if implementing `min_gain_to_split`.
	return (lgs * lgs / (lhs + l2_reg)) + (rgs * rgs / (rhs + l2_reg));
}


float Node::calc_score(
		float& lgs,
		float& rgs,
		float& lhs,
		float& rhs,
		float& l2_reg_in
		) {
	// float expr_0 = lgs * lgs / (lhs + l2_reg);
	// float expr_1 = rgs * rgs / (rhs + l2_reg);
	// float expr_2 = (grad_sum) * (grad_sum) / (hess_sum + l2_reg);

	// float score = 0.50f * (expr_0 + expr_1 - expr_2) - gamma; // Actual score
	

	// This returns the relative score (minus constant factors). Might want
	// absolute if implementing `min_gain_to_split`.
	return (lgs * lgs / (lhs + l2_reg_in)) + (rgs * rgs / (rhs + l2_reg_in));
}


float Node::predict_obs(std::vector<float>& obs) {
	if (is_leaf) {
		return gamma;
	}
	if (obs[split_col] < split_val) {
		return (*left_child).predict_obs(obs);
	}
	return (*right_child).predict_obs(obs);
}


std::vector<float> Node::predict(std::vector<std::vector<float>>& X_pred) {
	std::vector<float> preds;
	preds.reserve(X_pred.size());

	// X_pred is rowmajor storage.
	for (int row = 0; row < int(X_pred.size()); ++row) {
		preds.push_back(predict_obs(X_pred[row]));
	}
	return preds;
}


float Node::predict_obs_hist(std::vector<uint8_t>& obs) {
	if (is_leaf) {
		return gamma;
	}
	if (obs[split_col] < split_bin) {
		return (*left_child).predict_obs_hist(obs);
	}
	return (*right_child).predict_obs_hist(obs);
}


std::vector<float> Node::predict_hist(std::vector<std::vector<uint8_t>>& X_hist_pred) {
	std::vector<float> preds;
	preds.reserve(X_hist_pred.size());

	// X_pred is rowmajor storage.
	for (int row = 0; row < int(X_hist_pred.size()); ++row) {
		preds.push_back(predict_obs_hist(X_hist_pred[row]));
	}
	return preds;
}


std::vector<std::vector<float>> Node::calc_bin_statistics(
		std::vector<std::vector<uint8_t>>& X_hist_child,
		std::vector<float>& stat_vector
		) {
	int n_rows = int(X_hist_child.size());
	int n_cols = int(X_hist_child[0].size());

	std::vector<std::vector<float>> stat_hist(
			n_cols,
			std::vector<float>(max_bin, 0.00f)
			);

	for (int row = 0; row < n_rows; ++row) {
		for (int col = 0; col < n_cols; ++col) {
			stat_hist[col][int(X_hist_child[row][col])] += stat_vector[row];
		}
	}
	return stat_hist;
}


std::vector<std::vector<float>> Node::calc_diff_hist(
		std::vector<std::vector<float>>& orig,
		std::vector<std::vector<float>>& child
		) {
	for (int col = 0; col < int(orig.size()); ++col) {
		for (int bin = 0; bin < max_bin; bin++) {
			orig[col][bin] -= child[col][bin];
		}
	}
	return orig;
}


void Node::get_greedy_split() {
	int n_cols = int(X.size());
	int n_rows = int(X[0].size());

	std::vector<float> X_col;

	int   left_sum;
	int   right_sum;
	float left_gradient_sum;
	float right_gradient_sum;
	float left_hessian_sum;
	float right_hessian_sum;

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
					left_sum++;
					left_gradient_sum += gradient[idx];
					left_hessian_sum  += hessian[idx];
				}
				else {
					right_sum++;
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
			gradient_left, 
			hessian_left, 
			l2_reg,
			min_child_weight,
			min_data_in_leaf,
			max_depth,
			depth + 1
			);
	right_child = new Node(
			X_right, 
			gradient_right, 
			hessian_right, 
			l2_reg,
			min_child_weight,
			min_data_in_leaf,
			max_depth,
			depth + 1
			);
}


void Node::get_hist_split(
				std::vector<std::vector<uint8_t>>& X_hist,
				std::vector<float>& gradient_new,
				std::vector<float>& hessian_new,
				std::vector<std::vector<float>>& gradient_hist,
				std::vector<std::vector<float>>& hessian_hist,
				float& l2_reg_new,
				int& min_data_in_leaf_new,
				std::vector<std::vector<uint8_t>>& min_max_rem,
				int& max_bin_new,
				int& max_depth_new
		) {
	int n_rows = int(X_hist.size());
	int n_cols = int(X_hist[0].size());

	int min_bin_col;
	int max_bin_col;

	float left_gradient_sum;
	float right_gradient_sum;
	float left_hessian_sum;
	float right_hessian_sum;

	float score;
	float best_score = -INFINITY;

	int mid_pt;

	for (int col = 0; col < n_cols; ++col) {
		// Get min and max bin in hist col and only iterate over those buckets.
		min_bin_col = int(min_max_rem[col][0]);
		max_bin_col = int(min_max_rem[col][1]);
		
		// If only one bin remaining then skip.
		if (max_bin_col - min_bin_col == 1) {
			continue;
		}

		// Use subtraction trick to get other side grad/hist sum.
		mid_pt = int((max_bin_col - min_bin_col) / 2) + min_bin_col;

		// Start from min_bin_col + 1 
		// Splitting on min_bin would cause left_idxs.size() == 0
		for (int bin = min_bin_col + 1; bin < mid_pt; ++bin) {
			// Reset summary statistics.
			left_gradient_sum = 0.00f;
			left_hessian_sum  = 0.00f;

			for (int idx = min_bin_col; idx < bin; ++idx) {
				left_gradient_sum += gradient_hist[col][idx];
				left_hessian_sum  += hessian_hist[col][idx];
			}

			right_gradient_sum = grad_sum - left_gradient_sum;
			right_hessian_sum  = hess_sum - left_hessian_sum;

			if (left_hessian_sum  <= float(2 * min_data_in_leaf_new) ||
				right_hessian_sum <= float(2 * min_data_in_leaf_new)) {
				continue;
			}

			score = calc_score(
					left_gradient_sum,
					right_gradient_sum,
					left_hessian_sum, 
					right_hessian_sum,
					l2_reg_new
					);
			if (score > best_score) {
				split_bin  = bin;
				split_col  = col;
				best_score = score;
			}
		}

		for (int bin = mid_pt; bin < max_bin_col; ++bin) {
			// Reset summary statistics.
			right_gradient_sum = 0.00f;
			right_hessian_sum  = 0.00f;

			for (int idx = bin; idx < max_bin_col; ++idx) {
				right_gradient_sum += gradient_hist[col][idx];
				right_hessian_sum  += hessian_hist[col][idx];
			}

			left_gradient_sum = grad_sum - right_gradient_sum;
			left_hessian_sum  = hess_sum - right_hessian_sum;

			if (left_hessian_sum  <= float(2 * min_data_in_leaf_new) ||
				right_hessian_sum <= float(2 * min_data_in_leaf_new)) {
				continue;
			}

			score = calc_score(
					left_gradient_sum,
					right_gradient_sum,
					left_hessian_sum, 
					right_hessian_sum,
					l2_reg_new
					);
			if (score > best_score) {
				split_bin  = bin;
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

	std::vector<int> left_idxs;
	std::vector<int> right_idxs;

	left_idxs.reserve(int(n_rows * split_bin / int(min_max_rem[split_col][1])));
	right_idxs.reserve(n_rows - int(n_rows * split_bin / int(min_max_rem[split_col][1])));


	for (int row = 0; row < n_rows; ++row) {
		if (X_hist[row][split_col] < split_bin) {
			left_idxs.push_back(row);
		}
		else {
			right_idxs.push_back(row);
		}
	}

	std::vector<float> gradient_left;
	std::vector<float> gradient_right;
	std::vector<float> hessian_left;
	std::vector<float> hessian_right;

	gradient_left.reserve(left_idxs.size());
	gradient_right.reserve(right_idxs.size());
	hessian_left.reserve(left_idxs.size());
	hessian_right.reserve(right_idxs.size());

	assert(left_idxs.size()  != 0);
	assert(right_idxs.size() != 0);

	std::vector<std::vector<uint8_t>> min_max_rem_left  = min_max_rem;
	std::vector<std::vector<uint8_t>> min_max_rem_right = min_max_rem;

	min_max_rem_left[split_col][1]  = uint8_t(split_bin);
	min_max_rem_right[split_col][0] = uint8_t(split_bin);

	alignas(64) std::vector<std::vector<uint8_t>> X_hist_left;
	alignas(64) std::vector<std::vector<uint8_t>> X_hist_right;

	vector_reserve_2d(X_hist_left,  int(left_idxs.size()), n_cols);
	vector_reserve_2d(X_hist_right, int(right_idxs.size()), n_cols);

	for (const int& row: left_idxs) { 
		X_hist_left.push_back(X_hist[row]);
		gradient_left.push_back(gradient_new[row]);
		hessian_left.push_back(hessian_new[row]);
	}

	for (const int& row: right_idxs) { 
		X_hist_right.push_back(X_hist[row]);
		gradient_right.push_back(gradient_new[row]);
		hessian_right.push_back(hessian_new[row]);
	}
	
	alignas(64) std::vector<std::vector<float>> gradient_hist_left;
	alignas(64) std::vector<std::vector<float>> gradient_hist_right;
	alignas(64) std::vector<std::vector<float>> hessian_hist_left;
	alignas(64) std::vector<std::vector<float>> hessian_hist_right;

	if (left_idxs.size() > right_idxs.size()) {
		gradient_hist_right = calc_bin_statistics(X_hist_right, gradient_right);
		hessian_hist_right  = calc_bin_statistics(X_hist_right, hessian_right);
		gradient_hist_left  = calc_diff_hist(gradient_hist, gradient_hist_right);
		hessian_hist_left   = calc_diff_hist(hessian_hist, hessian_hist_right);
	}
	else {
		gradient_hist_left  = calc_bin_statistics(X_hist_left, gradient_left);
		hessian_hist_left   = calc_bin_statistics(X_hist_left, hessian_left);
		gradient_hist_right = calc_diff_hist(gradient_hist, gradient_hist_left);
		hessian_hist_right  = calc_diff_hist(hessian_hist, hessian_hist_left);
	}
	
	left_child = new Node(
			X_hist_left, 
			gradient_left, 
			hessian_left, 
			gradient_hist_left, 
			hessian_hist_left, 
			l2_reg_new,
			min_data_in_leaf_new,
			max_depth_new,
			++depth,
			min_max_rem_left
			);
	right_child = new Node(
			X_hist_right, 
			gradient_right, 
			hessian_right, 
			gradient_hist_right, 
			hessian_hist_right, 
			l2_reg_new,
			min_data_in_leaf_new,
			max_depth_new,
			++depth,
			min_max_rem_right
			);
}
