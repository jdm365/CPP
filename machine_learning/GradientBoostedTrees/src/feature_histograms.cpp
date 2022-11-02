#include <iostream>
#include <vector>
#include <unordered_map>

#include "feature_histograms.hpp"
#include "utils.hpp"


Bin::Bin(float grad, float hess) {
	gradient = grad;
	hessian  = hess;
}

FeatureHistograms::FeatureHistograms(int n_cols, int max_bin) {
	bins.reserve(n_cols);
	std::vector<Bin> bin_col;
	bin_col.reserve(max_bin);

	for (int col = 0; col < n_cols; ++col) {
		for (int bin = 0; bin < max_bin; ++bin) {
			bin_col.emplace_back();
		}
		bins.push_back(bin_col);
		bin_col.clear();
	}
}

void FeatureHistograms::calc_diff_hist(FeatureHistograms& other_hist) {
	for (int col = 0; col < int(bins.size()); ++col) {
		for (int bin = 0; bin < int(bins[0].size()); ++bin) {
			bins[col][bin].gradient -= other_hist.bins[col][bin].gradient;
			bins[col][bin].hessian  -= other_hist.bins[col][bin].hessian;
		}
	}
}

void FeatureHistograms::calc_hists(
		const std::vector<std::vector<uint8_t>>& X_hist,
		std::vector<float>& gradient,
		std::vector<float>& hessian,
		std::vector<int>& row_idxs,
		int max_bin
		) {

	std::vector<float> ordered_gradients;
	std::vector<float> ordered_hessians;
	ordered_gradients.reserve(row_idxs.size());
	ordered_hessians.reserve(row_idxs.size());

	for (int idx = 0; idx < int(row_idxs.size()); ++idx) {
		ordered_gradients.push_back(gradient[row_idxs[idx]]);
		ordered_hessians.push_back(hessian[row_idxs[idx]]);
	}

	#pragma omp parallel num_threads(NUM_THREADS)
	{
		#pragma omp for
		for (int col = 0; col < int(X_hist.size()); ++col) {
			calc_hists_single_feature(
					X_hist[col],
					ordered_gradients,
					ordered_hessians,
					row_idxs,
					max_bin,
					col
					);
		}
	}
}

void FeatureHistograms::calc_hists_single_feature(
		const std::vector<uint8_t>& X_hist_col,
		std::vector<float>& ordered_gradients,
		std::vector<float>& ordered_hessians,
		std::vector<int>& row_idxs,
		int max_bin,
		int col
		) {
	uint8_t bin;
	std::vector<uint8_t> all_bins;
	all_bins.reserve(row_idxs.size());

	for (int idx = 0; idx < int(row_idxs.size()); ++idx) {
		all_bins.push_back(X_hist_col[row_idxs[idx]]);
	}

	for (int idx = 0; idx < int(row_idxs.size()); ++idx) {
		// bin = X_hist_col[row_idxs[idx]];
		bin = all_bins[idx];

		bins[col][bin].gradient += ordered_gradients[idx];
		bins[col][bin].hessian  += ordered_hessians[idx];
	}
}

void FeatureHistograms::calc_hists_grad(
		const std::vector<std::vector<uint8_t>>& X_hist,
		std::vector<float>& gradient,
		std::vector<int>& row_idxs,
		int max_bin
		) {
	int bin;

	std::vector<float> ordered_gradients;
	ordered_gradients.reserve(row_idxs.size());

	for (int idx = 0; idx < int(row_idxs.size()); ++idx) {
		ordered_gradients.push_back(gradient[row_idxs[idx]]);
	}

	for (int col = 0; col < int(X_hist.size()); ++col) {
		for (int idx = 0; idx < int(row_idxs.size()); ++idx) {
			bin = int(X_hist[col][row_idxs[idx]]);

			bins[col][bin].gradient += ordered_gradients[idx];
		}
	}
}

void FeatureHistograms::calc_hists_hess(
		const std::vector<std::vector<uint8_t>>& X_hist,
		std::vector<float>& hessian,
		std::vector<int>& row_idxs,
		int max_bin
		) {
	int bin;

	std::vector<float> ordered_hessians;
	ordered_hessians.reserve(row_idxs.size());

	for (int idx = 0; idx < int(row_idxs.size()); ++idx) {
		ordered_hessians.push_back(hessian[row_idxs[idx]]);
	}

	for (int col = 0; col < int(X_hist.size()); ++col) {
		for (int idx = 0; idx < int(row_idxs.size()); ++idx) {
			bin = int(X_hist[col][row_idxs[idx]]);

			bins[col][bin].hessian += ordered_hessians[idx];
		}
	}
}

std::pair<float, float> FeatureHistograms::get_col_sums(int max_bin) {
	std::pair<float, float> sums;
	for (int bin = 0; bin < max_bin; ++bin) {
		sums.first  += bins[0][bin].gradient;
		sums.second += bins[0][bin].hessian;
	}
	return sums;
}
