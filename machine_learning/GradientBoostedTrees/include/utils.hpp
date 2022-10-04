#pragma once

#include <iostream>
#include <vector>
#include <array>
#include <stdlib.h>

std::array<std::vector<std::vector<float>>, 2> train_test_split(
		std::vector<std::vector<float>>& X,
		float train_size
		);
std::vector<float> sort_values(std::vector<float> X_col);
std::vector<float> get_quantiles(std::vector<float> X_col, int n_bins);
