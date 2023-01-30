#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>

#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

#include "node.hpp"
#include "tree.hpp"

// GBM -> GradientBoostingMachine

namespace np = boost::python::numpy;


struct GBM {
	int   max_depth;
	float l2_reg;
	float lr;
	float min_child_weight;
	float y_mean_train;
	int   min_data_in_leaf;
	int   num_boosting_rounds;
	int   max_bin;
	int   max_leaves;
	bool  dart;
	float col_subsample_rate;
	int verbosity;
	std::vector<Tree> trees;
	std::vector<std::vector<float>> bin_mapping;

	GBM() {}
	GBM(
			int   _max_depth,
			float _l2_reg,
			float _lr,
			float _min_child_weight,
			int   _min_data_in_leaf,
			int   _num_boosting_rounds,
			int   _max_bin,
			int   _max_leaves,
			float _col_subsample_rate = 1.00f,
			bool  _dart = false,
			int   _verbosity = 50
		);
	~GBM() {}

	void train_greedy(
			std::vector<std::vector<float>>& X, 
			std::vector<float>& y
			);
	void train_hist(
			std::vector<std::vector<float>>& X, 
			std::vector<float>& y
			);
	void train_hist(
			std::vector<std::vector<float>>& X, 
			std::vector<float>& y,
			std::vector<std::vector<float>>& X_validation, 
			std::vector<float>& y_validation,
			int early_stopping_steps
			);

	std::vector<float> predict(std::vector<std::vector<float>>& X_rowmajor);
	std::vector<float> predict_hist(std::vector<std::vector<float>>& X);
	std::vector<float> calculate_gradient(std::vector<float>& preds, std::vector<float>& y);
	std::vector<float> calculate_hessian(std::vector<float>& preds, std::vector<float>& y);
	float calculate_mse_loss(std::vector<float>& preds, std::vector<float>& y); 


	void train_hist_wrapper(np::ndarray const& X, np::ndarray const& y);
	void train_hist_wrapper_validation(
			np::ndarray const& X, 
			np::ndarray const& y,
			np::ndarray const& X_validation, 
			np::ndarray const& y_validation,
			int early_stopping_steps
			);
	np::ndarray predict_hist_wrapper(np::ndarray const& X);

	std::vector<float> __predict_hist(const std::vector<std::vector<uint8_t>>& X_hist_rowmajor);
	void predict_hist_iterative(
			const std::vector<std::vector<uint8_t>>& X_hist_rowmajor,
			std::vector<float>& preds
			);
	float compute_validation_loss(
			const std::vector<std::vector<uint8_t>>& X_hist_rowmajor,
			std::vector<float>& preds,
			std::vector<float>& y
			);


	/*
	***************************************************************
	***************************  GPU  *****************************
	***************************************************************
	*/
	void train_hist_gpu(
			const std::vector<std::vector<uint8_t>>& X_hist, 
			const std::vector<std::vector<uint8_t>>& X_hist_rowmajor, 
			std::vector<float>& y
			);
	thrust::device_vector<float> calculate_gradient_gpu(
			thrust::device_vector<float>& preds, 
			const thrust::device_vector<float>& y
			);
	thrust::device_vector<float> calculate_hessian_gpu(
			thrust::device_vector<float>& preds, 
			const thrust::device_vector<float>& y
			);
	float calculate_mse_loss_gpu(
			thrust::device_vector<float>& preds, 
			const thrust::device_vector<float>& y
			);
};
