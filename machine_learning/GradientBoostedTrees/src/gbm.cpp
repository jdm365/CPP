#include <iostream>
#include <vector>
#include <cmath>

#include "node.hpp"
#include "tree.hpp"
#include "gbm.hpp"


GBM::GBM(
		int   max_depth_new,
		float l2_reg_new,
		float lr_new,
		float min_child_weight_new,
		int   min_data_in_leaf_new,
		int   num_boosting_rounds_new
		) {
	max_depth 			= max_depth_new;
	l2_reg 				= l2_reg_new;
	lr 					= lr_new;
	min_child_weight 	= min_child_weight_new;
	min_data_in_leaf 	= min_data_in_leaf_new;
	num_boosting_rounds = num_boosting_rounds_new;
	trees = {};
}

void GBM::train(
		std::vector<std::vector<float>>& X, 
		std::vector<std::vector<float>>& X_rowwise, 
		std::vector<float>& y
		) {
	std::vector<float> gradient;
	std::vector<float> hessian;

	// Init gradient and hessian to 0.
	for (int idx = 0; idx < int(y.size()); idx++) {
		gradient.push_back(0.00f);
		hessian.push_back(0.00f);
	}
	float loss;

	for (int idx = 0; idx < num_boosting_rounds; idx++) {
		trees.push_back(
				Tree(
					X,
					gradient,
					hessian,
					max_depth,
					l2_reg,
					min_child_weight,
					min_data_in_leaf
				)
			);

		std::vector<float> round_preds = predict(X_rowwise);
		gradient = calculate_gradient(round_preds, y);
		hessian  = calculate_hessian(round_preds, y);

		loss = calculate_mse_loss(round_preds, y);
		std::cout << "Round " << idx + 1 << " MSE Loss: " << loss << std::endl;
	}
}

std::vector<float> GBM::predict(std::vector<std::vector<float>>& X_rowwise) {
	std::vector<float> round_preds;
	std::vector<float> tree_preds;

	for (int tree_num = 0; tree_num < int(trees.size()); tree_num++) {
		if (tree_num == 0) {
			round_preds = trees[tree_num].predict(X_rowwise);
			continue;
		}
		tree_preds = trees[tree_num].predict(X_rowwise);
		for (int row = 0; row < int(round_preds.size()); row++) {
			round_preds[row] += lr * tree_preds[row];
		}
	}
	return round_preds;
}

std::vector<float> GBM::calculate_gradient(std::vector<float>& preds, std::vector<float>& y) {
	std::vector<float> gradient;
	// Assume MSE for now
	for (int idx = 0; idx < int(y.size()); idx++) {
		gradient.push_back(2 * (preds[idx] - y[idx]));
	}
	return gradient;
}

std::vector<float> GBM::calculate_hessian(std::vector<float>& preds, std::vector<float>& y) {
	std::vector<float> hessian;
	// Assume MSE for now
	for (int idx = 0; idx < int(y.size()); idx++) {
		hessian.push_back(2.00f);
	}
	return hessian;
}

float GBM::calculate_mse_loss(std::vector<float>& preds, std::vector<float>& y) {
	float loss = 0.00f;
	// Assume MSE for now
	for (int idx = 0; idx < int(preds.size()); idx++) {
		loss += 0.50f * std::pow((preds[idx] - y[idx]), 2);
	}
	loss = loss / float(preds.size());	
	return loss;
}
