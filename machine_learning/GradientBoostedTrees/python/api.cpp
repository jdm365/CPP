#include <pybind11/pybind11.h>
#include <pybind11/numpy.h>

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
				>()
		)
		.def("train_hist", &GBM::train_hist_wrapper)
		.def("train_hist", &GBM::train_hist_wrapper_validation)
		.def("predict_hist", &GBM::predict_hist_wrapper)
		// .def("train_hist_gpu", &GBM::train_hist_gpu_wrapper, args("X", "y"))
		;
}
