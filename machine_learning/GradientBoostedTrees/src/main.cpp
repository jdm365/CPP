#include <iostream>
#include <vector>
#include <omp.h>
#include <random>
#include <cstdlib>

#include "../include/read_data.hpp"
#include "../include/utils.hpp"
#include "../include/gbm.hpp"

int main() {
	// omp_set_num_threads(1);

	// const char* FILENAME = "data/iris_dataset.csv";
	const char* FILENAME = "data/housing_price_prediction_dataset.csv";
	// const char* FILENAME = "data/hpx10.csv";
	// const char* FILENAME = "data/hpx100.csv";

	// std::vector<std::vector<float>> X = read_csv_columnar(FILENAME);
	// std::vector<std::vector<float>> X = read_csv_rowmajor(FILENAME);
	
	// Instead generate random data with 100 columns and 1,000,000 rows
	const int NUM_ROWS = 100000;
	const int NUM_COLS = 100;
	std::vector<std::vector<float>> X;

	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dis(0.0, 1.0);

	for (int i = 0; i <= NUM_COLS; i++) {
		std::vector<float> column;
		for (int j = 0; j < (int)NUM_ROWS * 1.25; j++) {
			column.push_back(dis(gen));
		}
		X.push_back(column);
	}

	std::pair<
			std::pair<std::vector<std::vector<float>>, std::vector<float>>,
			std::pair<std::vector<std::vector<float>>, std::vector<float>>
	> splits = train_test_split_columnar(X, 0.80);
	
	std::vector<std::vector<float>>& X_train = splits.first.first;
	std::vector<std::vector<float>>& X_test  = splits.second.first;
	std::vector<float>& y_train = splits.first.second;
	std::vector<float>& y_test  = splits.second.second;

	const int   max_depth 			= 5;
	const float l2_reg 				= 0.00f;
	const float lr 					= 0.10f;
	const float min_child_weight 	= 1.00f;
	const int   min_data_in_leaf 	= 20;
	const int   num_boosting_rounds = 5000;
	const int   max_bin 			= 255;
	const int   max_leaves 			= 31;
	const float col_subsample_rate  = 0.20f;
	const bool  enable_dart 		= false;
	const int   verbosity 			= 50;

	GBM model(
			max_depth,
			l2_reg,
			lr,
			min_child_weight,
			min_data_in_leaf,
			num_boosting_rounds,
			max_bin,
			max_leaves,
			col_subsample_rate,
			enable_dart,
			verbosity
			);

	// model.train_greedy(X_train, y_train);
	model.train_hist(X_train, y_train);

	std::vector<float> y_preds = model.predict_hist(X_test);

	std::cout << "Test MSE Loss: " << model.calculate_mse_loss(y_preds, y_test) << std::endl;

	return 0;
}
