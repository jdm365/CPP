#include <iostream>
#include <vector>
#include <numeric>
#include <unordered_map>
#include <random>
#include <assert.h>

#include "../include/node.hpp"
#include "../include/tree.hpp"
#include "../include/utils.hpp"
#include "../include/feature_histograms.hpp"

/*
****************************************************************
Greedy Constructor
****************************************************************
*/
Tree::Tree(
		std::vector<std::vector<float>>& X,
		std::vector<float>& gradient,
		std::vector<float>& hessian,
		int&   max_depth,
		float& l2_reg,
		float& min_child_weight,
		int&   min_data_in_leaf
		) {

	// Creating the root node will recursively create nodes and build the tree.
	root = new Node(
			X,
			gradient,
			hessian,
			l2_reg,
			min_child_weight,
			min_data_in_leaf,
			max_depth,
			0						// init depth = 0 for root node.
		);
}

/*
****************************************************************
Tree Constructor CPU
****************************************************************
*/

Tree::Tree(
		const std::vector<std::vector<uint8_t>>& X_hist,
		std::vector<float>& gradient,
		std::vector<float>& hessian,
		int&   max_depth,
		float& l2_reg,
		int&   min_data_in_leaf,
		int&   max_bin,
		int&   max_leaves,
		float& col_subsample_rate,
		std::vector<std::vector<uint8_t>>& min_max_rem ,
		int &  tree_num
		) {
	const std::vector<int> subsample_cols = get_subsample_cols(
			col_subsample_rate, 
			int(X_hist.size()),
			tree_num
			);

	// Init depth of root node to 0.
	int depth  = 0;
	num_leaves = 0;
	
	std::vector<int> row_idxs(X_hist[0].size());
	std::iota(row_idxs.begin(), row_idxs.end(), 0);

	struct FeatureHistograms hists(int(subsample_cols.size()), max_bin);
	hists.calc_hists(X_hist, subsample_cols, gradient, hessian, row_idxs, true);


	// Creating the root node will recursively create nodes and build the tree.
	// (Called from constructor).
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
			min_max_rem,
			num_leaves
		);
}


std::vector<float> Tree::predict(std::vector<std::vector<float>>& X_pred) {
	return (*root).predict(X_pred);
}

std::vector<float> Tree::predict_hist(const std::vector<std::vector<uint8_t>>& X_hist_pred) {
	return (*root).predict_hist(X_hist_pred);
}

std::vector<int> Tree::get_subsample_cols(float& col_subsample_rate, int n_cols, int tree_num) {
	std::vector<int> col_idxs(n_cols);
	std::iota(col_idxs.begin(), col_idxs.end(), 0);

	std::shuffle(col_idxs.begin(), col_idxs.end(), std::default_random_engine(tree_num));

	std::vector<int> subsample_cols;
	for (int idx = 0; idx < std::max(1, int(n_cols * col_subsample_rate)); ++idx) {
		subsample_cols.push_back(col_idxs[idx]);
	}
	return subsample_cols;
}
