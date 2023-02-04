#include <iostream>
#include <immintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <chrono>
		


#define DEFAULT true

int main() {
	const int N = 512;
	const int GIGA = 1000000000;
	const int BLOCK_SIZE = 32;
	assert(N % 4 == 0);

	for (int iter = 0; iter < 10; ++iter) {
		alignas(BLOCK_SIZE) float A[N][N];
		alignas(BLOCK_SIZE) float B[N][N];

		// Compiler wants to rip out Matmul if C is not accessed again.
		float C[N][N];

		double GFLOP = (double)N * (double)N * (double)N * 2 / GIGA;


		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				A[i][j] = float(rand() % 10);
				B[i][j] = float(rand() % 10);
			}
		}

		auto time_init = std::chrono::high_resolution_clock::now();



		if (DEFAULT) {
			// Transpose B
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					B[i][j] = B[j][i];
				}
			}

			// Matmul defualt 
			// No Tiling
			/*
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					C[i][j] = 0.00f;
					for (int k = 0; k < N; ++k) {
						C[i][j] += A[i][k] * B[j][k];
					}
				}
			}
			*/

			// Loop Tiling
			for (int ii = 0; ii < N; ii+=BLOCK_SIZE) {
				for (int jj = 0; jj < N; jj+=BLOCK_SIZE) {
					C[ii][jj] = 0.00f;

					for (int i = ii; i < ii + BLOCK_SIZE; ++i) {
						for (int j = jj; j < jj + BLOCK_SIZE; ++j) {
							for (int k = jj; k < jj + BLOCK_SIZE; ++k) {
								C[i][j] += A[i][k] * B[j][k];
							}
						}
					}
				}
			}
		}
		else {
			// Matmul SIMD 
			// No Tiling
			for (int i = 0; i < N; i+=4) {
				__m128 a, b, c;
				for (int j = 0; j < N; ++j) {
					c = _mm_setzero_ps();
					for (int k = 0; k < N; ++k) {
						a = _mm_load_ps(&A[i][k]);
						b = _mm_set1_ps(B[k][j]);
						c = _mm_add_ps(c, _mm_mul_ps(a, b));
					}
				_mm_storeu_ps(&C[i][j], c);
				}
			}
			// Loop Tiling
			/*
			for (int i = 0; i < N; i+=4) {
				__m128 a, b, c;
				for (int j = 0; j < N; ++j) {
					c = _mm_setzero_ps();
					for (int k = 0; k < N; ++k) {
						a = _mm_load_ps(&A[i][k]);
						b = _mm_set1_ps(B[k][j]);
						c = _mm_add_ps(c, _mm_mul_ps(a, b));
					}
				_mm_storeu_ps(&C[i][j], c);
				}
			}
			*/
		}
		auto time_final = std::chrono::high_resolution_clock::now();
		double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(time_final - time_init).count();

		double GFLOPS = GIGA * GFLOP / duration;

		std::cout << "GFLOP:            " << GFLOP << "               IGNORE: " << C[0][0] << std::endl;
		std::cout << "GFLOPS:           " << GFLOPS << std::endl << std::endl;

		//usleep(250000);

	}
	return 0;
}
