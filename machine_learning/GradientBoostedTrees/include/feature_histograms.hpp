#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

struct Bin {
	float gradient;
	float hessian;

	Bin(float grad=0.00f, float hess=0.00f);
};


struct FeatureHistograms {
	std::vector<Bin> bins;

	FeatureHistograms(int n_cols, int max_bin);
	void calc_diff_hist(FeatureHistograms& other_hist);
	void calc_hists(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			std::vector<int>& row_idxs,
			int max_bin
			); 
	void calc_hists_test(
			std::unordered_map<int, std::vector<int>>& bin_idx_map,
			std::vector<float>& gradient,
			std::vector<float>& hessian,
			std::vector<int>& row_idxs,
			int max_bin
			); 
	void calc_hists_grad(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& gradient,
			int max_bin
			); 
	void calc_hists_hess(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& hessian,
			int max_bin
			); 
	std::pair<float, float> get_col_sums(int max_bin);
};
