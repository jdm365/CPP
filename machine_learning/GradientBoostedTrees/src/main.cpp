#include <iostream>
#include <vector>
#include <array>
#include <assert.h>

#include "read_data.hpp"
#include "utils.hpp"
#include "gbm.hpp"


int main() {
	// const char* FILENAME = "data/iris_dataset.csv";
	// const char* FILENAME = "data/regression_dataset.csv";
	// const char* FILENAME = "data/housing_price_prediction_dataset.csv";
	const char* FILENAME = "data/hpx10.csv";

	std::vector<std::vector<float>> X = read_csv(FILENAME);

	std::cout << "Number of columns: " << int(X.size() - 1) << std::endl;
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
			5,				// max_depth
			2.00f,			// l2_reg
			0.50f,			// lr
			1.00f,			// min_child_weight (NOT USED IN HIST)
			20,				// min_data_in_leaf
			25,				// num_boosting_rounds
			256				// max_bins
			);
	// model.train_greedy(X_train, X_train_rowwise, y_train);
	model.train_hist(X_train, X_train_rowwise, y_train);

	std::vector<float> y_preds = model.predict_hist(X_test);
	std::cout << "Test MSE Loss: " << model.calculate_mse_loss(y_preds, y_test) << std::endl;

	return 0;
}
