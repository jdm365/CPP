#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include <unordered_map>
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

	X 		 		  = X_new;
	split_vals 		  = split_vals_new;
	gradient 		  = gradient_new;
	hessian  		  = hessian_new;
	tree_num		  = tree_num_new;
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
		//get_greedy_split();
		get_approximate_split();
	}
}

/*
****************************************************************
Histogram Constructor
****************************************************************
*/
Node::Node(
		std::vector<std::vector<int>>& X_hist_new,
		std::vector<float>& gradient_new,
		std::vector<float>& hessian_new,
		std::vector<std::vector<float>>& gradient_hist_new,
		std::vector<std::vector<float>>& hessian_hist_new,
		int&   tree_num_new,
		float& l2_reg_new,
		float& min_child_weight_new,
		int&   min_data_in_leaf_new,
		int&   max_depth_new,
		int    depth_new
		) {

	X_hist 		 	  = X_hist_new;
	gradient 	  	  = gradient_new;
	hessian  	  	  = hessian_new;
	gradient_hist 	  = gradient_hist_new;
	hessian_hist  	  = hessian_hist_new;
	tree_num		  = tree_num_new;
	l2_reg	 		  = l2_reg_new;
	min_child_weight  = min_child_weight_new;
	min_data_in_leaf  = min_data_in_leaf_new;
	max_depth	 	  = max_depth_new;
	depth	 		  = depth_new;
	is_leaf			  = (depth >= max_depth);
	split_bin		  = 0;
	split_col		  = 0;
	gamma 			  = calc_gamma();


	// Recursively finds child nodes to build tree.
	if (!is_leaf) {
		get_hist_split();
	}
}


float Node::calc_gamma() {
	float gradient_sum = 0.00f;
	float hessian_sum  = 0.00f;

	float eps = 0.00001f;
	for (int idx = 0; idx < int(gradient.size()); idx++) {
		gradient_sum += gradient[idx];
		hessian_sum  += hessian[idx];
	}
	float gamma = -gradient_sum / (hessian_sum + l2_reg + eps);
	return gamma;
}


float Node::calc_score(
		float& left_gradient_sum,
		float& right_gradient_sum,
		float& left_hessian_sum,
		float& right_hessian_sum
		) {
	float expr_0 = left_gradient_sum * left_gradient_sum / (left_hessian_sum + l2_reg);
	float expr_1 = right_gradient_sum * right_gradient_sum / (right_hessian_sum + l2_reg);
	float expr_2 = (left_gradient_sum + right_gradient_sum) *  (left_gradient_sum + right_gradient_sum)
							 / (left_hessian_sum + right_hessian_sum + l2_reg);

	float score = 0.50f * (expr_0 + expr_1 - expr_2) - gamma;
	return score;
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

	/*
	TRY PRECOMPUTING GRADIENT/HESSIAN SUMS IN BUCKETS.
	
	=============================================================
	-------------------------------------------------------------
	Approximate Histogram Algorithm
	-------------------------------------------------------------

	*************************************************************
	1) Get sorted histogram integer mapping
	   construction before training begins.
	2) In the node initialization phase calculate 
	   gradient and hessian statistics for each bin.
	3) Choose best split bin.
	4) When creating child nodes, pass bins still in data.

	*************************************************************
	=============================================================
*/

void Node::get_hist_split() {
	int n_cols = int(X_hist.size());
	int n_rows = int(X_hist[0].size());

	int n_bins   = int(gradient_hist[0].size());
	int bin_size = std::ceil(n_rows / n_bins);

	int   left_sum;
	int   right_sum;
	float left_gradient_sum;
	float right_gradient_sum;
	float left_hessian_sum;
	float right_hessian_sum;

	bool  cond_0, cond_1, cond_2, cond_3;
	float score;
	float best_score = -INFINITY;

	for (int col = 0; col < n_cols; ++col) {
		for (int bin = 0; bin < n_bins; ++bin) {
			// Reset summary statistics.
			left_sum 		   = 0;
			right_sum 		   = 0;
			left_gradient_sum  = 0.00f;
			right_gradient_sum = 0.00f;
			left_hessian_sum   = 0.00f;
			right_hessian_sum  = 0.00f;

			// Look at each potential split point.
			for (int idx = 0; idx < bin; ++idx) {
				left_sum += bin_size;
				left_gradient_sum += gradient_hist[col][idx];
				left_hessian_sum  += hessian_hist[col][idx];
			}
			for (int idx = bin; idx < n_bins; ++idx) {
				right_sum += bin_size;
				right_gradient_sum += gradient_hist[col][idx];
				right_hessian_sum  += hessian_hist[col][idx];
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
			if (score >= best_score) {
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


	std::vector<float> gradient_left;
	std::vector<float> gradient_right;
	std::vector<float> hessian_left;
	std::vector<float> hessian_right;

	std::vector<int> left_idxs;
	std::vector<int> right_idxs;

	for (int row = 0; row < n_rows; ++row) {
		if (X_hist[split_col][row] <= split_bin) {
			left_idxs.push_back(row);
			gradient_left.push_back(gradient[row]);
			hessian_left.push_back(hessian[row]);
		}
		else {
			right_idxs.push_back(row);
			gradient_right.push_back(gradient[row]);
			hessian_right.push_back(hessian[row]);
		}
	}

	std::vector<std::vector<int>> X_hist_left(
			n_cols,
			std::vector<int>(left_idxs.size())
			);
	std::vector<std::vector<int>> X_hist_right(
			n_cols,
			std::vector<int>(right_idxs.size())
			);

	int idx;
	for (int col = 0; col < n_cols; ++col) {
		idx = 0;
		for (const int& row: left_idxs) { 
			X_hist_left[col][idx] = X_hist[col][row];
			idx++;
		}
	}

	for (int col = 0; col < n_cols; ++col) {
		idx = 0;
		for (const int& row: right_idxs) { 
			X_hist_right[col][idx] = X_hist[col][row];
			idx++;
		}
	}

	std::vector<std::vector<float>> gradient_hist_left = calc_bin_statistics(
			X_hist_left,
			gradient_left,
			n_bins
			);
	std::vector<std::vector<float>> hessian_hist_left = calc_bin_statistics(
			X_hist_left,
			hessian_left,
			n_bins
			);
	std::vector<std::vector<float>> gradient_hist_right = calc_diff_hist(
			gradient_hist,
			gradient_hist_left,
			n_bins
			);
	std::vector<std::vector<float>> hessian_hist_right = calc_diff_hist(
			hessian_hist,
			hessian_hist_left,
			n_bins
			);

	left_child = new Node(
			X_hist_left, 
			gradient_left, 
			hessian_left, 
			gradient_hist_left, 
			hessian_hist_left, 
			tree_num,
			l2_reg,
			min_child_weight,
			min_data_in_leaf,
			max_depth,
			depth + 1
			);
	right_child = new Node(
			X_hist_right, 
			gradient_right, 
			hessian_right, 
			gradient_hist_right, 
			hessian_hist_right, 
			tree_num,
			l2_reg,
			min_child_weight,
			min_data_in_leaf,
			max_depth,
			depth + 1
			);
}


// Naive quantile based histogram split.
void Node::get_approximate_split() {
	int n_cols = int(X.size());
	int n_rows = int(X[0].size());

	std::vector<float> X_col;

	int   left_sum 			 = 0;
	int   right_sum			 = 0;
	float left_gradient_sum  = 0.00f;
	float right_gradient_sum = 0.00f;
	float left_hessian_sum   = 0.00f;
	float right_hessian_sum  = 0.00f;

	bool  cond_0, cond_1, cond_2, cond_3;
	float score;
	float best_score = -INFINITY;
	int   n_bins;



	for (int col = 0; col < n_cols; col++) {
		X_col = X[col];
		n_bins = int(split_vals[col].size());
		for (int quantile_idx = 0; quantile_idx < n_bins; quantile_idx++) {
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


std::vector<std::vector<float>> Node::calc_bin_statistics(
		std::vector<std::vector<int>>& X_hist_child,
		std::vector<float>& stat_vector,
		int& n_bins
		) {
	int n_rows = int(X_hist_child[0].size());
	int n_cols = int(X_hist_child.size());

	std::vector<std::vector<float>> stat_mapping(n_cols, std::vector<float>(n_bins));

	for (int col = 0; col < n_cols; ++col) {
		for (int row = 0; row < n_rows; ++row) {
			stat_mapping[col][X_hist_child[col][row]] += stat_vector[row];
		}
	}
	return stat_mapping;
}

std::vector<std::vector<float>> Node::calc_diff_hist(
		std::vector<std::vector<float>>& orig,
		std::vector<std::vector<float>>& child,
		int& n_bins
		) {
	int n_cols = int(orig.size());

	std::vector<std::vector<float>> other_child(n_cols, std::vector<float>(n_bins)); 


	for (int col = 0; col < n_cols; ++col) {
		for (int bin = 0; bin < n_bins; bin++) {
			other_child[col][bin] = orig[col][bin] - child[col][bin];
		}
	}
	return other_child;
}
