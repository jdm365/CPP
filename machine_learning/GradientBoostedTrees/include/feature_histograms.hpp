#pragma once

#include <iostream>
#include <vector>

#include "utils.hpp"


struct __attribute__ ((packed)) Bin {
	float grad_sum;
	float hess_sum;
	int   bin_cnt;

	Bin(float grad=0.00f, float hess=0.00f, int bin_cnt=0): grad_sum(grad),
															hess_sum(hess),
															bin_cnt(bin_cnt){}
};

struct FeatureHistograms {
	std::vector<std::vector<Bin>> bins;

	FeatureHistograms(int n_cols, int max_bin);
	void calc_diff_hist(FeatureHistograms& other_hist);
	void calc_hists(
			const std::vector<std::vector<uint8_t>>& X_hist,
			const std::vector<int>& subsample_cols,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			std::vector<int>& row_idxs,
			bool root=false,
			bool const_hessian=false 
			); 
	void calc_hists_single_feature(
			const std::vector<uint8_t>& X_hist_col,
			std::vector<float>& ordered_gradients,
			std::vector<float>& ordered_hessians,
			std::vector<int>& row_idxs,
			int col
			); 
	void calc_hists_grad_single_feature(
			const std::vector<uint8_t>& X_hist_col,
			std::vector<float>& ordered_gradients,
			std::vector<int>& row_idxs,
			int col
			); 
	std::pair<float, float> get_col_sums(int max_bin);
};


/*
struct GPUFeatureHistograms {
	thrust::device_vector<float> grad_bins;
	thrust::device_vector<float> hess_bins;
	thrust::device_vector<int> 	 bin_cnts;

	GPUFeatureHistograms(int n_cols, int max_bin);
	void calc_diff_hist(GPUFeatureHistograms& other_hist);
	void calc_hists(
			const thrust::device_vector<uint8_t>& X_hist,
			const thrust::device_vector<int>& subsample_cols,
			thrust::device_vector<float>& gradient,
			thrust::device_vector<float>& hessian,
			thrust::device_vector<int>& row_idxs
			); 
	thrust::pair<float, float> get_col_sums(int max_bin);
};
*/
