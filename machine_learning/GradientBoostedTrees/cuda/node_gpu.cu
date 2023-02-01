#include <iostream>
#include <vector>
#include <array>
#include <cmath>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <unordered_map>

#include "../include/node.hpp"
#include "../include/utils.hpp"




Node::Node(
		const thrust::device_vector<uint8_t>& X_hist,
		const thrust::device_vector<int>& subsample_cols,
		thrust::device_vector<float>& gradient,
		thrust::device_vector<float>& hessian,
		GPUFeatureHistograms& hists,
		thrust::device_vector<int>& row_idxs,
		float& l2_reg,
		int& min_data_in_leaf,
		int& max_depth,
		int  depth,
		int& max_bin,
		int& max_leaves,
		int& num_leaves
	) {

	is_leaf	= (depth >= max_depth) || (int(row_idxs.size()) < min_data_in_leaf);
	num_leaves++;

	float grad_sum; 
	float hess_sum; 
	thrust::tie(grad_sum, hess_sum) = hists.get_col_sums(max_bin);
	
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
				max_bin,
				depth,
				max_depth,
				max_leaves,
				num_leaves
				);
	}
}


void Node::get_hist_split(
		const thrust::device_vector<uint8_t>& X_hist,
		const thrust::device_vector<int>& subsample_cols,
		thrust::device_vector<float>& gradient,
		thrust::device_vector<float>& hessian,
		GPUFeatureHistograms& hists,
		thrust::device_vector<int>& row_idxs,
		float& grad_sum,
		float& hess_sum,
		float& l2_reg,
		int& min_data_in_leaf,
		int& max_bin,
		int  depth,
		int& max_depth,
		int& max_leaves,
		int& num_leaves
		) {
	int n_cols = int(subsample_cols.size());
	int n_rows = int(row_idxs.size());

	float left_gradient_sum;
	float left_hessian_sum;
	float right_gradient_sum;
	float right_hessian_sum;

	float best_score = -INFINITY;
	float score;

	int bin_cnt_sum = 0;

	for (int col = 0; col < n_cols; ++col) {
		/*
		*************************************************************
		****************** Find Best Col Split **********************
		*************************************************************
		*/

		left_gradient_sum  = 0.00f;
		left_hessian_sum   = 0.00f;
		right_gradient_sum = grad_sum;
		right_hessian_sum  = hess_sum;

		best_score = -INFINITY;

		bin_cnt_sum = 0;

		for (int bin = col * max_bin; bin < (col + 1) * max_bin; ++bin) {
			left_gradient_sum  += hists.grad_bins[bin];
			left_hessian_sum   += hists.hess_bins[bin];
			right_gradient_sum -= hists.grad_bins[bin];
			right_hessian_sum  -= hists.hess_bins[bin];

			bin_cnt_sum += hists.bin_cnts[bin];

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
				split_bin  = bin + 1;
				split_col  = col;
				best_score = score;
			}
		}

		/*
		*************************************************************
		*************************************************************
		*************************************************************
		*/

	}

	if (best_score == -INFINITY) {
		// If no split was found then node is leaf.
		is_leaf = true;
		return;
	}

	thrust::device_vector<int> left_idxs;
	thrust::device_vector<int> right_idxs;

	for (const int& row: row_idxs) {
		if (X_hist[split_col * n_rows + row] < split_bin) {
			left_idxs.push_back(row);
		}
		else {
			right_idxs.push_back(row);
		}
	}

	assert(left_idxs.size()  != 0);
	assert(right_idxs.size() != 0);

	depth++;
	if (left_idxs.size() > right_idxs.size()) {
		GPUFeatureHistograms left_hists(n_cols, max_bin);
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
					num_leaves
					);
		}
		else {
			is_leaf = true;
			return;
		}
	}
	else {
		GPUFeatureHistograms right_hists(n_cols, max_bin);
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
					num_leaves
					);
		}
		else {
			is_leaf = true;
			return;
		}
	}
}
