#include <iostream>
#include <iomanip>
#include <vector>
#include <algorithm>
#include <numeric>
#include <cmath>
#include <chrono>
#include <random>
#include <assert.h>
#include <omp.h>

#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

#include "../include/node.hpp"
#include "../include/tree.hpp"
#include "../include/gbm.hpp"
#include "../include/histogram_mapping.hpp"
#include "../include/utils.hpp"
#include "../include/loss_functions.hpp"


#define INF 1048576;

GBM::GBM(
		int   _max_depth,
		float l2_reg,
		float lr,
		float min_child_weight,
		int   min_data_in_leaf,
		int   num_boosting_rounds,
		int   max_bin,
		int   _max_leaves,
		float col_subsample_rate,
		bool  dart,
		int   verbosity
		) : l2_reg(l2_reg),
			lr(lr),
			min_child_weight(min_child_weight),
			min_data_in_leaf(min_data_in_leaf),
			num_boosting_rounds(num_boosting_rounds),
			max_bin(max_bin),
			col_subsample_rate(col_subsample_rate),
			dart(dart),
			verbosity(verbosity) {

	if (_max_depth <= 0) {
		max_depth = INF;
	}
	else {
		max_depth = _max_depth;
	}
	max_leaves = 1 + (_max_leaves << 1);
	trees.reserve(num_boosting_rounds);
}


void GBM::train_greedy(
		std::vector<std::vector<float>>& X, 
		std::vector<float>& y
		) {
	int n_rows = int(X[0].size());
	int n_cols = int(X.size());

	std::vector<float> gradient(n_rows, 0.00f);
	std::vector<float> hessian(n_rows, 2.00f);

	float loss;

	for (int row = 0; row < n_rows; ++row) {
		y_mean_train += y[row];
	}
	y_mean_train /= y.size();

	std::vector<float> round_preds;
	std::vector<float> preds(n_rows, y_mean_train);

	std::vector<std::vector<float>> X_rowmajor(
			n_rows,
			std::vector<float>(n_cols) 
			);
	for (int col = 0; col < n_cols; ++col) {
		for (int row = 0; row < n_rows; ++row) {
			X_rowmajor[row][col] = X[col][row];
		}
	}

	auto start = std::chrono::high_resolution_clock::now();
	for (int round = 0; round < num_boosting_rounds; ++round) {
		trees.emplace_back(
					X,
					gradient,
					hessian,
					max_depth,
					l2_reg,
					min_child_weight,
					min_data_in_leaf
			);

		round_preds = trees[round].predict(X_rowmajor);
		for (int row = 0; row < n_rows; ++row) {
			preds[row] += lr * round_preds[row];
		}
		gradient = calculate_gradient(preds, y);
		hessian  = calculate_hessian(preds, y);

		loss = calculate_mse_loss(preds, y);
		auto stop = std::chrono::high_resolution_clock::now();
		auto duration = std::chrono::duration_cast<std::chrono::milliseconds>(stop - start);
		if (round % verbosity == 0) {
			std::cout << "Round " << round + 1 << " MSE Loss: " << loss;
			std::cout << "               Time Elapsed: " << duration.count() << std::endl;
		}
	}
}

void GBM::train_hist(
		std::vector<std::vector<float>>& X, 
		std::vector<float>& y
		) {
	// Init random seed.
	srand(42);

	int n_rows = (int)X[0].size();

	std::vector<float> gradient(n_rows, 0.00f);
	std::vector<float> hessian(n_rows, 2.00f);

	auto start_0 = std::chrono::high_resolution_clock::now();

	const std::vector<std::vector<uint8_t>> X_hist = map_hist_bins_train(X, bin_mapping, max_bin);
	const std::vector<std::vector<uint8_t>> X_hist_rowmajor = get_hist_bins_rowmajor(X_hist);

	// Get min/max bin per col to avoid unneccessary split finding. 
	std::vector<std::vector<uint8_t>> min_max_rem = get_min_max_rem(X_hist);

	auto stop_0     = std::chrono::high_resolution_clock::now();
	auto duration_0 = std::chrono::duration_cast<std::chrono::milliseconds>(stop_0 - start_0);

	std::cout << "Hist constructed in " << duration_0.count() << " milliseconds" << std::endl;

	// Add mean for better start.
	y_mean_train = get_vector_mean(y);
	std::vector<std::vector<float>> all_preds;

	std::vector<float> round_preds(n_rows);
	std::vector<float> preds(n_rows, y_mean_train);

	auto start_1 = std::chrono::high_resolution_clock::now();
	for (int round = 0; round < num_boosting_rounds; ++round) {
		trees.emplace_back(
					X_hist,
					gradient,
					hessian,
					max_depth,
					l2_reg,
					min_data_in_leaf,
					max_bin,
					max_leaves,
					col_subsample_rate,
					min_max_rem,
					round
			);


		if (dart) {
			float scale_factor_0 = 0.20f;
			float scale_factor_1 = 1.00f;
			float drop_rate		 = 0.04f;
			
			round_preds = trees[round].predict_hist(X_hist_rowmajor);
			all_preds.push_back(round_preds);

			for (int idx = 0; idx < n_rows; ++idx) {
				preds[idx] = y_mean_train;
			}

			std::vector<int> pred_idxs(std::max(1, round));
			std::iota(pred_idxs.begin(), pred_idxs.end(), 0);

			auto rng = std::default_random_engine{};
			std::shuffle(pred_idxs.begin(), pred_idxs.end(), rng);

			// Use first (rounds * ratio) random idx round preds. 20% dropped.
			
			// Non-dropped trees
			for (int _idx = 0; _idx < int((1.00f - drop_rate) * round) + 1; ++_idx) {
				for (int idx = 0; idx < n_rows; ++idx) {
					preds[idx] += lr * all_preds[pred_idxs[_idx]][idx];
				}
			}


			// Dropped trees with scale factor
			for (int _idx = int((1.00f - drop_rate) * round) + 1; _idx < round; ++_idx) {
				for (int idx = 0; idx < n_rows; ++idx) {
					preds[idx] += lr * scale_factor_0 * all_preds[pred_idxs[_idx]][idx];
				}
			}

			// Current round.
			for (int idx = 0; idx < n_rows; ++idx) {
				preds[idx] += lr * scale_factor_0 * scale_factor_1 * all_preds[round][idx];
			}
		}
		else {
			round_preds = trees[round].predict_hist(X_hist_rowmajor);
			for (int idx = 0; idx < n_rows; ++idx) {
				preds[idx] += lr * round_preds[idx];
			}
		}

		gradient = calculate_gradient(preds, y);
		hessian  = calculate_hessian(preds, y);

		auto stop_1 = std::chrono::high_resolution_clock::now();
		auto duration_1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop_1 - start_1);

		if (round % verbosity == (verbosity - 1)) {
			std::cout << std::fixed << std::setprecision(6);
			std::cout << "Round " << round + 1 << " MSE Loss: " << calculate_mse_loss(preds, y);
			std::cout << "       "; 
			std::cout << "Num leaves: " << (trees[round].num_leaves + 1) / 2;
			std::cout << "       "; 
			std::cout << "Time Elapsed: " << duration_1.count() << " ms" << std::endl;
		}
	}
}


void GBM::train_hist(
		std::vector<std::vector<float>>& X, 
		std::vector<float>& y,
		std::vector<std::vector<float>>& X_validation,
		std::vector<float>& y_validation,
		int early_stopping_steps
		) {
	float validation_loss;
	int   stop_cntr = 0;
	float min_validation_loss = INFINITY;

	if (early_stopping_steps <= 0) {
		early_stopping_steps = num_boosting_rounds;
	}

	int n_rows = int(X[0].size());

	std::vector<float> gradient(n_rows, 0.00f);
	std::vector<float> hessian(n_rows, 2.00f);

	auto start_0 = std::chrono::high_resolution_clock::now();

	const std::vector<std::vector<uint8_t>> X_hist = map_hist_bins_train(X, bin_mapping, max_bin);
	const std::vector<std::vector<uint8_t>> X_hist_rowmajor = get_hist_bins_rowmajor(X_hist);

	// const std::vector<std::vector<uint8_t>> X_hist_val = map_hist_bins_inference(X_validation, bin_mapping);
	const std::vector<std::vector<uint8_t>> X_hist_val = map_hist_bins_train(X_validation, bin_mapping, max_bin);
	const std::vector<std::vector<uint8_t>> X_hist_rowmajor_val = get_hist_bins_rowmajor(X_hist_val);

	// Get min/max bin per col to avoid unneccessary split finding. 
	std::vector<std::vector<uint8_t>> min_max_rem = get_min_max_rem(X_hist);

	auto stop_0     = std::chrono::high_resolution_clock::now();
	auto duration_0 = std::chrono::duration_cast<std::chrono::milliseconds>(stop_0 - start_0);

	std::cout << "Hist constructed in " << duration_0.count() << " milliseconds" << std::endl;

	// Add mean for better start.
	y_mean_train = get_vector_mean(y);
	std::vector<std::vector<float>> all_preds;

	std::vector<float> round_preds(n_rows);
	std::vector<float> preds(n_rows, y_mean_train);
	std::vector<float> validation_preds(n_rows, y_mean_train);

	auto start_1 = std::chrono::high_resolution_clock::now();
	for (int round = 0; round < num_boosting_rounds; ++round) {
		// Check for early stopping.
		if (stop_cntr == early_stopping_steps) {
			std::cout << std::fixed << std::setprecision(6);
			std::cout << "Round " << round << " MSE Train Loss: " << calculate_mse_loss(preds, y);
			std::cout << "       "; 
			std::cout << " MSE Validation Loss: " << validation_loss;
			std::cout << "       "; 
			std::cout << "Num leaves: " << (trees[round-1].num_leaves + 1) / 2 << std::endl << std::endl;
			std::cout << "EARLY STOPPING" << std::endl; 
			return;
		}

		trees.emplace_back(
					X_hist,
					gradient,
					hessian,
					max_depth,
					l2_reg,
					min_data_in_leaf,
					max_bin,
					max_leaves,
					col_subsample_rate,
					min_max_rem,
					round
			);


		// Init random seed.
		srand(42);
		if (dart) {
			float scale_factor_0 = 0.20f;
			float scale_factor_1 = 1.00f;
			float drop_rate		 = 0.04f;
			
			round_preds = trees[round].predict_hist(X_hist_rowmajor);
			all_preds.push_back(round_preds);

			for (int idx = 0; idx < n_rows; ++idx) {
				preds[idx] = y_mean_train;
			}

			std::vector<int> pred_idxs(std::max(1, round));
			std::iota(pred_idxs.begin(), pred_idxs.end(), 0);

			auto rng = std::default_random_engine{};
			std::shuffle(pred_idxs.begin(), pred_idxs.end(), rng);

			// Use first (rounds * ratio) random idx round preds. 20% dropped.
			
			// Non-dropped trees
			for (int _idx = 0; _idx < int((1.00f - drop_rate) * round) + 1; ++_idx) {
				for (int idx = 0; idx < n_rows; ++idx) {
					preds[idx] += lr * all_preds[pred_idxs[_idx]][idx];
				}
			}
			// Dropped trees with scale factor
			for (int _idx = int((1.00f - drop_rate) * round) + 1; _idx < round; ++_idx) {
				for (int idx = 0; idx < n_rows; ++idx) {
					preds[idx] += lr * scale_factor_0 * all_preds[pred_idxs[_idx]][idx];
				}
			}
			// Current round.
			for (int idx = 0; idx < n_rows; ++idx) {
				preds[idx] += lr * scale_factor_0 * scale_factor_1 * all_preds[round][idx];
			}
		}
		else {
			round_preds = trees[round].predict_hist(X_hist_rowmajor);
			for (int idx = 0; idx < n_rows; ++idx) {
				preds[idx] += lr * round_preds[idx];
			}
		}

		gradient = calculate_gradient(preds, y);
		hessian  = calculate_hessian(preds, y);

		auto stop_1 = std::chrono::high_resolution_clock::now();
		auto duration_1 = std::chrono::duration_cast<std::chrono::milliseconds>(stop_1 - start_1);

		validation_loss = compute_validation_loss(X_hist_rowmajor_val, validation_preds, y_validation);
		if (validation_loss < min_validation_loss) {
			stop_cntr = 0;
			min_validation_loss = validation_loss;
		}
		else {
			stop_cntr++;
		}

		if (round % verbosity == (verbosity - 1)) {
			std::cout << std::fixed << std::setprecision(6);
			std::cout << "Round " << round + 1 << " MSE Train Loss: " << calculate_mse_loss(preds, y);
			std::cout << "       "; 
			std::cout << " MSE Validation Loss: " << validation_loss;
			std::cout << "       "; 
			std::cout << "Num leaves: " << (trees[round].num_leaves + 1) / 2;
			std::cout << "       "; 
			std::cout << "Time Elapsed: " << duration_1.count() << " ms" << std::endl;
		}
	}
}


std::vector<float> GBM::predict(std::vector<std::vector<float>>& X) {
	int n_rows = int(X[0].size());
	int n_cols = int(X.size());

	std::vector<std::vector<float>> X_rowmajor(
			n_rows,
			std::vector<float>(n_cols) 
			);
	for (int col = 0; col < n_cols; ++col) {
		for (int row = 0; row < n_rows; ++row) {
			X_rowmajor[row][col] = X[col][row];
		}
	}

	std::vector<float> preds(n_rows, y_mean_train);
	std::vector<float> tree_preds;

	for (int tree_num = 0; tree_num < num_boosting_rounds; ++tree_num) {
		tree_preds = trees[tree_num].predict(X_rowmajor);
		for (int row = 0; row < n_rows; ++row) {
			preds[row] += lr * tree_preds[row];
		}
	}
	return preds;
}


std::vector<float> GBM::predict_hist(std::vector<std::vector<float>>& X) {
	// const std::vector<std::vector<uint8_t>> X_hist = map_hist_bins_inference(X, bin_mapping);
	const std::vector<std::vector<uint8_t>> X_hist = map_hist_bins_train(X, bin_mapping, max_bin);
	const std::vector<std::vector<uint8_t>> X_hist_rowmajor = get_hist_bins_rowmajor(X_hist);

	return __predict_hist(X_hist_rowmajor);
}


std::vector<float> GBM::__predict_hist(const std::vector<std::vector<uint8_t>>& X_hist_rowmajor) {
	std::vector<float> preds(int(X_hist_rowmajor.size()), y_mean_train);
	std::vector<float> tree_preds;

	for (int tree_num = 0; tree_num < int(trees.size()); ++tree_num) {
		tree_preds = trees[tree_num].predict_hist(X_hist_rowmajor);
		for (int row = 0; row < int(X_hist_rowmajor.size()); ++row) {
			preds[row] += lr * tree_preds[row];
		}
	}
	return preds;
}

void GBM::predict_hist_iterative(
		const std::vector<std::vector<uint8_t>>& X_hist_rowmajor,
		std::vector<float>& preds
		) {
	std::vector<float> tree_preds;

	tree_preds = trees[int(trees.size() - 1)].predict_hist(X_hist_rowmajor);
	for (int row = 0; row < int(X_hist_rowmajor.size()); ++row) {
		preds[row] += lr * tree_preds[row];
	}
}


float GBM::compute_validation_loss(
		const std::vector<std::vector<uint8_t>>& X_hist_rowmajor,
		std::vector<float>& preds,
		std::vector<float>& y
		) {
	predict_hist_iterative(X_hist_rowmajor, preds);
	return calculate_mse_loss(preds, y);
}


std::vector<float> GBM::calculate_gradient(std::vector<float>& preds, std::vector<float>& y) {
	std::vector<float> gradient = calc_grad(preds, y, "fair_loss");
	// std::vector<float> gradient = calc_grad(preds, y, "mse_loss");
	return gradient;
}


std::vector<float> GBM::calculate_hessian(std::vector<float>& preds, std::vector<float>& y) {
	std::vector<float> hessian = calc_hess(preds, y, "fair_loss");
	// std::vector<float> hessian = calc_hess(preds, y, "mse_loss");
	return hessian;
}


float GBM::calculate_mse_loss(std::vector<float>& preds, std::vector<float>& y) {
	float loss = 0.00f;

	// Assume MSE for now
	for (int idx = 0; idx < int(y.size()); ++idx) {
		loss += 0.50f * (preds[idx] - y[idx]) * (preds[idx] - y[idx]);
	}
	loss /= float(y.size());	
	return loss;
}


void GBM::train_hist_wrapper(
		const pybind11::array_t<float>& X, 
		const pybind11::array_t<float>& y
		) {
	// Convert pyarray to vec 2d
	std::vector<std::vector<float>> X_vec = np_to_vec2d(X);
	std::vector<float> y_vec = np_to_vec(y);

	train_hist(X_vec, y_vec);
}


/*
void GBM::train_hist_wrapper_validation(
		np::ndarray const& X,
		np::ndarray const& y,
		np::ndarray const& X_validation,
		np::ndarray const& y_validation,
		int early_stopping_steps
		) {
*/
void GBM::train_hist_wrapper_validation(
		const pybind11::array_t<float>& X,
		const pybind11::array_t<float>& y,
		const pybind11::array_t<float>& X_validation,
		const pybind11::array_t<float>& y_validation,
		int early_stopping_steps
		) {
	std::vector<std::vector<float>> X_vec = np_to_vec2d(X);
	std::vector<float> y_vec = np_to_vec(y);

	std::vector<std::vector<float>> X_vec_validation = np_to_vec2d(X_validation);
	std::vector<float> y_vec_validation = np_to_vec(y_validation);

	train_hist(X_vec, y_vec, X_vec_validation, y_vec_validation, early_stopping_steps);
}

// pybind11::array_t<float> GBM::predict_hist_wrapper(np::ndarray const& X) {
pybind11::array_t<float> GBM::predict_hist_wrapper(const pybind11::array_t<float>& X) {
	std::vector<std::vector<float>> X_vec = np_to_vec2d(X);

	// Need static declaration to keep pointer valid.
	static std::vector<float> preds_vec = predict_hist(X_vec);

	/*
	np::dtype dt     = np::dtype::get_builtin<float>();
	p::tuple  shape  = p::make_tuple(int(preds_vec.size()));
	p::tuple  stride = p::make_tuple(sizeof(float));

	return np::from_data(
			preds_vec.data(),
			dt,
			shape,
			stride,
			p::object()	
			);
	*/

	// Create pybind11 array from vector
	pybind11::array_t<float> preds = pybind11::array_t<float>(preds_vec.size(), preds_vec.data());
	return preds;
}

// void GBM::train_hist_gpu_wrapper(np::ndarray const& X, np::ndarray const& y) {
/*
void GBM::train_hist_gpu_wrapper(const pybind11::array_t<float>& X, const pybind11::array_t<float>& y) {
	std::vector<std::vector<float>> X_vec = np_to_vec2d(X);
	std::vector<float> y_vec = np_to_vec(y);

	const std::vector<std::vector<uint8_t>> X_hist = map_hist_bins_train(
			X_vec, 
			bin_mapping, 
			max_bin
			);
	const std::vector<std::vector<uint8_t>> X_hist_rowmajor = get_hist_bins_rowmajor(X_hist);

	train_hist_gpu(X_hist, X_hist_rowmajor, y_vec);
}
*/
