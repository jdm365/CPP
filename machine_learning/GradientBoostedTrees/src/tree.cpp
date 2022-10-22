#include <iostream>
#include <vector>
#include <unordered_map>
#include <assert.h>

#include "node.hpp"
#include "tree.hpp"

/*
****************************************************************
Greedy Constructor
****************************************************************
*/
Tree::Tree(
		std::vector<std::vector<float>>& X_new,
		std::vector<float>& gradient_new,
		std::vector<float>& hessian_new,
		int&   tree_num_new,
		int&   max_depth_new,
		float& l2_reg_new,
		float& min_child_weight_new,
		int&   min_data_in_leaf_new
		) {

	X 		   = X_new;
	gradient   = gradient_new;
	hessian    = hessian_new;
	tree_num   = tree_num_new;
	max_depth  = max_depth_new;
	l2_reg	   = l2_reg_new;
	min_child_weight = min_child_weight_new;
	min_data_in_leaf = min_data_in_leaf_new;

	// Creating the root node will recursively create nodes and build the tree.
	root = new Node(
			X,
			gradient,
			hessian,
			tree_num,
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
		std::vector<std::vector<int>>& X_hist_in,
		std::vector<float>& gradient_in,
		std::vector<float>& hessian_in,
		int&   tree_num_in,
		int&   max_depth_in,
		float& l2_reg_in,
		float& min_child_weight_in,
		int&   min_data_in_leaf_in,
		int&   max_bin_in,
		std::vector<std::vector<int>>& min_max_rem_in 
		) {

	X_hist			  = X_hist_in;
	gradient		  = gradient_in;	
	hessian			  = hessian_in;
	tree_num          = tree_num_in;
	max_depth		  = max_depth_in;	
	l2_reg		  	  = l2_reg_in;	
	min_child_weight  = min_child_weight_in;	
	min_data_in_leaf  = min_data_in_leaf_in;	
	max_bin			  = max_bin_in;

	std::vector<std::vector<float>> gradient_hist = calc_bin_statistics(gradient);
	std::vector<std::vector<float>> hessian_hist  = calc_bin_statistics(hessian);

	int depth = 0;

	// Creating the root node will recursively create nodes and build the tree.
	root = new Node(
			X_hist,
			gradient,
			hessian,
			gradient_hist,
			hessian_hist,
			tree_num,
			l2_reg,
			min_child_weight,
			min_data_in_leaf,
			max_depth,
			depth,
			min_max_rem_in
		);
}


std::vector<float> Tree::predict(std::vector<std::vector<float>>& X_pred) {
	return (*root).predict(X_pred);
}

std::vector<float> Tree::predict_hist(std::vector<std::vector<int>>& X_hist_pred) {
	return (*root).predict_hist(X_hist_pred);
}

std::vector<std::vector<float>> Tree::calc_bin_statistics(std::vector<float>& stat_vector) {
	int n_rows = int(X_hist.size());
	int n_cols = int(X_hist[0].size());

	std::vector<std::vector<float>> stat_hist(n_cols, std::vector<float>(max_bin, 0.00f));

	for (int row = 0; row < n_rows; ++row) {
		for (int col = 0; col < n_cols; ++col) {
			stat_hist[col][X_hist[row][col]] += stat_vector[row];
		}
	}
	return stat_hist;
}
