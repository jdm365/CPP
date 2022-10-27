#include <iostream>
#include <vector>
#include <array>
#include <assert.h>

#include "read_data.hpp"
#include "utils.hpp"
#include "gbm.hpp"


int main() {
	// const char* FILENAME = "data/iris_dataset.csv";
	const char* FILENAME = "data/housing_price_prediction_dataset.csv";
	// const char* FILENAME = "data/hpx10.csv";
	// const char* FILENAME = "data/hpx100.csv";

	std::vector<std::vector<float>> X = read_csv_columnar(FILENAME);
	// std::vector<std::vector<float>> X = read_csv_rowmajor(FILENAME);

	std::cout << "Number of columns: " << X.size() - 1 << std::endl;
	std::cout << "Number of rows:    " << X[0].size() << std::endl;
	std::cout << std::endl;

	std::array<std::vector<std::vector<float>>, 2> split = train_test_split_columnar(X, 0.80);

	std::vector<std::vector<float>>& data_train = split[0];
	std::vector<std::vector<float>>& data_test  = split[1];

	std::vector<std::vector<float>> X_train;
	std::vector<std::vector<float>> X_test;
	std::vector<float> y_train;
	std::vector<float> y_test;

	for (int row = 0; row < int(X.size()) - 1; ++row) {
		X_train.push_back(data_train[row]);
		X_test.push_back(data_test[row]);
	}
	y_train = data_train[(int(X.size()) - 1)];
	y_test  = data_test[(int(X.size()) - 1)];

	GBM model(
			8,				// max_depth
			2.00f,			// l2_reg
			0.10f,			// lr
			1.00f,			// min_child_weight (NOT USED IN HIST)
			20,				// min_data_in_leaf
			50,				// num_boosting_rounds
			255				// max_bin
			);
	// model.train_greedy(X_train, y_train);
	// std::vector<float> y_preds = model.predict(X_test);

	model.train_hist(X_train, y_train);
	std::vector<float> y_preds = model.predict_hist(X_test);
	std::cout << "Test MSE Loss: " << model.calculate_mse_loss(y_preds, y_test) << std::endl;
	return 0;
}
