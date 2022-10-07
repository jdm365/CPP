#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <assert.h>

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
		std::vector<std::vector<float>>& X_new,
		std::vector<std::vector<int>>& orig_col_idxs_new,
		std::vector<float>& gradient_new,
		std::vector<float>& hessian_new,
		int&   tree_num_new,
		float& l2_reg_new,
		float& min_child_weight_new,
		int&   min_data_in_leaf_new,
		int&   max_depth_new,
		int    depth_new,
		int&   n_bins_new
		) {

	X 		 		  = X_new;
	orig_col_idxs 	  = orig_col_idxs_new;
	gradient 		  = gradient_new;
	hessian  		  = hessian_new;
	tree_num		  = tree_num_new;
	l2_reg	 		  = l2_reg_new;
	min_child_weight  = min_child_weight_new;
	min_data_in_leaf  = min_data_in_leaf_new;
	max_depth	 	  = max_depth_new;
	depth	 		  = depth_new;
	n_bins	 		  = n_bins_new;
	is_leaf			  = (depth >= max_depth);
	split_val		  = 0.00f;
	split_col		  = 0;
	gamma 			  = calc_gamma();


	// Recursively finds child nodes to build tree.
	if (!is_leaf) {
		get_hist_statistics();
		get_hist_split();
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

	/*
	TRY PRECOMPUTING GRADIENT/HESSIAN SUMS IN BUCKETS.
	
	=============================================================
	-------------------------------------------------------------
	Approximate Histogram Algorithm
	-------------------------------------------------------------

	*************************************************************
	1) Get sorted indices upon initial histogram 
	   construction before training begins.
	2) In the node initialization phase calculate 
	   gradient and hessian statistics for each bin.
	3) Choose best split bin.
	4) When creating child nodes, pass bins still in data.

	*************************************************************
	=============================================================
*/

void Node::get_hist_statistics() {
	int orig_idx = 0;
	int bin_size = std::max(1, int(orig_col_idxs[0].size() / n_bins));
	float gradient_sum;
	float hessian_sum;
	std::vector<float> gradient_sums_col;
	std::vector<float> hessian_sums_col;
	std::vector<float> split_vals_col;

	gradient_sums_col.reserve(X[0].size());
	hessian_sums_col.reserve(X[0].size());
	split_vals_col.reserve(X[0].size());

	for (int col = 0; col < int(X.size()); col++) {
		for (int idx = 0; idx < n_bins; idx++) {
			// Reset summary statistics.
			gradient_sum  = 0.00f;
			hessian_sum   = 0.00f;

			for (
					int row = idx * bin_size; 
					row < std::min(int(orig_col_idxs[0].size()), (idx + 1) * bin_size); 
					row++
					) {
				assert(orig_col_idxs[col].size() > row);
				orig_idx = orig_col_idxs[col][row];	
				gradient_sum += gradient[orig_idx];
				hessian_sum  += hessian[orig_idx];
			}
			gradient_sums_col.push_back(gradient_sum);
			hessian_sums_col.push_back(hessian_sum);
			split_vals_col.push_back(X[col][orig_idx]);
		}
		gradient_sums.push_back(gradient_sums_col);
		hessian_sums.push_back(hessian_sums_col);
		split_vals.push_back(split_vals_col);

		gradient_sums_col.clear();
		hessian_sums_col.clear();
		split_vals.clear();
	}
}
	
void Node::get_hist_split() {
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
	int   bin_size;

	for (int col = 0; col < n_cols; col++) {
		X_col = X[col];
		bin_size = int(n_rows / n_bins);
		for (int quantile_idx = 1; quantile_idx < n_bins - 1; quantile_idx++) {
			// Reset summary statistics.
			left_sum 		   = 0;
			right_sum 		   = 0;
			left_gradient_sum  = 0.00f;
			right_gradient_sum = 0.00f;
			left_hessian_sum   = 0.00f;
			right_hessian_sum  = 0.00f;

			// Look at each potential split point.
			for (int bin = 0; bin < quantile_idx; bin++) {
				left_sum += bin_size;
				left_gradient_sum += gradient_sums[col][bin];
				left_hessian_sum  += hessian_sums[col][bin];
			}
			for (int bin = quantile_idx; bin < n_bins; bin++) {
				right_sum += bin_size;
				right_gradient_sum += gradient_sums[col][bin];
				right_hessian_sum  += hessian_sums[col][bin];
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
				split_val  = X_col[orig_col_idxs[col][int(bin_size * quantile_idx)]];
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
	std::vector<float> gradient_left;
	std::vector<float> gradient_right;
	std::vector<float> hessian_left;
	std::vector<float> hessian_right;
	
	std::vector<float> X_left_col;
	std::vector<float> X_right_col;

	for (int col = 0; col < n_cols; col++) {
		for (int row = 0; row < n_rows; row++) {
			if (X[split_col][row] < split_val) {
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
	std::vector<std::vector<int>> orig_col_idxs_left  = get_sorted_idxs(X_left, n_bins);
	std::vector<std::vector<int>> orig_col_idxs_right = get_sorted_idxs(X_right, n_bins);

	left_child = new Node(
			X_left, 
			orig_col_idxs_left,
			gradient_left, 
			hessian_left, 
			tree_num,
			l2_reg,
			min_child_weight,
			min_data_in_leaf,
			max_depth,
			depth + 1,
			n_bins
			);
	right_child = new Node(
			X_right, 
			orig_col_idxs_right,
			gradient_right, 
			hessian_right, 
			tree_num,
			l2_reg,
			min_child_weight,
			min_data_in_leaf,
			max_depth,
			depth + 1,
			n_bins
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

std::vector<std::vector<int>> Node::get_sorted_idxs(
		std::vector<std::vector<float>>& X_new, 
		int n_bins
		) {
	std::vector<std::vector<int>> new_orig_col_idxs;
	std::vector<float> X_col;
	for (int col = 0; col < int(X.size()); col++) {
		X_col = X_new[col];
		std::vector<int> sorted_col_idxs(X_col.size());
		std::iota(sorted_col_idxs.begin(), sorted_col_idxs.end(), 0);

		std::stable_sort(
				sorted_col_idxs.begin(), 
				sorted_col_idxs.end(), 
				[&X_col](int i, int j) {return X_col[i] < X_col[j];}
				);
		new_orig_col_idxs.push_back(sorted_col_idxs);
	} 
	return new_orig_col_idxs;
}
