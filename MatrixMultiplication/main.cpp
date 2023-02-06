#include <iostream>
#include <immintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <assert.h>
#include <stdio.h>
#include <unistd.h>
#include <chrono>
#include <cblas.h>
#include <omp.h>
		


#define DEFAULT false
#define MULTITHREAD true

int main() {
	const int N = 768;
	const int GIGA = 1000000000;
	const int BLOCK_SIZE = std::min(N / 2, 16);

	assert(N % 4 == 0);

	srand(0);

	for (int iter = 0; iter < 50; ++iter) {
		alignas(32) float A[N][N];
		alignas(32) float B[N][N];

		// Compiler wants to rip out Matmul if C is not accessed again.
		alignas(32) float C[N][N];

		double GFLOP = (double)N * (double)N * (double)(N-1) * 2 / GIGA;


		for (int i = 0; i < N; ++i) {
			for (int j = 0; j < N; ++j) {
				A[i][j] = float(rand() % 10);
				B[i][j] = float(rand() % 10);
			}
		}

		auto time_init = std::chrono::high_resolution_clock::now();

		if (DEFAULT) {
			// Currently tested and working.

			// Transpose B
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					B[i][j] = B[j][i];
				}
			}

			// Matmul defualt 

			// Loop Tiling
			for (int ii = 0; ii < N; ii+=BLOCK_SIZE) {
				for (int jj = 0; jj < N; jj+=BLOCK_SIZE) {

					for (int i = 0; i < N; ++i) {
						for (int j = jj; j < jj + BLOCK_SIZE; ++j) {
							C[i][j] = 0.00f;
							for (int k = ii; k < ii + BLOCK_SIZE; ++k) {
								C[i][j] += A[i][k] * B[j][k];
							}
						}
					}
				}
			}
		}
		else {
			// Transpose B
			for (int i = 0; i < N; ++i) {
				for (int j = 0; j < N; ++j) {
					B[i][j] = B[j][i];
				}
			}

			// Loop Tiling
			if (N < 16) {
				__m128 a, b, c;
				for (int ii = 0; ii < N; ii+=BLOCK_SIZE) {
					for (int jj = 0; jj < N; jj+=BLOCK_SIZE) {

						for (int i = 0; i < N; ++i) {
							for (int j = jj; j < jj + BLOCK_SIZE; j+=4) {
								c = _mm_load_ps(&C[i][j]);
								for (int k = ii; k < ii + BLOCK_SIZE; ++k) {
									a = _mm_load_ps(&A[i][k]);
									b = _mm_set1_ps(B[j][k]);
									c = _mm_fmadd_ps(b, a, c);
								}
								_mm_storeu_ps(&C[i][j], c);
							}
						}
					}
				}
			}
			else { // if ((N >= 16) && (N < 32)) {
				// Loop Tiling
				__m256 a, b, c;

				if (MULTITHREAD) {
					#pragma omp parallel private(a, b, c) shared(A, B, C) num_threads(omp_get_num_procs())
					{
						#pragma omp for schedule(dynamic)
						for (int ii = 0; ii < N; ii+=BLOCK_SIZE) {
							for (int jj = 0; jj < N; jj+=BLOCK_SIZE) {

								for (int i = 0; i < N; ++i) {
									for (int j = jj; j < jj + BLOCK_SIZE; j+=8) {
										c = _mm256_load_ps(&C[i][j]);
										for (int k = ii; k < ii + BLOCK_SIZE; ++k) {
											a = _mm256_load_ps(&A[i][k]);
											b = _mm256_set1_ps(B[k][j]);
											c = _mm256_fmadd_ps(a, b, c);
										}
										_mm256_storeu_ps(&C[i][j], c);
									}
								}
							}
						}
					}
				}
				else {
					for (int ii = 0; ii < N; ii+=BLOCK_SIZE) {
						for (int jj = 0; jj < N; jj+=BLOCK_SIZE) {

							for (int i = 0; i < N; ++i) {
								for (int j = jj; j < jj + BLOCK_SIZE; j+=8) {
									c = _mm256_load_ps(&C[i][j]);
									for (int k = ii; k < ii + BLOCK_SIZE; ++k) {
										a = _mm256_load_ps(&A[i][k]);
										b = _mm256_set1_ps(B[j][k]);
										c = _mm256_fmadd_ps(a, b, c);
									}
									_mm256_storeu_ps(&C[i][j], c);
								}
							}
						}
					}
				}
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

		auto time_final = std::chrono::high_resolution_clock::now();
		double duration = std::chrono::duration_cast<std::chrono::nanoseconds>(time_final - time_init).count();

		// Add dependency to avoid dead code elimination.
		double eta = duration * C[0][0] / GIGA;
		double GFLOPS = GIGA * GFLOP / duration;
		GFLOPS -= (eta / 10000);

		std::cout << "GFLOPS:  " << GFLOPS << std::endl;

		//usleep(250000);

		if (N == 4 || N == 8) {
			std::cout << "[[" << A[0][0] << ", " << A[0][1] << ", " << A[0][2] << ", " << A[0][3] << "]," << std::endl;
			std::cout << "["  << A[1][0] << ", " << A[1][1] << ", " << A[1][2] << ", " << A[1][3] << "]," << std::endl;
			std::cout << "["  << A[2][0] << ", " << A[2][1] << ", " << A[2][2] << ", " << A[2][3] << "]," << std::endl;
			std::cout << "["  << A[3][0] << ", " << A[3][1] << ", " << A[3][2] << ", " << A[3][3] << "]]" << std::endl << std::endl;

			std::cout << "[[" << B[0][0] << ", " << B[0][1] << ", " << B[0][2] << ", " << B[0][3] << "]," << std::endl;
			std::cout << "["  << B[1][0] << ", " << B[1][1] << ", " << B[1][2] << ", " << B[1][3] << "]," << std::endl;
			std::cout << "["  << B[2][0] << ", " << B[2][1] << ", " << B[2][2] << ", " << B[2][3] << "]," << std::endl;
			std::cout << "["  << B[3][0] << ", " << B[3][1] << ", " << B[3][2] << ", " << B[3][3] << "]]" << std::endl << std::endl;

			std::cout << "[[" << C[0][0] << ", " << C[0][1] << ", " << C[0][2] << ", " << C[0][3] << "]," << std::endl;
			std::cout << "["  << C[1][0] << ", " << C[1][1] << ", " << C[1][2] << ", " << C[1][3] << "]," << std::endl;
			std::cout << "["  << C[2][0] << ", " << C[2][1] << ", " << C[2][2] << ", " << C[2][3] << "]," << std::endl;
			std::cout << "["  << C[3][0] << ", " << C[3][1] << ", " << C[3][2] << ", " << C[3][3] << "]]" << std::endl << std::endl;
			return 0;
		}

	}
	return 0;
}
