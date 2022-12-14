#pragma once

#include <iostream>
#include <vector>
#include <cuda.h>
#include <cuda_device_runtime_api.h>
#include <cuda_runtime.h>

#include "../include/gbm.hpp"



void calc_hists_single_feature_wrapper(
		uint8_t** X_hist_gpu,
		Bin**  gpu_hist,
		float* ordered_gradients,
		float* ordered_hessians,
		int*   row_idxs,
		int grid_size,
		int block_size,
		int n_rows,
		int n_cols
		); 


template<typename T> void convert_vector_to_cpu_array(
		std::vector<std::vector<T>>& _vector,
		T** cpu_array,
		int n_rows,
		int n_cols
		) {

	// Copy 2d std::vector to array of col arrays.
	for (int col = 0; col < n_cols; ++col) {
		cpu_array[col] = (T*)malloc(sizeof(T) * n_rows);
		for (int row = 0; row < n_rows; ++row) {
			cpu_array[col][row] = (T)0;
		}
	}
}


template<typename T> void calc_hists(T** X_hist_cpu, int n_cols, int n_rows) {
	float* ordered_gradients = (float*)malloc(sizeof(float) * n_rows);
	for (int row = 0; row < n_rows; ++row) {
		ordered_gradients[row] = 0.00f;
	}

	float* ordered_hessians = (float*)malloc(sizeof(float) * n_rows);
	for (int row = 0; row < n_rows; ++row) {
		ordered_hessians[row] = 0.00f;
	}

	int* row_idxs = (int*)malloc(sizeof(int) * n_rows);
	for (int row = 0; row < n_rows; ++row) {
		row_idxs[row] = 0.00f;
	}

	int block_size = 256;
	int grid_size  = ((n_rows * n_cols + block_size) / block_size);


	Bin** cpu_hist = (Bin**)malloc(sizeof(int) * n_cols); 

	calc_hists_single_feature_wrapper(
			X_hist_cpu, 
			cpu_hist,
			ordered_gradients, 
			ordered_hessians, 
			row_idxs,
			grid_size,
			block_size,
			n_rows,
			n_cols
			);
}
