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

	typedef thrust::device_vector<const uint8_t>::iterator ElementIterator;
	typedef thrust::device_vector<int>::iterator   IndexIterator;
	typedef thrust::device_vector<float>::iterator FloatIterator;
	typedef thrust::permutation_iterator<ElementIterator, IndexIterator> PermuteIterator;

	thrust::constant_iterator<int> const_iter(1);

	thrust::device_vector<int> idx_maps(n_rows);

	for (const int& idx: subsample_cols) {
		PermuteIterator row_iter(
				X_hist.begin() + idx * n_rows,
				row_idxs.begin()
				);

		/*
		thrust::permutation_iterator<FloatIterator, PermuteIterator> row_iter_grad(
				gradient.begin(),
				row_iter
				);
		thrust::permutation_iterator<FloatIterator, PermuteIterator> row_iter_hess(
				hessian.begin(),
				row_iter
				);
		*/

		/*
		thrust::sort_by_key(
				row_iter, 
				row_iter + n_rows, 
				gradient.begin()
				);
		thrust::sort_by_key(
				row_iter, 
				row_iter + n_rows, 
				hessian.begin()
				);
		*/

		thrust::reduce_by_key(
				row_iter,
				row_iter + n_rows,
				gradient.begin(),
				grad_bins.begin() + idx * n_rows,
				idx_maps.begin()
				);
		thrust::reduce_by_key(
				row_iter,
				row_iter + n_rows,
				hessian.begin(),
				hess_bins.begin() + idx * n_rows,
				idx_maps.begin()
				);
		thrust::reduce_by_key(
				row_iter,
				row_iter + n_rows,
				const_iter,
				bin_cnts.begin() + idx * n_rows,
				idx_maps.begin()
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
