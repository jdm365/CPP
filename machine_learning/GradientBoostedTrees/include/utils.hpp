#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <stdlib.h>

std::pair<
		std::pair<std::vector<std::vector<float>>, std::vector<float>>,
		std::pair<std::vector<std::vector<float>>, std::vector<float>>
> train_test_split_columnar(
		std::vector<std::vector<float>>& X,
		float train_size,
		int   seed = 42,
		bool  verbose = true
		);
std::array<std::vector<std::vector<float>>, 2> train_test_split_rowmajor(
		std::vector<std::vector<float>>& X,
		float train_size
		);
std::vector<float> get_ordered_gradients(
		std::vector<uint8_t>& vec_0, 
		std::vector<float> vec_1, 
		int max_val
		);

template <typename T> void vector_reserve_2d(std::vector<std::vector<T>>& vec, int dim_0, int dim_1) {
	vec.reserve(dim_0);
	std::for_each(
			vec.begin(), 
			vec.end(), 
			[dim_1](std::vector<T>& row) {row.reserve(dim_1);}
			);
}

inline float get_vector_mean(std::vector<float>& vec) {
	float mean_val = 0.00f;
	for (const float& val: vec) {
		mean_val += val;
	}
	return mean_val / int(vec.size());
}
