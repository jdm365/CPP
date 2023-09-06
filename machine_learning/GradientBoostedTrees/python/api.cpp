#include <pybind11/pybind11.h>
#include <pybind11/stl.h>

#include <iostream>
#include <iterator>
#include <vector>
#include <array>

#include "../include/read_data.hpp"
#include "../include/utils.hpp"
#include "../include/tree.hpp"
#include "../include/gbm.hpp"



PYBIND11_MODULE(GBDTEngine, m) {
	m.doc() = "gbdt engine";

	pybind11::class_<GBM>(m, "GBM")
		.def(pybind11::init<
				int,
				float,
				float,
				float,
				int,
				int,
				int,
				int,
				float,
				bool,
				int
				>(),
				pybind11::arg("max_depth"),
				pybind11::arg("l2_reg"),
				pybind11::arg("lr"),
				pybind11::arg("min_child_weight"),
				pybind11::arg("min_data_in_leaf"),
				pybind11::arg("num_boosting_rounds"),
				pybind11::arg("max_bin"),
				pybind11::arg("max_leaves"),
				pybind11::arg("col_subsample_rate"),
				pybind11::arg("enable_dart"),
				pybind11::arg("verbosity")
		)
		.def("train_hist", &GBM::train_hist_wrapper, pybind11::arg("X"), pybind11::arg("y"))
		.def("train_hist", &GBM::train_hist_wrapper_validation)
		.def("predict_hist", &GBM::predict_hist_wrapper, pybind11::arg("X"))
		// .def("train_hist_gpu", &GBM::train_hist_gpu_wrapper, args("X", "y"))
		;
}
