#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <stdlib.h>

std::array<std::vector<std::vector<float>>, 2> train_test_split_columnar(
		std::vector<std::vector<float>>& X,
		float train_size
		);
std::array<std::vector<std::vector<float>>, 2> train_test_split_rowmajor(
		std::vector<std::vector<float>>& X,
		float train_size
		);
void vector_reserve_2d(
		std::vector<std::vector<uint8_t>>& vec,
		int dim_0,
		int dim_1
		);
void vector_reserve_2d(
		std::vector<std::vector<int>>& vec,
		int dim_0,
		int dim_1
		);
void vector_reserve_2d(
		std::vector<std::vector<float>>& vec,
		int dim_0,
		int dim_1
		);
