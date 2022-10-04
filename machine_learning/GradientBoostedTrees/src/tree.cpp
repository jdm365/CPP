#include <iostream>
#include <vector>

#include "node.hpp"
#include "tree.hpp"

Tree::Tree(
		std::vector<std::vector<float>>& X_new,
		std::vector<std::vector<float>>& split_vals_new,
		std::vector<float>& gradient_new,
		std::vector<float>& hessian_new,
		int&   tree_num_new,
		int&   max_depth_new,
		float& l2_reg_new,
		float& min_child_weight_new,
		int&   min_data_in_leaf_new
		) {

	X 		   = X_new;
	split_vals = split_vals_new;
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
			split_vals,
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

std::vector<float> Tree::predict(std::vector<std::vector<float>>& X_pred) {
	return (*root).predict(X_pred);
}
