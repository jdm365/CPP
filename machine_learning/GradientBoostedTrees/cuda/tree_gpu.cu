#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>
#include <chrono>

#include <thrust/device_vector.h>
#include <thrust/host_vector.h>

#include "../include/node.hpp"
#include "../include/tree.hpp"
#include "../include/gbm.hpp"
#include "../include/utils.hpp"



Tree::Tree(
		const cuda_hist& X_hist,
		thrust::device_vector<float>& gradient,
		thrust::device_vector<float>& hessian,
		int&   max_depth,
		float& l2_reg,
		int&   min_data_in_leaf,
		int&   max_bin,
		int&   max_leaves,
		float& col_subsample_rate,
		int&   tree_num
		) {
	const thrust::device_vector<int> subsample_cols = get_subsample_cols(
			col_subsample_rate, 
			int(sizeof(X_hist) / sizeof(int)),
			tree_num
			);

	// Init depth of root node to 0.
	int depth  = 0;
	num_leaves = 0;
	
	thrust::device_vector<int> row_idxs(int(gradient.size()));
	thrust::sequence(row_idxs.begin(), row_idxs.end());

	struct GPUFeatureHistograms hists(int(subsample_cols.size()), max_bin);
	hists.calc_hists(X_hist, subsample_cols, gradient, hessian, row_idxs);


	// Creating the root node will recursively create nodes and build the tree.
	// (Called from constructor).
	/*
	root = new Node(
			X_hist,
			subsample_cols,
			gradient,
			hessian,
			hists,
			row_idxs,
			l2_reg,
			min_data_in_leaf,
			max_depth,
			depth,
			max_bin,
			max_leaves,
			num_leaves
		);
	*/
}
