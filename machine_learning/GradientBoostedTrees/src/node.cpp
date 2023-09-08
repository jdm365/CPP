#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_map>
#include <assert.h>
#include <omp.h>

#include "../include/node.hpp"
#include "../include/utils.hpp"
#include "../include/feature_histograms.hpp"


/*
****************************************************************
Greedy Constructor
****************************************************************
*/
Node::Node(
		std::vector<std::vector<float>>& X,
		std::vector<float>& gradient,
		std::vector<float>& hessian,
		float& l2_reg,
		float& min_child_weight,
		int&   min_data_in_leaf,
		int&   max_depth,
		int    depth
		) {
	is_leaf	= (depth >= max_depth);
	float grad_sum = 0.00f;
	float hess_sum = 0.00f;
	for (int row = 0; row < int(X.size()); ++row) {
		grad_sum += gradient[row];
		hess_sum += hessian[row];
	}

	calc_gamma(grad_sum, hess_sum, l2_reg);

	// Recursively finds child nodes to build tree.
	if (!is_leaf) {
		get_greedy_split(
				X,
				gradient,
				hessian,
				grad_sum,
				hess_sum,
				l2_reg,
				min_child_weight,
				min_data_in_leaf,
				max_depth,
				depth
				);
	}
}

/*
****************************************************************
Histogram Constructor
****************************************************************
*/
Node::Node(
		const std::vector<std::vector<uint8_t>>& X_hist,
		const std::vector<int>& subsample_cols,
		std::vector<float>& gradient,
		std::vector<float>& hessian,
		FeatureHistograms& hists,
		std::vector<int>& row_idxs,
		float& l2_reg,
		int&   min_data_in_leaf,
		int&   max_depth,
		int    depth,
		int&   max_bin,
		int&   max_leaves,
		std::vector<std::vector<uint8_t>>& min_max_rem,
		int&   num_leaves
		) {

	is_leaf	= (depth >= max_depth) || ((int)X_hist[0].size() < min_data_in_leaf);
	num_leaves++;

	float grad_sum; 
	float hess_sum; 
	std::tie(grad_sum, hess_sum) = hists.get_col_sums(max_bin);
	
	calc_gamma(grad_sum, hess_sum, l2_reg);

	// Needed for prediction.
	//     1. is_leaf
	//     2. gamma
	//     3. split_col
	//     4. split_bin

	// Recursively finds child nodes to build tree.
	if (!is_leaf) {
		get_hist_split(
				X_hist,
				subsample_cols,
				gradient,
				hessian,
				hists,
				row_idxs,
				grad_sum,
				hess_sum,
				l2_reg,
				min_data_in_leaf,
				min_max_rem,
				max_bin,
				depth,
				max_depth,
				max_leaves,
				num_leaves
				);
	}
}


void Node::calc_gamma(
		float grad_sum,
		float hess_sum,
		float l2_reg
		) {
	float eps = 0.00001f;
	gamma = -grad_sum / (hess_sum + l2_reg + eps);
}


float Node::calc_score(
		float lgs,
		float rgs,
		float lhs,
		float rhs,
		float l2_reg
		) {
	// float expr_0 = lgs * lgs / (lhs + l2_reg);
	// float expr_1 = rgs * rgs / (rhs + l2_reg);
	// float expr_2 = (grad_sum) * (grad_sum) / (hess_sum + l2_reg);

	// float score = 0.50f * (expr_0 + expr_1 - expr_2) - gamma; // Actual score
	

	// This returns the relative score (minus constant factors). Might want
	// absolute if implementing `min_gain_to_split`.
	return (lgs * lgs / (lhs + l2_reg)) + (rgs * rgs / (rhs + l2_reg));
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
	preds.reserve(X_pred.size());

	// X_pred is rowmajor storage.
	for (int row = 0; row < int(X_pred.size()); ++row) {
		preds.emplace_back(predict_obs(X_pred[row]));
	}
	return preds;
}


float Node::predict_obs_hist(const std::vector<uint8_t>& obs) {
	if (is_leaf) {
		return gamma;
	}
	if (obs[split_col] < split_bin) {
		return (*left_child).predict_obs_hist(obs);
	}
	return (*right_child).predict_obs_hist(obs);
}


std::vector<float> Node::predict_hist(const std::vector<std::vector<uint8_t>>& X_hist_pred) {
	int n_rows = (int)X_hist_pred.size();
	std::vector<float> preds(n_rows, 0.0f);

	for (int row = 0; row < n_rows; ++row) {
		preds[row] = predict_obs_hist(X_hist_pred[row]);
	}
	return preds;
}


void Node::get_greedy_split(
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
		) {
	int n_cols = (int)X.size();
	int n_rows = (int)X[0].size();

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

	for (int col = 0; col < n_cols; ++col) {
		X_col = X[col];

		std::vector<int> idxs(n_rows);
		std::iota(idxs.begin(), idxs.end(), 0);
		std::stable_sort(idxs.begin(), idxs.end(), [&X_col](int i, int j){return X_col[i] < X_col[j];});

		for (int row = 0; row < n_rows; ++row) {
			// Reset summary statistics.
			left_sum 		   = 0;
			left_gradient_sum  = 0.00f;
			left_hessian_sum   = 0.00f;

			right_sum 		   = n_rows;
			right_gradient_sum = grad_sum;
			right_hessian_sum  = hess_sum;

			// Look at each potential split point.
			for (int idx = 0; idx < row; ++idx) {
				left_sum++;
				left_gradient_sum += gradient[idxs[idx]];
				left_hessian_sum  += hessian[idxs[idx]];

				right_sum--;
				right_gradient_sum -= gradient[idxs[idx]];
				right_hessian_sum  -= hessian[idxs[idx]];
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
					right_hessian_sum,
					l2_reg
					);
			if (score > best_score) {
				split_val  = X_col[idxs[row]];
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

	std::vector<std::vector<float>> X_left(n_cols, std::vector<float>(0));
	std::vector<std::vector<float>> X_right(n_cols, std::vector<float>(0));

	std::vector<float> gradient_left;
	std::vector<float> gradient_right;
	std::vector<float> hessian_left;
	std::vector<float> hessian_right;

	// Reserve n_rows for each gradient. Okay use of extra memory.
	gradient_left.reserve(n_rows);
	gradient_right.reserve(n_rows);
	hessian_left.reserve(n_rows);
	hessian_right.reserve(n_rows);

	for (int col = 0; col < n_cols; ++col) {
		for (int row = 0; row < n_rows; ++row) {
			if (X[split_col][row] <= split_val) {
				X_left[col].push_back(X[col][row]);
				if (col == 0) {
					gradient_left.push_back(gradient[row]);
					hessian_left.push_back(hessian[row]);
				}
			}
			else {
				X_right[col].push_back(X[col][row]);
				if (col == 0) {
					gradient_right.push_back(gradient[row]);
					hessian_right.push_back(hessian[row]);
				}
			}
		}
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
				const std::vector<std::vector<uint8_t>>& X_hist,
				const std::vector<int>& subsample_cols,
				std::vector<float>& gradient,
				std::vector<float>& hessian,
				FeatureHistograms& hists,
				std::vector<int>& row_idxs,
				float& grad_sum,
				float& hess_sum,
				float& l2_reg,
				int& min_data_in_leaf,
				std::vector<std::vector<uint8_t>>& min_max_rem,
				int& max_bin,
				int  depth,
				int& max_depth,
				int& max_leaves,
				int& num_leaves
		) {
	int n_cols = int(subsample_cols.size());

	int min_bin_col;
	int max_bin_col;

	std::unordered_map<int, std::pair<int, float>> col_splits;

	for (const int& col: subsample_cols) {
		col_splits[col] = {0, 0.00f};
	}

	const int num_threads = std::min(MAX_THREADS, (int)subsample_cols.size());
	#pragma omp parallel num_threads(num_threads)
	{
		#pragma omp for schedule(static)
		for (int idx = 0; idx < n_cols; ++idx) {
			int col = subsample_cols[idx];

			// Get min and max bin in hist col and only iterate over those buckets.
			min_bin_col = (int)min_max_rem[col][0];
			max_bin_col = (int)min_max_rem[col][1];
			
			col_splits[col] = find_hist_split_col(
					hists.bins[idx],
					min_data_in_leaf,
					l2_reg,
					grad_sum,
					hess_sum,
					min_bin_col,
					max_bin_col,
					(int)row_idxs.size()
					);
		}
	}

	float best_score = -INFINITY;
	for (const int& col: subsample_cols) {
		if (col_splits[col].second > best_score) [[unlikely]] {
			split_bin  = col_splits[col].first;
			best_score = col_splits[col].second;
			split_col  = col;
		}
	}

	if (best_score == -INFINITY) {
		// If no split was found then node is leaf.
		is_leaf = true;
		return;
	}

	std::vector<int> left_idxs;
	std::vector<int> right_idxs;

	for (const int& row: row_idxs) {
		if (X_hist[split_col][row] < split_bin) {
			left_idxs.push_back(row);
		}
		else {
			right_idxs.push_back(row);
		}
	}

	assert(left_idxs.size()  != 0);
	assert(right_idxs.size() != 0);

	std::vector<std::vector<uint8_t>> min_max_rem_left  = min_max_rem;
	std::vector<std::vector<uint8_t>> min_max_rem_right = min_max_rem;

	min_max_rem_left[split_col][1]  = uint8_t(split_bin);
	min_max_rem_right[split_col][0] = uint8_t(split_bin);

	depth++;
	if (left_idxs.size() > right_idxs.size()) {
		FeatureHistograms left_hists(n_cols, max_bin);
		left_hists.calc_hists(X_hist, subsample_cols, gradient, hessian, left_idxs);
		hists.calc_diff_hist(left_hists);

		if (num_leaves + depth < max_leaves) {
			right_child = new Node(
					X_hist, 
					subsample_cols,
					gradient, 
					hessian, 
					hists,
					right_idxs,
					l2_reg,
					min_data_in_leaf,
					max_depth,
					depth,
					max_bin,
					max_leaves,
					min_max_rem_right,
					num_leaves
					);
			left_child = new Node(
					X_hist, 
					subsample_cols,
					gradient, 
					hessian, 
					left_hists, 
					left_idxs,
					l2_reg,
					min_data_in_leaf,
					max_depth,
					depth,
					max_bin,
					max_leaves,
					min_max_rem_left,
					num_leaves
					);
		}
		else {
			is_leaf = true;
			return;
		}
	}
	else {
		FeatureHistograms right_hists(n_cols, max_bin);
		right_hists.calc_hists(X_hist, subsample_cols, gradient, hessian, right_idxs);
		hists.calc_diff_hist(right_hists);

		if (num_leaves + depth < max_leaves) {
			left_child = new Node(
					X_hist, 
					subsample_cols,
					gradient, 
					hessian, 
					hists, 
					left_idxs,
					l2_reg,
					min_data_in_leaf,
					max_depth,
					depth,
					max_bin,
					max_leaves,
					min_max_rem_left,
					num_leaves
					);
			right_child = new Node(
					X_hist, 
					subsample_cols,
					gradient, 
					hessian, 
					right_hists,
					right_idxs,
					l2_reg,
					min_data_in_leaf,
					max_depth,
					depth,
					max_bin,
					max_leaves,
					min_max_rem_right,
					num_leaves
					);
		}
		else {
			is_leaf = true;
			return;
		}
	}
}


std::pair<int, float> Node::find_hist_split_col(
		std::vector<Bin>& bins_col,
		int   min_data_in_leaf,
		float l2_reg,
		float grad_sum,
		float hess_sum,
		int   min_bin_col,
		int   max_bin_col,
		int   n_rows
		) {
	if (max_bin_col - min_bin_col == 1) {
		std::pair<int, float> best_split(1, -INFINITY);
		return best_split;
	}

	float left_gradient_sum  = 0.00f;
	float left_hessian_sum   = 0.00f;
	float right_gradient_sum = grad_sum;
	float right_hessian_sum  = hess_sum;

	float best_score = -INFINITY;
	float score;

	int split_bin_  = 0;
	int bin_cnt_sum = 0;

	for (int bin = min_bin_col; bin < max_bin_col; ++bin) {
		left_gradient_sum  += bins_col[bin].grad_sum;
		left_hessian_sum   += bins_col[bin].hess_sum;
		right_gradient_sum -= bins_col[bin].grad_sum;
		right_hessian_sum  -= bins_col[bin].hess_sum;

		bin_cnt_sum += bins_col[bin].bin_cnt;

		if (bin_cnt_sum < min_data_in_leaf) {
			continue;
		}
		if (bin_cnt_sum > n_rows - min_data_in_leaf) {
			break;
		}

		score = calc_score(
				left_gradient_sum,
				right_gradient_sum,
				left_hessian_sum, 
				right_hessian_sum,
				l2_reg
				);

		if (score > best_score) {
			split_bin_  = bin + 1;
			best_score  = score;
		}
	}
	std::pair<int, float> best_split = {split_bin_, best_score};
	return best_split;
}
