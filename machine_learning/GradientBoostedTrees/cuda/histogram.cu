#include <algorithm>
#include <stdio.h>
#include <cuda.h>
#include <cuda_device_runtime_api.h>
#include <cuda_runtime.h>
#include <vector>


__global__ void vector_add(float* out, float* a, float* b, int n) {
	int thd_idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (thd_idx < n) {
		out[thd_idx] = a[thd_idx] + b[thd_idx];
	}
}

int main() {
	std::vector<std::vector<uint8_t>> X_hist(100, std::vector<uint8_t>(100));

	// Create X_hist on gpu \\
	
	// X_hist is columnar
	int n_cols = int(X_hist.size());
	int n_rows = int(X_hist[0].size());

	uint8_t* X_hist_gpu;

	// Copy 2d std::vector to 1d array.
	uint8_t* X_hist_cpu = (uint8_t*)malloc(n_rows * n_cols);
	for (int col = 0; col < n_cols; ++col) {
		for (int row = 0; row < n_rows; ++row) {
			X_hist_cpu[col * n_rows + row] = X_hist[col][row];
		}
	}
	cudaMalloc((void**)& X_hist_gpu, n_rows * n_cols);
	cudaMemcpy(X_hist_gpu, X_hist_cpu, n_rows * n_cols, cudaMemcpyHostToDevice);
	free(X_hist_cpu);


	int block_size = 256;
	int grid_size  = ((n_rows * n_cols + block_size) / block_size);

	vector_add<<<grid_size, block_size>>>(out, a, b, N);
	cudaMemcpy(d_out, out, sizeof(float) * N, cudaMemcpyHostToDevice);

	cudaFree(X_hist_gpu);
	return 0;
}
