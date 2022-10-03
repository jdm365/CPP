#include <iostream>
#include <vector>
#include <array>
#include <assert.h>

#include "read_data.hpp"
#include "utils.hpp"
#include "gbm.hpp"

int main() {
	const char* FILENAME = "data/iris_copy.csv";
	std::vector<std::vector<float>> X = read_csv(FILENAME);

	std::cout << "Number of columns: " << X.size() << std::endl;
	std::cout << "Number of rows:    " << X[0].size() << std::endl;
	std::cout << std::endl;

	std::array<std::vector<std::vector<float>>, 2> split = train_test_split(X, 0.80);

	std::vector<std::vector<float>>& data_train = split[0];
	std::vector<std::vector<float>>& data_test  = split[1];

	std::vector<std::vector<float>> X_train;
	std::vector<std::vector<float>> X_test;
	std::vector<float> y_train;
	std::vector<float> y_test;
	
	std::vector<std::vector<float>> X_train_rowwise;
	std::vector<std::vector<float>> X_test_rowwise;

	std::vector<float> X_train_rowwise_row;
	std::vector<float> X_test_rowwise_row;

	for (int idx = 0; idx < (int(X.size()) - 1); idx++) {
		X_train.push_back(data_train[idx]);
		X_test.push_back(data_test[idx]);
	}
	y_train = data_train[(int(X.size()) - 1)];
	y_test  = data_test[(int(X.size()) - 1)];

	for (int row = 0; row < int(y_train.size()); row++) {
		for (int col = 0; col < int(X_train.size()); col++) {
			X_train_rowwise_row.push_back(X_train[col][row]);
			if (row < int(y_test.size())) {
				X_test_rowwise_row.push_back(X_test[col][row]);
			}
		}
		X_train_rowwise.push_back(X_train_rowwise_row);
		X_test_rowwise.push_back(X_test_rowwise_row);

		// Empty elements of array.
		X_train_rowwise_row.clear();
		X_test_rowwise_row.clear();
	}

	assert (X_train_rowwise[2][1] == X_train[1][2]);
	assert (X_train_rowwise[3][0] == X_train[0][3]);
	assert (X_train_rowwise[1][1] == X_train[1][1]);

	GBM model(
			8,				// max_depth
			1.00f,			// l2_reg
			0.10f,			// lr
			1.00f,			// min_child_weight
			5,				// min_data_in_leaf
			250				// num_boosting_rounds
			);
	model.train(X_train, X_train_rowwise, y_train);
	return 0;
}