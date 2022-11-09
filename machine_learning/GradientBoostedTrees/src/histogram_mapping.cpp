#include <iostream>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <map>
#include <chrono>
#include <assert.h>

#include "histogram_mapping.hpp"

std::vector<std::vector<uint8_t>> map_hist_bins_train(
		std::vector<std::vector<float>>& X,
		std::vector<std::vector<float>>& bin_mapping,
		int& max_bin
		) {
	int n_rows = int(X[0].size());
	int n_cols = int(X.size());

	int bin_size   = std::ceil(n_rows / max_bin);

	std::vector<std::vector<uint8_t>> X_hist(n_cols, std::vector<uint8_t>(n_rows));
	std::vector<float> X_col;

	X_col.reserve(n_rows);
	for (int col = 0; col < n_cols; ++col) {
		bin_mapping.emplace_back();
	}

	#pragma omp parallel private(X_col)
	{
		#pragma omp for schedule(static)
		for (int col = 0; col < n_cols; col++) {
			X_col = X[col];
			float 	last_X = 0.00f;
			int     cntr   = 0;
			uint8_t bin    = 0;

			std::vector<int> idxs(n_rows);
			std::iota(idxs.begin(), idxs.end(), 0);
			std::stable_sort(idxs.begin(), idxs.end(), [&X_col](int i, int j){return X_col[i] < X_col[j];});

			for (int row = 0; row < n_rows; ++row) {
				X_hist[col][idxs[row]] = std::min(bin, uint8_t(max_bin - 1));
				if (cntr >= bin_size && last_X != X_col[idxs[row]]) {
					cntr = 0;
					++bin;
					bin_mapping[col].push_back(last_X);
				}
				else {
					++cntr;
				}
				last_X = X_col[idxs[row]];
			}
		}
	}
	return X_hist;
}


std::vector<std::vector<uint8_t>> map_hist_bins_inference(
		const std::vector<std::vector<float>>& X,
		std::vector<std::vector<float>>& bin_mapping
		) {
	int n_rows = int(X[0].size());
	int n_cols = int(X.size());

	std::vector<std::vector<uint8_t>> X_hist(n_cols, std::vector<uint8_t>(n_rows));
	std::vector<float> X_col;
	X_col.reserve(n_rows);

	float 	  val;
	uint8_t   bin;
	uint8_t   max_bin_col;


	for (int col = 0; col < n_cols; col++) {
		X_col = X[col];
		max_bin_col = uint8_t(bin_mapping[col].size());
		bin = 0;

		std::vector<int> idxs(n_rows);
		std::iota(idxs.begin(), idxs.end(), 0);
		std::stable_sort(idxs.begin(), idxs.end(), [&X_col](int i, int j){return X_col[i] < X_col[j];});

		for (int row = 0; row < n_rows; ++row) {
			val = X_col[idxs[row]];
			X_hist[col][idxs[row]] = std::min(bin, max_bin_col);
			if (val >= bin_mapping[col][bin]) {
				++bin;
			}
		}
	}
	return X_hist;
}

std::vector<std::vector<uint8_t>> get_hist_bins_rowmajor(
		const std::vector<std::vector<uint8_t>>& X_hist
		) {
	std::vector<std::vector<uint8_t>> X_hist_rowmajor;
	std::vector<uint8_t>  X_hist_rowmajor_row;

	for (int row = 0; row < int(X_hist[0].size()); ++row) {
		X_hist_rowmajor_row.clear();
		for (int col = 0; col < int(X_hist.size()); ++col) {
			X_hist_rowmajor_row.push_back(X_hist[col][row]);
		}
		X_hist_rowmajor.push_back(X_hist_rowmajor_row);
	}
	return X_hist_rowmajor;
}

std::vector<std::vector<uint8_t>> get_min_max_rem(
		const std::vector<std::vector<uint8_t>>& X_hist
		) {
	std::vector<std::vector<uint8_t>> min_max_rem;
	for (int col = 0; col < int(X_hist.size()); ++col) {
		min_max_rem.push_back(
				{0, uint8_t(1 + *std::max_element(X_hist[col].begin(), X_hist[col].end()))}
				);
	}
	return min_max_rem;
}

