#include <iostream>
#include <immintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <omp.h>

#include "funcs.hpp"


void avx2_blocked_matmul(
		const int N, 
		const int BLOCK_SIZE, 
		const float* A, 
		const float* B, 
		float* C
		) {
	__m256 sum;
	for (int ii = 0; ii < N; ii+=BLOCK_SIZE) {
		for (int jj = 0; jj < N; jj+=BLOCK_SIZE) {

			for (int i = 0; i < N; ++i) {
				for (int j = jj; j < jj + BLOCK_SIZE; j+=8) {
					sum = _mm256_load_ps(&C[i * N + j]);
					for (int k = ii; k < ii + BLOCK_SIZE; ++k) {
						// SAXPY
						sum = _mm256_fmadd_ps(_mm256_load_ps(&A[i * N + k]), _mm256_set1_ps(B[j * N + k]), sum);
					}
					_mm256_store_ps(&C[i * N + j], sum);
				}
			}
		}
	}
}

void micro_kernel(
		const int N, 
		const int B_col_start, 
		const int B_col_end,
		const int A_row_start, 
		const int A_row_end, 
		const float* A, 
		float* B, 
		float* C
		) {
	// Need to keep explicit stack variable 
	// to avoid writing to memory in each loop.
	__m256 sums[4];

	for (int k = B_col_start; k < B_col_end; ++k) {
		__m256 b0 = _mm256_load_ps(&B[k * N + B_col_end]);
		__m256 b1 = _mm256_load_ps(&B[k * N + B_col_end + 8]);

		__m256 a0 = _mm256_load_ps(&A[B_col_start * N + B_col_end]);
		sums[0] = _mm256_fmadd_ps(a0, b0, sums[0]);
		sums[1] = _mm256_fmadd_ps(a0, b0, sums[1]);

		__m256 a1 = _mm256_load_ps(&A[(B_col_start + 1) * N + B_col_end]);
		sums[2] = _mm256_fmadd_ps(a0, b0, sums[2]);
		sums[3] = _mm256_fmadd_ps(a0, b0, sums[3]);
	}
	
	_mm256_store_ps(&C[A_row_start * N + B_col_start], sums[0]); // C[i][j]
	_mm256_store_ps(&C[(A_row_start + 1) * N + B_col_start], sums[1]); // C[i+1][j]
	_mm256_store_ps(&C[A_row_start * N + B_col_start + 8], sums[2]); // C[i][j+8]
	_mm256_store_ps(&C[(A_row_start + 1) * N + B_col_start + 8], sums[3]); // C[i+1][j+8]
}

void avx2_blocked_matmul_multithread(
		const int N, 
		const int BLOCK_SIZE, 
		const float* A, 
		const float* B, 
		float* C
		) {
	__m256 sum;
	#pragma omp parallel private(sum) shared(A, B, C) num_threads(16)
	{
		#pragma omp for// schedule(static, 1)
		for (int ii = 0; ii < N; ii+=BLOCK_SIZE) {
			for (int jj = 0; jj < N; jj+=BLOCK_SIZE) {

				for (int i = 0; i < N; ++i) {
					for (int j = jj; j < jj + BLOCK_SIZE; j+=8) {
						sum = _mm256_load_ps(&C[i * N + j]);
						for (int k = ii; k < ii + BLOCK_SIZE; ++k) {
							// SAXPY
							sum = _mm256_fmadd_ps(_mm256_load_ps(&A[i * N + k]), _mm256_set1_ps(B[j * N + k]), sum);
						}
						_mm256_store_ps(&C[i * N + j], sum);
					}
				}
			}
		}
	}
}


void avx_blocked_matmul(
		const int N, 
		const int BLOCK_SIZE, 
		const float* A, 
		const float* B, 
		float* C
		) {
	__m128 sum;
	for (int ii = 0; ii < N; ii+=BLOCK_SIZE) {
		for (int jj = 0; jj < N; jj+=BLOCK_SIZE) {

			for (int i = 0; i < N; ++i) {
				for (int j = jj; j < jj + BLOCK_SIZE; j+=8) {
					sum = _mm_load_ps(&C[i * N + j]);
					for (int k = ii; k < ii + BLOCK_SIZE; ++k) {
						// SAXPY
						sum = _mm_fmadd_ps(_mm_load_ps(&A[i * N + k]), _mm_set1_ps(B[j * N + k]), sum);
					}
					_mm_store_ps(&C[i * N + j], sum);
				}
			}
		}
	}
}


void blocked_matmul(
		const int N, 
		const int BLOCK_SIZE, 
		const float* A, 
		const float* B, 
		float* C
		) {
	for (int ii = 0; ii < N; ii+=BLOCK_SIZE) {
		for (int jj = 0; jj < N; jj+=BLOCK_SIZE) {

			for (int i = 0; i < N; ++i) {
				for (int j = jj; j < jj + BLOCK_SIZE; ++j) {
					C[i * N + j] = 0.00f;
					for (int k = ii; k < ii + BLOCK_SIZE; ++k) {
						C[i * N + j] += A[i * N + k] * B[j * N + k];
					}
				}
			}
		}
	}
}

void transpose(const int N, float* A) {
	for (int i = 0; i < N; ++i) {
		for (int j = 0; j < N; ++j) {
			A[i * N + j] = A[j * N + i];
		}
	}
}
