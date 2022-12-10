#include <pybind11/stl.h>
#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>
#include <vector>

#include "../include/gbm.hpp"

namespace py = pybind11;


/*
PYBIND11_MODULE(GradientBoostedTrees, module) {
	module.doc() = "Histogram Gradient Boosted Trees Module";

	py::class_<GBM>(module, "GradientBoostedTrees")
		.def(
				py::init<int, float, float, float, int, int, int, int, float, bool>(), 
				py::arg("_max_depth"),
				py::arg("_l2_reg"),
				py::arg("_lr"),
				py::arg("_min_child_weight"),
				py::arg("_min_data_in_leaf"),
				py::arg("_num_boosting_rounds"),
				py::arg("_max_bin"),
				py::arg("_max_leaves"),
				py::arg("_col_subsample_rate"),
				py::arg("_dart")
			)
		.def(
				"train_hist", 
				&GBM::train_hist, 
				py::arg("X"), 
				py::arg("y")
			)
		.def(
				"train_greedy", 
				py::overload_cast<>(&GBM::train_greedy, py::const_), 
				py::arg("X"), 
				py::arg("y")
			);
}
*/


// Wrap C++ function with np.array I/O
void py_train_hist(
		// py::array_t<float, py::array::f_style | py::array::forecast> X,
		// py::array_t<float, py::array::f_style | py::array::forecast> y
		py::array_t<float, py::array::f_style> X,
		py::array_t<float, py::array::f_style> y
		// py::array_t<float> X,
		// py::array_t<float> y
		) {
	// For testing purposes initialize GBM within function. 
	// Make separate pyclass later.
		
	GBM model(
			-1,				// max_depth (<= 0 implies no limit)
			0.00f,			// l2_reg
			0.10f,			// lr
			1.00f,			// min_child_weight (NOT USED IN HIST)
			20,				// min_data_in_leaf
			50,				// num_boosting_rounds
			255,			// max_bin
			127,			// max_leaves
			0.80f,			// col_subsample_rate [0.0, 1.0]
			true			// enable_dart
			);

	int n_cols = int(X.size() / y.size());
	int n_rows = int(y.size());

	std::vector<std::vector<float>> X_mat;
	std::vector<float> X_vec(X.size());
	std::vector<float> _X_vec(y.size());

	std::memcpy(X_vec.data(), X.data(), X.size() * sizeof(float));

	// allocate std::vectors
	for (py::ssize_t col = 0; col < n_cols; ++col) {
		for (py::ssize_t row = 0; row < n_rows; ++row) {
			_X_vec.push_back((float)X_vec[n_rows * col + row]);
		}
		X_mat.emplace_back(X_vec);
		_X_vec.clear();
	}

	std::vector<float> y_vec(y.size());
	std::memcpy(y_vec.data(), y.data(), y.size() * sizeof(float));


	// Call C++ function
	model.train_hist(X_mat, y_vec);
}


PYBIND11_MODULE(GBDT, module) {
	module.doc() = "Histogram Gradient Boosted Trees Module";

	module.def("train_hist", &py_train_hist);
}
