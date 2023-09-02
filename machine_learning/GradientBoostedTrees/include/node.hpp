#pragma once

#include <vector>
#include <array>
#include <unordered_map>

// #include <thrust/device_vector.h>
// #include <thrust/tuple.h>

#include "feature_histograms.hpp"

struct Node {
	float 	gamma;
	int     split_col;
	int		split_bin;
	float 	split_val;
	bool  	is_leaf;

	Node* 	left_child;
	Node* 	right_child;

	// Default Constructor
	Node();
	~Node() {};

	// Greedy Constructor
	Node(
			std::vector<std::vector<float>>& X,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			float& l2_reg,
			float& min_child_weight,
			int& min_data_in_leaf,
			int& max_depth,
			int depth
		);

	// Histogram Constructor
	Node(
			const std::vector<std::vector<uint8_t>>& X_hist,
			const std::vector<int>& subsample_cols,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			FeatureHistograms& hists,
			std::vector<int>& row_idxs,
			float& l2_reg,
			int& min_data_in_leaf,
			int& max_depth,
			int  depth,
			int& max_bin,
			int& max_leaves,
			std::vector<std::vector<uint8_t>>& min_max_rem,
			int& num_leaves
		);
	void get_hist_split(
			const std::vector<std::vector<uint8_t>>& X_hist,
			const std::vector<int>& subsample_cols,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			FeatureHistograms& hists,
			std::vector<int>& row_idxs,
			float& grad_sum,
			float& hess_sum,
			float& l2_reg,
			int& min_data_in_leaf,
			std::vector<std::vector<uint8_t>>& min_max_rem,
			int& max_bin,
			int  depth,
			int& max_depth,
			int& max_leaves,
			int& num_leaves
			);
	void get_greedy_split(
			std::vector<std::vector<float>>& X,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			float& grad_sum,
			float& hess_sum,
			float& l2_reg,
			float& min_child_weight,
			int& min_data_in_leaf,
			int& max_depth,
			int depth
			);

	void calc_gamma(
			float grad_sum, 
			float hess_sum,
			float l2_reg
			);

	float calc_score(
			float lgs, // Left gradient sum
			float rgs, // Right gradient sum
			float lhs, // Left hessian sum
			float rhs, // Right hessian sum
			float l2_reg
			);

	std::pair<int, float> find_hist_split_col(
		std::vector<Bin>& bins_col,
		int   min_data_in_leaf,
		float l2_reg,
		float grad_sum,
		float hess_sum,
		int   min_bin_col,
		int   max_bin_col,
		int   n_rows
		);

	float predict_obs(std::vector<float>& obs);
	std::vector<float> predict(std::vector<std::vector<float>>& X_pred);

	float predict_obs_hist(const std::vector<uint8_t>& obs);
	std::vector<float> predict_hist(const std::vector<std::vector<uint8_t>>& X_hist_pred);



	/*
	*****************************************************************
	***************************** GPU *******************************
	*****************************************************************
	*/
	/*
	Node(
			const thrust::device_vector<uint8_t>& X_hist,
			const thrust::device_vector<int>& subsample_cols,
			thrust::device_vector<float>& gradient,
			thrust::device_vector<float>& hessian,
			GPUFeatureHistograms& hists,
			thrust::device_vector<int>& row_idxs,
			float& l2_reg,
			int& min_data_in_leaf,
			int& max_depth,
			int  depth,
			int& max_bin,
			int& max_leaves,
			int& num_leaves
		);
	void get_hist_split(
			const thrust::device_vector<uint8_t>& X_hist,
			const thrust::device_vector<int>& subsample_cols,
			thrust::device_vector<float>& gradient,
			thrust::device_vector<float>& hessian,
			GPUFeatureHistograms& hists,
			thrust::device_vector<int>& row_idxs,
			float& grad_sum,
			float& hess_sum,
			float& l2_reg,
			int& min_data_in_leaf,
			int& max_bin,
			int  depth,
			int& max_depth,
			int& max_leaves,
			int& num_leaves
			);

	struct is_less_than {
		typedef thrust::tuple<uint8_t, int> Tuple;
		__host__ __device__ bool operator()(const Tuple& x) {
			return (int)thrust::get<0>(x) < thrust::get<1>(x);
		}
	};

	struct is_negative {
		__host__ __device__ bool operator()(const int& x) {
			return x < 0;
		}
	};

	struct cast_to_int{
		__host__ __device__ int operator()(const uint8_t& x) {
			return (int)x;
		}
	};
	*/
};
