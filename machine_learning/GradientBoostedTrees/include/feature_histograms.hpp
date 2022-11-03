#pragma once

#include <iostream>
#include <vector>

#define NUM_THREADS	16

struct __attribute__ ((packed)) Bin {
	float gradient;
	float hessian;

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
			int max_bin
			); 
	void calc_hists_single_feature(
			const std::vector<uint8_t>& X_hist_col,
			std::vector<float>& ordered_gradients,
			std::vector<float>& ordered_hessians,
			std::vector<int>& row_idxs,
			int max_bin,
			int col
			); 
	void calc_hists_grad(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& gradient,
			std::vector<int>& row_idxs,
			int max_bin
			); 
	void calc_hists_hess(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& hessian,
			std::vector<int>& row_idxs,
			int max_bin
			); 
	std::pair<float, float> get_col_sums(int max_bin);
};
