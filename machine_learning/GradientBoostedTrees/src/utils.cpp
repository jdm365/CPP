#include <iostream>
#include <vector>
#include <array>
#include <stdlib.h>

#include "utils.hpp"


std::array<std::vector<std::vector<float>>, 2> train_test_split(
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
