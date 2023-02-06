#pragma once

#include <immintrin.h>
#include <emmintrin.h>
#include <xmmintrin.h>
#include <omp.h>

void avx2_blocked_matmul(
		const int N, 
		const int BLOCK_SIZE, 
		const float* A, 
		const float* B, 
		float* C
		);


void avx2_blocked_matmul_multithread(
		const int N, 
		const int BLOCK_SIZE, 
		const float* A, 
		const float* B, 
		float* C
		);

void avx_blocked_matmul(
		const int N, 
		const int BLOCK_SIZE, 
		const float* A, 
		const float* B, 
		float* C
		);

void blocked_matmul(
		const int N, 
		const int BLOCK_SIZE, 
		const float* A, 
		const float* B, 
		float* C
		);

void transpose(const int N, float* A);
