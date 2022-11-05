#include <iostream>
#include <vector>
#include <array>
#include <assert.h>

#include "read_data.hpp"
#include "utils.hpp"
#include "gbm.hpp"

int main() {
	// const char* FILENAME = "data/iris_dataset.csv";
	// const char* FILENAME = "data/housing_price_prediction_dataset.csv";
	const char* FILENAME = "data/hpx10.csv";
	// const char* FILENAME = "data/hpx100.csv";

	std::vector<std::vector<float>> X = read_csv_columnar(FILENAME);
	// std::vector<std::vector<float>> X = read_csv_rowmajor(FILENAME);

	std::pair<
			std::pair<std::vector<std::vector<float>>, std::vector<float>>,
			std::pair<std::vector<std::vector<float>>, std::vector<float>>
	> splits = train_test_split_columnar(X, 0.80);
	
	std::vector<std::vector<float>> X_train = splits.first.first;
	std::vector<std::vector<float>> X_test  = splits.second.first;
	std::vector<float> y_train = splits.first.second;
	std::vector<float> y_test  = splits.second.second;


	GBM model(
			8,				// max_depth
			0.00f,			// l2_reg
			0.10f,			// lr
			1.00f,			// min_child_weight (NOT USED IN HIST)
			20,				// min_data_in_leaf
			50,				// num_boosting_rounds
			4				// max_bin
			);
	// model.train_greedy(X_train, y_train);
	model.train_hist(X_train, y_train);

	float* y_preds = model.predict_hist(X_test);

	std::cout << "Test MSE Loss: " << model.calculate_mse_loss(y_preds, y_test) << std::endl;

	free(y_preds);
	return 0;
}
