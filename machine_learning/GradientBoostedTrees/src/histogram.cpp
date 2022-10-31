#include <vector>
#include <iostream>

#include "histogram.hpp"


alignas(8) Bin::Bin(float grad, float hess) {
	gradient = grad;
	hessian  = hess;
}


Histograms::Histograms(int n_cols, int max_bin) {
	alignas(64) std::vector<Bin> bins_col;
	bins_col.reserve(max_bin);

	for (int col = 0; col < n_cols; ++col) {
		bins_col.clear();
		for (int bin = 0; bin < max_bin; ++bin) {
			bins_col.emplace_back();
		}
		bins.push_back(bins_col);
	}
}

void Histograms::calc_diff_hist(Histograms& other_hist) {
	for (int col = 0; col < int(bins.size()); ++col) {
		for (int bin = 0; bin < int(bins[0].size()); ++bin) {
			bins[col][bin].gradient -= other_hist.bins[col][bin].gradient;
			bins[col][bin].hessian  -= other_hist.bins[col][bin].hessian;
		}
	}
}

void Histograms::calc_hists(
		const std::vector<std::vector<uint8_t>>& X_hist,
		std::vector<float>& gradient,
		std::vector<float>& hessian,
		std::vector<int>& row_idxs
		) {
	for (const int& row: row_idxs) {
		for (int col = 0; col < int(X_hist[0].size()); ++col) {
			bins[col][X_hist[row][col]].gradient += gradient[row];
			bins[col][X_hist[row][col]].hessian  += hessian[row];
		}
	}
}

void Histograms::calc_hists_grad(
		const std::vector<std::vector<uint8_t>>& X_hist,
		std::vector<float>& gradient
		) {
	for (int row = 0; row < int(X_hist.size()); ++row) {
		for (int col = 0; col < int(X_hist[0].size()); ++col) {
			bins[col][X_hist[row][col]].gradient += gradient[row];
		}
	}
}

void Histograms::calc_hists_hess(
		const std::vector<std::vector<uint8_t>>& X_hist,
		std::vector<float>& hessian 
		) {
	for (int row = 0; row < int(X_hist.size()); ++row) {
		for (int col = 0; col < int(X_hist[0].size()); ++col) {
			bins[col][X_hist[row][col]].hessian += hessian[row];
		}
	}
}

std::pair<float, float> Histograms::get_col_sums() {
	std::pair<float, float> sums;
	for (int bin = 0; bin < int(bins[0].size()); ++bin) {
		sums.first  += bins[0][bin].gradient;
		sums.second += bins[0][bin].hessian;
	}
	return sums;
}
