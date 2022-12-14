#include <stdio.h>
#include <iostream>
#include <cuda.h>
#include <cuda_device_runtime_api.h>
#include <cuda_runtime.h>

#include "histogram.hpp"


template<typename T> void convert_cpu_array_to_gpu_array_2d(
		T** cpu_array,
		T** gpu_array,
		int n_rows,
		int n_cols
		) {

	// Copy 2d std::vector to array of col arrays.
	for (int col = 0; col < n_cols; ++col) {
		cudaMalloc(&gpu_array[col], sizeof(T) * n_rows);
		cudaMemcpy(
				gpu_array[col], 
				cpu_array[col], 
				sizeof(T) * n_rows, 
				cudaMemcpyHostToDevice
				);
	}
	for (int col = 0; col < n_cols; ++col) {
		free(cpu_array[col]);
	}
	free(cpu_array);
}

template<typename T> void convert_cpu_array_to_gpu_array(
		T* cpu_array,
		T* gpu_array,
		int n_rows
		) {
	cudaMalloc(&gpu_array, sizeof(T) * n_rows);
	std::cout << "test" << std::endl;
	
	cudaMemcpy(
			gpu_array, 
			cpu_array, 
			sizeof(T) * n_rows, 
			cudaMemcpyHostToDevice
			);
	free(cpu_array);
}

__global__ void calc_hists_single_feature(
		uint8_t* X_hist_gpu_col,
		Bin*   gpu_hist_col,
		float* ordered_gradients,
		float* ordered_hessians,
		int* row_idxs
		) {
	int thd_idx = blockIdx.x * blockDim.x + threadIdx.x;
	int bin = (int)X_hist_gpu_col[row_idxs[thd_idx]];

	gpu_hist_col[bin].grad_sum += ordered_gradients[thd_idx];
	gpu_hist_col[bin].hess_sum += ordered_hessians[thd_idx];
	gpu_hist_col[bin].bin_cnt++;
}


void calc_hists_single_feature_wrapper(
		uint8_t** X_hist_cpu,
		Bin**   cpu_hist,
		float* ordered_gradients,
		float* ordered_hessians,
		int* row_idxs,
		int grid_size,
		int block_size,
		int n_rows,
		int n_cols
		) {
	float* ordered_gradients_gpu;
	convert_cpu_array_to_gpu_array(
			ordered_gradients,
			ordered_gradients_gpu,
			n_rows
			);


	float* ordered_hessians_gpu;
	convert_cpu_array_to_gpu_array(
			ordered_hessians,
			ordered_hessians_gpu,
			n_rows
			);


	int*   row_idxs_gpu;
	convert_cpu_array_to_gpu_array(
			row_idxs,
			row_idxs_gpu,
			n_rows
			);


	uint8_t** X_hist_gpu;
	convert_cpu_array_to_gpu_array_2d(
			X_hist_cpu, 
			X_hist_gpu, 
			n_rows, 
			n_cols
			);

	Bin** gpu_hist;
	convert_cpu_array_to_gpu_array_2d(
			cpu_hist, 
			gpu_hist, 
			n_rows, 
			n_cols
			);


	for (int col = 0; col < n_cols; ++col) {
		calc_hists_single_feature<<<grid_size, block_size>>>(
				X_hist_gpu[col], 
				gpu_hist[col],
				ordered_gradients, 
				ordered_hessians, 
				row_idxs
				);
	}
	cudaFree(X_hist_gpu);
	cudaFree(gpu_hist);

}

