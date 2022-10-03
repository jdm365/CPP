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

	std::vector<float> preds;
	for (int round = 0; round < num_boosting_rounds; round++) {
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

		std::vector<float> round_preds = trees[round].predict(X_rowwise);
		for (int idx = 0; idx < int(round_preds.size()); idx++) {
			if (round == 0) {
				preds.push_back(lr * round_preds[idx]);
			}
			else {
				preds[idx] += lr * round_preds[idx];
			}
		}
		gradient = calculate_gradient(preds, y);
		hessian  = calculate_hessian(preds, y);

		loss = calculate_mse_loss(preds, y);
		std::cout << "Round " << round + 1 << " MSE Loss: " << loss << std::endl;
	}
}

std::vector<float> GBM::predict(std::vector<std::vector<float>>& X_rowwise) {
	std::vector<float> round_preds;
	std::vector<float> tree_preds;

	for (int tree_num = 0; tree_num < int(trees.size()); tree_num++) {
		tree_preds = trees[tree_num].predict(X_rowwise);
		for (int row = 0; row < int(X_rowwise.size()); row++) {
			if (tree_num == 0) {
				round_preds.push_back(lr * tree_preds[row]);
			}
			else {
				round_preds[row] += lr * tree_preds[row];
			}
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
