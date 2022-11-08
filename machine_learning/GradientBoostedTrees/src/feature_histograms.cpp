#include <iostream>
#include <vector>

#include "feature_histograms.hpp"
#include "utils.hpp"


Bin::Bin(float grad, float hess) {
	grad_sum = grad;
	hess_sum = hess;
}

FeatureHistograms::FeatureHistograms(int n_cols, int max_bin) {
	bins.reserve(n_cols);
	alignas(64) std::vector<Bin> bin_col;
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
			bins[col][bin].grad_sum -= other_hist.bins[col][bin].grad_sum;
			bins[col][bin].hess_sum -= other_hist.bins[col][bin].hess_sum;
		}
	}
}

void FeatureHistograms::calc_hists(
		const std::vector<std::vector<uint8_t>>& X_hist,
		std::vector<float>& gradient,
		std::vector<float>& hessian,
		std::vector<int>& row_idxs,
		int max_bin,
		bool root,
		bool const_hessian
		) {

	if (!const_hessian) {
		if (!root) {
			std::vector<float> ordered_gradients;
			std::vector<float> ordered_hessians;
			ordered_gradients.reserve(row_idxs.size());
			ordered_hessians.reserve(row_idxs.size());

			for (const int& row: row_idxs) {
				ordered_gradients.push_back(gradient[row]);
				ordered_hessians.push_back(hessian[row]);
			}

			#pragma omp parallel if (int(row_idxs.size()) > 100) num_threads(NUM_THREADS)
			{
				#pragma omp for schedule(static)
				for (int col = 0; col < int(X_hist.size()); ++col) {
					calc_hists_single_feature(
							X_hist[col],
							ordered_gradients,
							ordered_hessians,
							row_idxs,
							col
							);
				}
			}
		}
		else {
			#pragma omp parallel num_threads(NUM_THREADS)
			{
				#pragma omp for schedule(static)
				for (int col = 0; col < int(X_hist.size()); ++col) {
					calc_hists_single_feature(
							X_hist[col],
							gradient,
							hessian,
							row_idxs,
							col
							);
				}
			}
		}
	}
	else {
		if (!root) {
			std::vector<float> ordered_gradients;
			ordered_gradients.reserve(row_idxs.size());

			for (const int& row: row_idxs) {
				ordered_gradients.push_back(gradient[row]);
			}

			#pragma omp parallel if (int(row_idxs.size()) > 100) num_threads(NUM_THREADS)
			{
				#pragma omp for schedule(static)
				for (int col = 0; col < int(X_hist.size()); ++col) {
					calc_hists_grad_single_feature(
							X_hist[col],
							ordered_gradients,
							row_idxs,
							col
							);
				}
			}
		}
		else {
			#pragma omp parallel num_threads(NUM_THREADS)
			{
				#pragma omp for schedule(static)
				for (int col = 0; col < int(X_hist.size()); ++col) {
					calc_hists_grad_single_feature(
							X_hist[col],
							gradient,
							row_idxs,
							col
							);
				}
			}
		}

	}
}

void FeatureHistograms::calc_hists_single_feature(
		const std::vector<uint8_t>& X_hist_col,
		std::vector<float>& ordered_gradients,
		std::vector<float>& ordered_hessians,
		std::vector<int>& row_idxs,
		int col
		) {
	uint8_t bin_0;
	uint8_t bin_1;
	uint8_t bin_2;
	uint8_t bin_3;

	int final_step = int(row_idxs.size() / 4) * 4;

	for (int idx = 0; idx < final_step; idx+=4) {
		bin_0 = X_hist_col[row_idxs[idx]];
		bin_1 = X_hist_col[row_idxs[idx + 1]];
		bin_2 = X_hist_col[row_idxs[idx + 2]];
		bin_3 = X_hist_col[row_idxs[idx + 3]];

		bins[col][bin_0].grad_sum += ordered_gradients[idx];
		bins[col][bin_1].grad_sum += ordered_gradients[idx + 1];
		bins[col][bin_2].grad_sum += ordered_gradients[idx + 2];
		bins[col][bin_3].grad_sum += ordered_gradients[idx + 3];

		bins[col][bin_0].hess_sum += ordered_hessians[idx];
		bins[col][bin_1].hess_sum += ordered_hessians[idx + 1];
		bins[col][bin_2].hess_sum += ordered_hessians[idx + 2];
		bins[col][bin_3].hess_sum += ordered_hessians[idx + 3];
	}

	for (int idx = final_step; idx < int(row_idxs.size()); ++idx) {
		bin_0 = X_hist_col[row_idxs[idx]];

		bins[col][bin_0].grad_sum += ordered_gradients[idx];
		bins[col][bin_0].hess_sum += ordered_hessians[idx];
	}
}

void FeatureHistograms::calc_hists_grad_single_feature(
		const std::vector<uint8_t>& X_hist_col,
		std::vector<float>& ordered_gradients,
		std::vector<int>& row_idxs,
		int col
		) {
	uint8_t bin_0;
	uint8_t bin_1;
	uint8_t bin_2;
	uint8_t bin_3;

	int final_step = int(row_idxs.size() / 4) * 4;

	for (int idx = 0; idx < final_step; idx+=4) {
		bin_0 = X_hist_col[row_idxs[idx]];
		bin_1 = X_hist_col[row_idxs[idx + 1]];
		bin_2 = X_hist_col[row_idxs[idx + 2]];
		bin_3 = X_hist_col[row_idxs[idx + 3]];

		bins[col][bin_0].grad_sum += ordered_gradients[idx];
		bins[col][bin_1].grad_sum += ordered_gradients[idx + 1];
		bins[col][bin_2].grad_sum += ordered_gradients[idx + 2];
		bins[col][bin_3].grad_sum += ordered_gradients[idx + 3];
	}

	for (int idx = final_step; idx < int(row_idxs.size()); ++idx) {
		bin_0 = X_hist_col[row_idxs[idx]];

		bins[col][bin_0].grad_sum += ordered_gradients[idx];
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

			bins[col][bin].hess_sum += ordered_hessians[idx];
		}
	}
}

std::pair<float, float> FeatureHistograms::get_col_sums(int max_bin) {
	std::pair<float, float> sums;
	for (int bin = 0; bin < max_bin; ++bin) {
		sums.first  += bins[0][bin].grad_sum;
		sums.second += bins[0][bin].hess_sum;
	}
	return sums;
}
