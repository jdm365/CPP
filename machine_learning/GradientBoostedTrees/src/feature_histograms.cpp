#include <iostream>
#include <vector>
#include <unordered_map>

#include "feature_histograms.hpp"


alignas(8) Bin::Bin(float grad, float hess) {
	gradient = grad;
	hessian  = hess;
}


FeatureHistograms::FeatureHistograms(int n_cols, int max_bin) {
	bins.reserve(max_bin * n_cols);

	for (int col = 0; col < n_cols; ++col) {
		for (int bin = 0; bin < max_bin; ++bin) {
			bins.emplace_back();
		}
	}
}

void FeatureHistograms::calc_diff_hist(FeatureHistograms& other_hist) {
	for (int bin = 0; bin < int(bins.size()); ++bin) {
			bins[bin].gradient -= other_hist.bins[bin].gradient;
			bins[bin].hessian  -= other_hist.bins[bin].hessian;
		}
	}

void FeatureHistograms::calc_hists(
		const std::vector<std::vector<uint8_t>>& X_hist,
		std::vector<float>& gradient,
		std::vector<float>& hessian,
		std::vector<int>& row_idxs,
		int max_bin
		) {
	float ordered_gradients[int(row_idxs.size())];
	float ordered_hessians[int(row_idxs.size())];

	for (int idx = 0; idx < int(row_idxs.size()); ++idx) {
		ordered_gradients[idx] = gradient[row_idxs[idx]];
		ordered_hessians[idx]  = hessian[row_idxs[idx]];
	}

	int rem = int(row_idxs.size()) % 4;

	uint8_t bin_0;
	uint8_t bin_1;
	uint8_t bin_2;
	uint8_t bin_3;

	for (int col = 0; col < int(X_hist.size()); ++col) {
		for (int idx = 0; idx < int(row_idxs.size() - rem); idx+=4) {
			bin_0 = X_hist[col][row_idxs[idx]];
			bin_1 = X_hist[col][row_idxs[idx + 1]];
			bin_2 = X_hist[col][row_idxs[idx + 2]];
			bin_3 = X_hist[col][row_idxs[idx + 3]];

			bins[max_bin * col + bin_0].gradient += ordered_gradients[idx];
			bins[max_bin * col + bin_0].hessian  += ordered_hessians[idx];

			bins[max_bin * col + bin_1].gradient += ordered_gradients[idx + 1];
			bins[max_bin * col + bin_1].hessian  += ordered_hessians[idx  + 1];

			bins[max_bin * col + bin_2].gradient += ordered_gradients[idx + 2];
			bins[max_bin * col + bin_2].hessian  += ordered_hessians[idx  + 2];

			bins[max_bin * col + bin_3].gradient += ordered_gradients[idx + 3];
			bins[max_bin * col + bin_3].hessian  += ordered_hessians[idx  + 3];
		}

		for (int idx = int(row_idxs.size()) - rem; idx < int(row_idxs.size()); ++idx) {
			bin_0 = X_hist[col][row_idxs[idx]];

			bins[max_bin * col + bin_0].gradient += ordered_gradients[idx];
			bins[max_bin * col + bin_0].hessian  += ordered_hessians[idx];
		}
	}
}

void FeatureHistograms::calc_hists_grad(
		const std::vector<std::vector<uint8_t>>& X_hist,
		std::vector<float>& gradient,
		int max_bin
		) {
	for (int col = 0; col < int(X_hist.size()); ++col) {
		for (int row = 0; row < int(X_hist[0].size()); ++row) {
			bins[max_bin * col + X_hist[col][row]].gradient += gradient[row];
		}
	}
}

void FeatureHistograms::calc_hists_hess(
		const std::vector<std::vector<uint8_t>>& X_hist,
		std::vector<float>& hessian,
		int max_bin
		) {
	for (int row = 0; row < int(X_hist[0].size()); ++row) {
		for (int col = 0; col < int(X_hist.size()); ++col) {
			bins[max_bin * col + X_hist[col][row]].hessian += hessian[row];
		}
	}
}

std::pair<float, float> FeatureHistograms::get_col_sums(int max_bin) {
	std::pair<float, float> sums;
	for (int bin = 0; bin < max_bin; ++bin) {
		sums.first  += bins[bin].gradient;
		sums.second += bins[bin].hessian;
	}
	return sums;
}
