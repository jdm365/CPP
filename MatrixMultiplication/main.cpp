#include <iostream>
#include <assert.h>
#include <chrono>
#include <cstdio>
#include <cstdlib>

#include "funcs.hpp"
		

// i9-12900k max clock speed -> 5.2GHz
// AVX2 fused multiply add flops / cycle = 8 * 2 = 16
// Theoretical max single-threaded GFLOPS = 5.2 * 16 = 83.2 GFLOPS



#define SIMD true

int main() {
	const int N = 64;
	const int GIGA = 1000000000;
	const int BLOCK_SIZE = std::min(N, 32);
	bool MULTITHREAD = (N <= 64) ? false : true;
	const int N_ITERS = 100;
	float flops_arr[N_ITERS];

	assert(N % 4 == 0);

	srand(0);
	
	for (int iter = 0; iter < N_ITERS; ++iter) {
		float* A = static_cast<float*>(aligned_alloc(32, N * N * sizeof(float)));
		float* B = static_cast<float*>(aligned_alloc(32, N * N * sizeof(float)));
		float* C = static_cast<float*>(aligned_alloc(32, N * N * sizeof(float)));

		double GFLOP = (double)N * (double)N * (double)(N-1) * 2 / GIGA;


		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				A[i * N + j] = float(rand() % 10);
				B[i * N + j] = float(rand() % 10);
			}
		}

		auto time_init = std::chrono::high_resolution_clock::now();

		// Transpose B
		transpose(N, B);

		if (!SIMD) {
			blocked_matmul(N, BLOCK_SIZE, A, B, C);
		}
		else {
			if (N < 16) {
				avx_blocked_matmul(N, BLOCK_SIZE, A, B, C);
			}
			else { // if ((N >= 16) && (N < 32)) {
				if (MULTITHREAD) {
					avx2_blocked_matmul_multithread(N, BLOCK_SIZE, A, B, C);
				}
				else {
					avx2_blocked_matmul(N, BLOCK_SIZE, A, B, C);
				}
			/*
			else {
				// Loop Tiling
				__m512 a, b, c;
				for (int ii = 0; ii < N; ii+=BLOCK_SIZE) {
					for (int jj = 0; jj < N; jj+=BLOCK_SIZE) {

						for (int i = 0; i < N; ++i) {
							for (int j = jj; j < jj + BLOCK_SIZE; j+=16) {
								c = _mm512_load_ps(&C[i][j]);
								for (int k = ii; k < ii + BLOCK_SIZE; ++k) {
									a = _mm512_load_ps(&A[i][k]);
									b = _mm512_set1_ps(B[k][j]);
									c = _mm512_fmadd_ps(a, b, c);
								}
								_mm512_storeu_ps(&C[i][j], c);
							}
						}
					}
				}
			}
			*/
			}
		}

		auto time_final = std::chrono::high_resolution_clock::now();
		double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(time_final - time_init).count();

		// Add dependency to avoid dead code elimination.
		double eta = duration * C[0] / GIGA;
		double GFLOPS = GIGA * GFLOP / duration;
		GFLOPS -= (eta / 10000);

		std::cout << "GFLOPS:  " << GFLOPS << std::endl;
		flops_arr[iter] = GFLOPS;

		//usleep(250000);

		if (N == 4 || N == 8) {
			std::cout << "[[" << A[0 * N + 0] << ", " << A[0 * N + 1] << ", " << A[0 * N + 2] << ", " << A[0 * N + 3] << "]," << std::endl;
			std::cout << "["  << A[1 * N + 0] << ", " << A[1 * N + 1] << ", " << A[1 * N + 2] << ", " << A[1 * N + 3] << "]," << std::endl;
			std::cout << "["  << A[2 * N + 0] << ", " << A[2 * N + 1] << ", " << A[2 * N + 2] << ", " << A[2 * N + 3] << "]," << std::endl;
			std::cout << "["  << A[3 * N + 0] << ", " << A[3 * N + 1] << ", " << A[3 * N + 2] << ", " << A[3 * N + 3] << "]]" << std::endl << std::endl;

			std::cout << "[[" << B[0 * N + 0] << ", " << B[0 * N + 1] << ", " << B[0 * N + 2] << ", " << B[0 * N + 3] << "]," << std::endl;
			std::cout << "["  << B[1 * N + 0] << ", " << B[1 * N + 1] << ", " << B[1 * N + 2] << ", " << B[1 * N + 3] << "]," << std::endl;
			std::cout << "["  << B[2 * N + 0] << ", " << B[2 * N + 1] << ", " << B[2 * N + 2] << ", " << B[2 * N + 3] << "]," << std::endl;
			std::cout << "["  << B[3 * N + 0] << ", " << B[3 * N + 1] << ", " << B[3 * N + 2] << ", " << B[3 * N + 3] << "]]" << std::endl << std::endl;

			std::cout << "[[" << C[0 * N + 0] << ", " << C[0 * N + 1] << ", " << C[0 * N + 2] << ", " << C[0 * N + 3] << "]," << std::endl;
			std::cout << "["  << C[1 * N + 0] << ", " << C[1 * N + 1] << ", " << C[1 * N + 2] << ", " << C[1 * N + 3] << "]," << std::endl;
			std::cout << "["  << C[2 * N + 0] << ", " << C[2 * N + 1] << ", " << C[2 * N + 2] << ", " << C[2 * N + 3] << "]," << std::endl;
			std::cout << "["  << C[3 * N + 0] << ", " << C[3 * N + 1] << ", " << C[3 * N + 2] << ", " << C[3 * N + 3] << "]]" << std::endl << std::endl;
			return 0;
		}
		delete[] A;
		delete[] B;
		delete[] C;
	}

	float flops_mean = 0;
	for (int i = 0; i < N_ITERS; ++i) {
		flops_mean += flops_arr[i];
	}
	flops_mean /= N_ITERS;

	std::cout << std::endl << "MEAN GFLOPS: " << flops_mean << " GFLOPS" << std::endl;
	return 0;
}
