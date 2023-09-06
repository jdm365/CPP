#include <iostream>
#include <vector>
#include <omp.h>

#include "../include/feature_histograms.hpp"
#include "../include/utils.hpp"


/*
***************************************************
***************** CPU HISTOGRAMS ******************
***************************************************
*/

FeatureHistograms::FeatureHistograms(int n_cols, int max_bin) {
	bins.resize(n_cols, std::vector<Bin>(max_bin));
}

void FeatureHistograms::calc_diff_hist(FeatureHistograms& other_hist) {
	const int num_threads = std::min(omp_get_max_threads(), (int)bins.size());
	#pragma omp parallel num_threads(num_threads)
	{
		#pragma omp for schedule(static)
		for (int col = 0; col < (int)bins.size(); ++col) {
			for (int bin = 0; bin < (int)bins[0].size(); ++bin) {
				bins[col][bin].grad_sum -= other_hist.bins[col][bin].grad_sum;
				bins[col][bin].hess_sum -= other_hist.bins[col][bin].hess_sum;
				bins[col][bin].bin_cnt  -= other_hist.bins[col][bin].bin_cnt;
			}
		}
	}
}

void FeatureHistograms::calc_hists(
		const std::vector<std::vector<uint8_t>>& X_hist,
		const std::vector<int>& subsample_cols,
		const std::vector<float>& gradient,
		const std::vector<float>& hessian,
		const std::vector<int>& row_idxs,
		bool root,
		bool const_hessian
		) {
	const int num_rows = (int)row_idxs.size();
	const int num_cols = (int)subsample_cols.size();

	int num_threads = std::min(omp_get_max_threads(), num_cols);

	if (!const_hessian) {
		if (!root) {
			// Create new gradient and hessian vectors contiguous in memory
			std::vector<float> ordered_gradients;
			std::vector<float> ordered_hessians;
			ordered_gradients.reserve(num_rows);
			ordered_hessians.reserve(num_rows);

			for (const int& row: row_idxs) {
				ordered_gradients.push_back(gradient[row]);
				ordered_hessians.push_back(hessian[row]);
			}

			#pragma omp parallel num_threads(num_threads)
			{
				#pragma omp for schedule(static)
				for (int col_idx = 0; col_idx < num_cols; ++col_idx) {

					calc_hists_single_feature(
							X_hist[subsample_cols[col_idx]],
							ordered_gradients,
							ordered_hessians,
							row_idxs,
							col_idx	
							);
				}
			}

		}
		else {
			#pragma omp parallel num_threads(num_threads)
			{
				#pragma omp for schedule(static)
				for (int idx = 0; idx < num_cols; ++idx) {
					calc_hists_single_feature(
							X_hist[subsample_cols[idx]],
							gradient,
							hessian,
							row_idxs,
							idx	
							);
				}
			}
		}
	}
	else {
		if (!root) {
			std::vector<float> ordered_gradients;
			ordered_gradients.reserve(num_rows);

			for (const int& row: row_idxs) {
				ordered_gradients.push_back(gradient[row]);
			}

			#pragma omp parallel num_threads(num_threads)
			{
				#pragma omp for schedule(static)
				for (int idx = 0; idx < num_cols; ++idx) {
					calc_hists_grad_single_feature(
							X_hist[subsample_cols[idx]],
							ordered_gradients,
							row_idxs,
							idx	
							);
				}
			}
		}
		else {
			#pragma omp parallel num_threads(num_threads)
			{
				#pragma omp for schedule(static)
				for (int idx = 0; idx < num_cols; ++idx) {
					calc_hists_grad_single_feature(
							X_hist[subsample_cols[idx]],
							gradient,
							row_idxs,
							idx	
							);
				}
			}
		}

	}
}

void FeatureHistograms::calc_hists_single_feature(
		const std::vector<uint8_t>& X_hist_col,
		const std::vector<float>& ordered_gradients,
		const std::vector<float>& ordered_hessians,
		const std::vector<int>& row_idxs,
		int bin_idx
		) {
	uint8_t bin_0;
	// uint8_t bin_1;
	// uint8_t bin_2;
	// uint8_t bin_3;

	const int num_rows = (int)row_idxs.size();

	// int final_step = ((int)num_rows / 4) * 4;

	std::vector<Bin>& bin_col = bins[bin_idx];
	/*
	#pragma omp simd
	for (int idx = 0; idx < final_step; idx += 4) {
		bin_0 = X_hist_col[row_idxs[idx]];
		bin_1 = X_hist_col[row_idxs[idx + 1]];
		bin_2 = X_hist_col[row_idxs[idx + 2]];
		bin_3 = X_hist_col[row_idxs[idx + 3]];

		bin_col[bin_0].grad_sum += ordered_gradients[idx];
		bin_col[bin_1].grad_sum += ordered_gradients[idx + 1];
		bin_col[bin_2].grad_sum += ordered_gradients[idx + 2];
		bin_col[bin_3].grad_sum += ordered_gradients[idx + 3];

		bin_col[bin_0].hess_sum += ordered_hessians[idx];
		bin_col[bin_1].hess_sum += ordered_hessians[idx + 1];
		bin_col[bin_2].hess_sum += ordered_hessians[idx + 2];
		bin_col[bin_3].hess_sum += ordered_hessians[idx + 3];

		bin_col[bin_0].bin_cnt++;
		bin_col[bin_1].bin_cnt++;
		bin_col[bin_2].bin_cnt++;
		bin_col[bin_3].bin_cnt++;
	}
	*/

	// for (int idx = final_step; idx < num_rows; ++idx) {
	for (int idx = 0; idx < num_rows; ++idx) {
		bin_0 = X_hist_col[row_idxs[idx]];

		bin_col[bin_0].grad_sum += ordered_gradients[idx];
		bin_col[bin_0].hess_sum += ordered_hessians[idx];
		bin_col[bin_0].bin_cnt++;
	}
}


void FeatureHistograms::calc_hists_grad_single_feature(
		const std::vector<uint8_t>& X_hist_col,
		const std::vector<float>& ordered_gradients,
		const std::vector<int>& row_idxs,
		int bin_idx
		) {
	uint8_t bin_0;
	uint8_t bin_1;
	uint8_t bin_2;
	uint8_t bin_3;

	const int num_rows = (int)row_idxs.size();

	int final_step = ((int)num_rows / 4) * 4;

	std::vector<Bin>& bins_col = bins[bin_idx];
	for (int idx = 0; idx < final_step; idx+=4) {
		bin_0 = X_hist_col[row_idxs[idx]];
		bin_1 = X_hist_col[row_idxs[idx + 1]];
		bin_2 = X_hist_col[row_idxs[idx + 2]];
		bin_3 = X_hist_col[row_idxs[idx + 3]];

		bins_col[bin_0].grad_sum += ordered_gradients[idx];
		bins_col[bin_1].grad_sum += ordered_gradients[idx + 1];
		bins_col[bin_2].grad_sum += ordered_gradients[idx + 2];
		bins_col[bin_3].grad_sum += ordered_gradients[idx + 3];

		bins_col[bin_0].hess_sum += 2.00f;
		bins_col[bin_1].hess_sum += 2.00f;
		bins_col[bin_2].hess_sum += 2.00f;
		bins_col[bin_3].hess_sum += 2.00f;

		bins_col[bin_0].bin_cnt++;
		bins_col[bin_1].bin_cnt++;
		bins_col[bin_2].bin_cnt++;
		bins_col[bin_3].bin_cnt++;
	}

	for (int idx = final_step; idx < num_rows; ++idx) {
		bin_0 = X_hist_col[row_idxs[idx]];

		bins_col[bin_0].grad_sum += ordered_gradients[idx];
		bins_col[bin_0].hess_sum += 2.00f;
		bins_col[bin_0].bin_cnt++;
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
