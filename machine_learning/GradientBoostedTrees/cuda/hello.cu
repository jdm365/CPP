#include <stdio.h>
#include <cuda.h>
#include <cuda_device_runtime_api.h>
#include <cuda_runtime.h>


#define N 1000000000

__global__ void vector_add(float* out, float* a, float* b, int n) {
	int thd_idx = blockIdx.x * blockDim.x + threadIdx.x;

	if (thd_idx < n) {
		out[thd_idx] = a[thd_idx] + b[thd_idx];
	}
}

int main() {
	float* a   = (float*)malloc(sizeof(float) * N);
	float* b   = (float*)malloc(sizeof(float) * N);
	float* out = (float*)malloc(sizeof(float) * N);

	float* d_a;
	float* d_b;
	float* d_out;

	for (int idx = 0; idx < N; ++idx) {
		a[idx] = 1.00f;
		b[idx] = 2.00f;
	}

	cudaMalloc((void**)& d_a,   sizeof(float) * N);
	cudaMalloc((void**)& d_b,   sizeof(float) * N);
	cudaMalloc((void**)& d_out, sizeof(float) * N);

	cudaMemcpy(d_a, a, sizeof(float) * N, cudaMemcpyHostToDevice);
	cudaMemcpy(d_b, b, sizeof(float) * N, cudaMemcpyHostToDevice);


	int block_size = 256;
	int grid_size  = ((N + block_size) / block_size);

	vector_add<<<grid_size, block_size>>>(out, a, b, N);
	cudaMemcpy(d_out, out, sizeof(float) * N, cudaMemcpyHostToDevice);
	

	free(a);
	free(b);
	free(out);

	cudaFree(d_a);
	cudaFree(d_b);
	cudaFree(d_out);
	return 0;
}
