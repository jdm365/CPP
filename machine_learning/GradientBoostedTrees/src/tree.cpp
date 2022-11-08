#include <iostream>
#include <vector>
#include <numeric>
#include <unordered_map>
#include <assert.h>

#include "node.hpp"
#include "tree.hpp"
#include "utils.hpp"
#include "feature_histograms.hpp"

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
Histogram Constructor
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
		std::vector<std::vector<uint8_t>>& min_max_rem 
		) {
	// init depth of root node to 0.
	int depth = 0;
	
	std::vector<int> row_idxs(X_hist[0].size());
	std::iota(row_idxs.begin(), row_idxs.end(), 0);

	struct FeatureHistograms hists(int(X_hist.size()), max_bin);
	hists.calc_hists(X_hist, gradient, hessian, row_idxs, true);

	// Creating the root node will recursively create nodes and build the tree.
	root = new Node(
			X_hist,
			gradient,
			hessian,
			hists,
			row_idxs,
			l2_reg,
			min_data_in_leaf,
			max_depth,
			depth,
			max_bin,
			min_max_rem
		);
}

std::vector<float> Tree::predict(std::vector<std::vector<float>>& X_pred) {
	return (*root).predict(X_pred);
}

float* Tree::predict_hist(const std::vector<std::vector<uint8_t>>& X_hist_pred) {
	return (*root).predict_hist(X_hist_pred);
}
