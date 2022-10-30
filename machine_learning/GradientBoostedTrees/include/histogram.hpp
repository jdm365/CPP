#pragma once

#include <vector>
#include <stdint.h>

struct Bin {
	float gradient;
	float hessian;

	Bin(float grad=0.00f, float hess=0.00f);
};


struct Histograms {
	std::vector<std::vector<Bin>> bins;

	Histograms(int n_cols, int max_bin);
	void calc_diff_hist(Histograms& other_hist);
	void calc_hists(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& gradient,
			std::vector<float>& hessian
			); 
	void calc_hists_grad(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& gradient
			); 
	void calc_hists_hess(
			const std::vector<std::vector<uint8_t>>& X_hist,
			std::vector<float>& hessian 
			); 
	std::pair<float, float> get_col_sums();
};
