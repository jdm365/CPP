#include <iostream>
#include <vector>
#include <algorithm>
#include <cuda.h>
#include <cuda_device_runtime_api.h>
#include <cuda_runtime.h>

#include "histogram.hpp"
#include "../include/read_data.hpp"
#include "../include/utils.hpp"
#include "../include/gbm.hpp"
#include "../include/histogram_mapping.hpp"


int main() {
	const char* FILENAME = "data/housing_price_prediction_dataset.csv";

	std::vector<std::vector<float>> X = read_csv_columnar(FILENAME);

	std::pair<
			std::pair<std::vector<std::vector<float>>, std::vector<float>>,
			std::pair<std::vector<std::vector<float>>, std::vector<float>>
	> splits = train_test_split_columnar(X, 0.80);
	
	std::vector<std::vector<float>>& X_train = splits.first.first;
	// std::vector<std::vector<float>>& X_test  = splits.second.first;
	// std::vector<float>& y_train = splits.first.second;
	// std::vector<float>& y_test  = splits.second.second;

	int max_bin = 255;	// Need to init to allow r_value reference in function call.

	GBM model(
			-1,				// max_depth (<= 0 implies no limit)
			0.00f,			// l2_reg
			0.10f,			// lr
			1.00f,			// min_child_weight (NOT USED IN HIST)
			20,				// min_data_in_leaf
			50,				// num_boosting_rounds
			max_bin,		// max_bin
			127,			// max_leaves
			0.80f,			// col_subsample_rate [0.0, 1.0]
			true			// enable_dart
			);


	std::vector<std::vector<float>> bin_mapping;	// Calculated in initial bin mapping.
	std::vector<std::vector<uint8_t>> X_hist = map_hist_bins_train(
			X_train,
			bin_mapping,
			max_bin
			);

	// X_hist is columnar
	int n_cols = int(X_hist.size());
	int n_rows = int(X_hist[0].size());

	// Copy 2d std::vector to array of col arrays.
	uint8_t** X_hist_cpu = (uint8_t**)malloc(sizeof(int) * n_cols);
	convert_vector_to_cpu_array(X_hist, X_hist_cpu, n_rows, n_cols);

	// Send array to histogram.cu file for gpu processing.
	// X_hist_cpu will be freed when converted to gpu array.
	calc_hists(X_hist_cpu, n_cols, n_rows);

	return 0;
}
