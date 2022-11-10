#pragma once

#include <iostream>
#include <vector>

#define NUM_THREADS	4

struct __attribute__ ((packed)) Bin {
	float grad_sum;
	float hess_sum;

	Bin(float grad=0.00f, float hess=0.00f);
};


struct FeatureHistograms {
	std::vector<std::vector<Bin>> bins;

	FeatureHistograms(int n_cols, int max_bin);
	void calc_diff_hist(FeatureHistograms& other_hist);
	void calc_hists(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			std::vector<int>& row_idxs,
			int max_bin,
			bool root = false,
			bool const_hessian = true 
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
	void calc_hists_hess(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& hessian,
			std::vector<int>& row_idxs,
			int max_bin
			); 
	std::pair<float, float> get_col_sums(int max_bin);
};
