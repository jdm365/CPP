#include <iostream>
#include <vector>

#include <thrust/device_vector.h>
#include <thrust/host_vector.h>
#include <thrust/iterator/permutation_iterator.h>
#include <thrust/iterator/constant_iterator.h>
#include <thrust/sort.h>
#include <thrust/reduce.h>
#include <thrust/execution_policy.h>

#include "../include/feature_histograms.hpp"
#include "../include/utils.hpp"



GPUFeatureHistograms::GPUFeatureHistograms(int n_cols, int max_bin) {
	grad_bins.resize(n_cols * max_bin);
	hess_bins.resize(n_cols * max_bin);
	bin_cnts.resize(n_cols * max_bin);
}

void GPUFeatureHistograms::calc_diff_hist(GPUFeatureHistograms& other_hist) {
	for (int bin = 0; bin < int(grad_bins.size()); ++bin) {
		grad_bins[bin] -= other_hist.grad_bins[bin];
		hess_bins[bin] -= other_hist.hess_bins[bin];
		bin_cnts[bin]  -= other_hist.bin_cnts[bin];
	}
}

void GPUFeatureHistograms::calc_hists(
		const thrust::device_vector<uint8_t>& X_hist,
		const thrust::device_vector<int>& subsample_cols,
		thrust::device_vector<float>& gradient,
		thrust::device_vector<float>& hessian,
		thrust::device_vector<int>& row_idxs
		) {
	int n_rows = int(row_idxs.size());

	for (const int& idx: subsample_cols) {
		thrust::constant_iterator<int> const_iter(1);

		//thrust::sort_by_key(row_iter, row_iter + (sizeof(row_iter) / sizeof(float)), gradient.begin());

		thrust::device_vector<uint8_t> idx_maps;

		thrust::reduce_by_key(
				thrust::host,
				thrust::make_permutation_iterator(
					X_hist.begin() + idx * n_rows,
					row_idxs.begin()
				),
				thrust::make_permutation_iterator(
					X_hist.begin() + idx * n_rows,
					row_idxs.end()
				),
				grad_bins.begin(),
				idx_maps.begin(),
				grad_bins.begin() + idx * n_rows
				);
		thrust::reduce_by_key(
				thrust::host,
				thrust::make_permutation_iterator(
					X_hist.begin() + idx * n_rows,
					row_idxs.begin()
				),
				thrust::make_permutation_iterator(
					X_hist.begin() + idx * n_rows,
					row_idxs.end()
				),
				hess_bins.begin(),
				idx_maps.begin(),
				hess_bins.begin() + idx * n_rows
				);
		thrust::reduce_by_key(
				thrust::host,
				const_iter,
				const_iter + (sizeof(const_iter) / sizeof(int)),
				const_iter,
				idx_maps.begin(),
				bin_cnts.begin() + idx * n_rows
				);
	}
}



thrust::pair<float, float> GPUFeatureHistograms::get_col_sums(int max_bin) {
	thrust::pair<float, float> sums;
	sums.first = thrust::reduce(
			grad_bins.begin(),
			grad_bins.begin() + max_bin
			);
	sums.second = thrust::reduce(
			hess_bins.begin(),
			hess_bins.begin() + max_bin
			);
	return sums;
}
