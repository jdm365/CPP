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


int partition(std::vector<int>& values, int left, int right) {
	int pivotIndex = left + (right - left) / 2;
	int pivotValue = values[pivotIndex];
	int i = left, j = right;
	int temp;
	while(i <= j) {
		while(values[i] < pivotValue) {
			i++;
		}
		while(values[j] > pivotValue) {
			j--;
		}
		if(i <= j) {
			temp = values[i];
			values[i] = values[j];
			values[j] = temp;
			i++;
			j--;
		}
	}
	return i;
}

void quicksort(std::vector<float> values, int left, int right) {
	if (left < right) {
        int pivotIndex = partition(&values, left, right);
        quicksort(values, left, pivotIndex - 1);
        quicksort(values, pivotIndex, right);
    }
}


std::vector<float> get_quantiles(std::vector<float> X_col, int n_bins) {
	std::vector<float> split_vals;
	int split_idx;
	int bin_size = int(int(X_col.size()) / n_bins);

	quicksort(X_col, 0, int(X_col.size()));

	for (int idx = 0; idx < n_bins; idx++) {
		split_idx = idx * bin_size;
		split_vals.push_back(X_col[split_idx]);
	}
	return split_vals;
}
