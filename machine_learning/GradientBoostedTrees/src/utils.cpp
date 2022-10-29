#include <iostream>
#include <vector>
#include <array>
#include <stdlib.h>
#include <assert.h>

#include "utils.hpp"


std::array<std::vector<std::vector<float>>, 2> train_test_split_columnar(
		std::vector<std::vector<float>>& X,
		float train_size
		) {
	// Create index array to draw idxs from.
	std::vector<int> train_idxs;
	std::vector<int> test_idxs;
	for (int idx = 0; idx < int(X[0].size()); idx++) {
		train_idxs.push_back(idx);
	}

	int train_length = int(int(X[0].size()) * train_size);
	int test_length  = int(int(X[0].size()) - train_length);

	// Init random seed.
	srand(time(NULL));

	int idx;
	while (int(test_idxs.size()) != test_length) {
		// Generate random number in range [0, X.size()].
		idx = rand() % int(train_idxs.size());
		test_idxs.push_back(idx);
		train_idxs.erase(train_idxs.begin() + idx);
	}

	std::vector<std::vector<float>> X_train;
	std::vector<std::vector<float>> X_test;

	std::vector<float> X_train_col;
	std::vector<float> X_test_col;

	for (int col = 0; col < int(X.size()); col++) {
		for (int row = 0; row < train_length; row++) {
			if (row < test_length) {
				X_test_col.push_back(X[col][test_idxs[row]]);
			}
			X_train_col.push_back(X[col][train_idxs[row]]);
		}
		X_test.push_back(X_test_col);
		X_train.push_back(X_train_col);

		// Delete all elements of vectors.
		X_test_col.clear();
		X_train_col.clear();
	}

	std::array<std::vector<std::vector<float>>, 2> split = {X_train, X_test};
	return split;
}

std::array<std::vector<std::vector<float>>, 2> train_test_split_rowmajor(
		std::vector<std::vector<float>>& X,
		float train_size
		) {
	// Create index array to draw idxs from.
	std::vector<int> train_idxs;
	std::vector<int> test_idxs;
	for (int idx = 0; idx < int(X.size()); idx++) {
		train_idxs.push_back(idx);
	}

	int train_length = int(X.size() * train_size);
	int test_length  = int(X.size() - train_length);

	// Init random seed.
	srand(time(NULL));

	int idx;
	while (int(test_idxs.size()) != test_length) {
		// Generate random number in range [0, X.size()].
		idx = rand() % int(train_idxs.size());
		test_idxs.push_back(idx);
		train_idxs.erase(train_idxs.begin() + idx);
	}

	std::vector<std::vector<float>> X_train;
	std::vector<std::vector<float>> X_test;

	for (int row = 0; row < train_length; row++) {
		if (row < test_length) {
			X_test.push_back(X[test_idxs[row]]);
		}
		X_train.push_back(X[train_idxs[row]]);
	}

	std::array<std::vector<std::vector<float>>, 2> split = {X_train, X_test};
	return split;
}


std::vector<float> get_ordered_gradients(
		std::vector<uint8_t>& vec_0,
		std::vector<float> vec_1, 
		int max_val
		) {
	uint8_t output_0[vec_0.size()];
	float   output_1[vec_0.size()];

	int count[max_val + 1]; 

	for (int idx = 0; idx < max_val + 1; ++idx) {
		count[idx] = 0;
	}

	for (int idx = 1; idx < int(vec_0.size()); ++idx) {
		++count[int(vec_0[idx])];
	}

	for (int idx = 1; idx < max_val + 1; ++idx) {
		count[idx] += count[idx - 1];
	}

	for (int idx = int(vec_0.size()); idx > 0; --idx) {
		assert(int(vec_0[idx]) <= max_val + 1);
		output_0[count[int(vec_0[idx])]] = vec_0[idx];
		output_1[count[int(vec_0[idx])]] = vec_1[idx];
		--count[int(vec_0[idx])];
	}
	
	for (int idx = 0; idx < int(vec_0.size()); ++idx) {
		vec_0[idx] = output_0[idx];
		vec_1[idx] = output_1[idx];
	}
	return vec_1;
}
