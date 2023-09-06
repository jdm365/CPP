#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>
#include <map>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "node.hpp"
#include "tree.hpp"

// GBM -> GradientBoostingMachine


struct GBM {
	int   max_depth;
	float l2_reg;
	float lr;
	float min_child_weight;
	int   min_data_in_leaf;
	int   num_boosting_rounds;
	int   max_bin;
	int   max_leaves;
	float col_subsample_rate;
	bool  dart;
	int   verbosity;
	float y_mean_train;
	std::vector<Tree> trees;
	std::vector<std::vector<float>> bin_mapping;

	// GBM() {}
	GBM(
			int   max_depth,
			float l2_reg,
			float lr,
			float min_child_weight,
			int   min_data_in_leaf,
			int   num_boosting_rounds,
			int   max_bin,
			int   max_leaves,
			float col_subsample_rate = 1.00f,
			bool  dart = false,
			int   verbosity = 50
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


	void train_hist_wrapper(
			const pybind11::array_t<float>& X, 
			const pybind11::array_t<float>& y
			);
	void train_hist_wrapper_validation(
			const pybind11::array_t<float>& X, 
			const pybind11::array_t<float>& y,
			const pybind11::array_t<float>& X_validation, 
			const pybind11::array_t<float>& y_validation,
			int early_stopping_steps
			);
	// np::ndarray predict_hist_wrapper(np::ndarray const& X);
	pybind11::array_t<float> predict_hist_wrapper(const pybind11::array_t<float>& X);

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

	/*
	void train_hist_gpu(
			const std::vector<std::vector<uint8_t>>& X_hist, 
			const std::vector<std::vector<uint8_t>>& X_hist_rowmajor, 
			std::vector<float>& y
			);
	void train_hist_gpu_wrapper(
			np::ndarray const& X, 
			np::ndarray const& y
			);
	*/

	/*
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
	*/
};
