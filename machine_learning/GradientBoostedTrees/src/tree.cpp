#include <iostream>
#include <vector>
#include <assert.h>

#include "node.hpp"
#include "tree.hpp"
#include "utils.hpp"

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
Histogram Constructor 0
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

	// Calc init. grad + hess histograms.
	int n_rows = int(X_hist.size());
	int n_cols = int(X_hist[0].size());


	// Most obvious case of slow down at the moment.
	std::vector<std::vector<float>> gradient_hist_root(
			n_cols, 
			std::vector<float>(max_bin, 0.00f)
			);
	std::vector<std::vector<float>> hessian_hist_root(
			n_cols, 
			std::vector<float>(max_bin, 0.00f)
			);
	for (int row = 0; row < n_rows; ++row) {
		for (int col = 0; col < n_cols; ++col) {
			gradient_hist_root[col][X_hist[row][col]] += gradient[row];
			hessian_hist_root[col][X_hist[row][col]]  += hessian[row];
		}
	}
	int depth = 0;

	// Creating the root node will recursively create nodes and build the tree.
	root = new Node(
			X_hist,
			gradient,
			hessian,
			gradient_hist_root,
			hessian_hist_root,
			l2_reg,
			min_data_in_leaf,
			max_depth,
			depth,
			min_max_rem
		);
}

/*
****************************************************************
Histogram Constructor 1
****************************************************************
*/

Tree::Tree(
		const std::vector<std::vector<uint8_t>>& X_hist,
		std::vector<std::vector<float>> hessian_hist_root,
		std::vector<float>& gradient,
		std::vector<float>& hessian,
		int&   max_depth,
		float& l2_reg,
		int&   min_data_in_leaf,
		int&   max_bin,
		std::vector<std::vector<uint8_t>>& min_max_rem 
		) {

	// Calc init. grad histogram.
	int n_rows = int(X_hist.size());
	int n_cols = int(X_hist[0].size());

	// Most obvious case of slow down at the moment.
	std::vector<std::vector<float>> gradient_hist_root(n_cols, std::vector<float>(max_bin));

	for (int row = 0; row < n_rows; ++row) {
		for (int col = 0; col < n_cols; ++col) {
			gradient_hist_root[col][X_hist[row][col]] += gradient[row];
		}
	}
	int depth = 0;

	// Creating the root node will recursively create nodes and build the tree.
	root = new Node(
			X_hist,
			gradient,
			hessian,
			gradient_hist_root,
			hessian_hist_root,
			l2_reg,
			min_data_in_leaf,
			max_depth,
			depth,
			min_max_rem
		);
}


std::vector<float> Tree::predict(std::vector<std::vector<float>>& X_pred) {
	return (*root).predict(X_pred);
}

std::vector<float> Tree::predict_hist(const std::vector<std::vector<uint8_t>>& X_hist_pred) {
	return (*root).predict_hist(X_hist_pred);
}
