#include <iostream>
#include <iomanip>
#include <vector>
#include <unordered_map>
#include <map>
#include <chrono>
#include <stdio.h>

#include <thrust/device_vector.h>
#include <thrust/host_vector.h>

#include "../include/node.hpp"
#include "../include/tree.hpp"
#include "../include/gbm.hpp"
#include "../include/utils.hpp"


void GBM::train_hist_gpu(
		const std::vector<std::vector<uint8_t>>& _X_hist, 
		const std::vector<std::vector<uint8_t>>& _X_hist_rowmajor, 
		std::vector<float>& _y
		) {
	int n_rows = int(_y.size());

	thrust::device_vector<float> gradient(n_rows, 0.00f);
	thrust::device_vector<float> hessian(n_rows, 2.00f);

	// Add mean for better start.
	y_mean_train = get_vector_mean(_y);

	thrust::device_vector<float> round_preds(n_rows);
	thrust::device_vector<float> preds(n_rows, y_mean_train);

	const cuda_hist X_hist 			= convert_hist_to_cuda(_X_hist);
	const cuda_hist X_hist_rowmajor = convert_hist_to_cuda(_X_hist_rowmajor);
	const thrust::device_vector<float> y(_y);

	auto start_1 = std::chrono::high_resolution_clock::now();
	for (int round = 0; round < num_boosting_rounds; ++round) {
		trees.emplace_back(
					X_hist,
					gradient,
					hessian,
					max_depth,
					l2_reg,
					min_data_in_leaf,
					max_bin,
					max_leaves,
					col_subsample_rate,
					round
			);


		round_preds = trees[round].predict_hist_gpu(X_hist_rowmajor);
		for (int idx = 0; idx < n_rows; ++idx) {
			preds[idx] += lr * round_preds[idx];
		}

		gradient = calculate_gradient_gpu(preds, y);
		hessian  = calculate_hessian_gpu(preds, y);

		auto stop_1 	= std::chrono::high_resolution_clock::now();
		auto duration_1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop_1 - start_1);

		if (round % verbosity == (verbosity - 1)) {
			printf("Round %i MSE Loss: %2.6f", round + 1, calculate_mse_loss_gpu(preds, y));
			printf("       ");
			printf("Num leaves: %i", (trees[round].num_leaves + 1) / 2);
			printf("       ");
			printf("Time Elapsed: %i ms", int(duration_1.count()));
		}
	}
}
