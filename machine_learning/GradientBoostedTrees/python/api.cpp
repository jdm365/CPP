#include <boost/python.hpp>
#include <boost/python/numpy.hpp>
#include <boost/smart_ptr/detail/yield_k.hpp>
#include <pylifecycle.h>

#include <iostream>
#include <iterator>
#include <vector>
#include <array>

#include "../include/read_data.hpp"
#include "../include/utils.hpp"
#include "../include/tree.hpp"
#include "../include/gbm.hpp"


using namespace boost::python;
namespace np = boost::python::numpy;


BOOST_PYTHON_MODULE(GBDTEngine) {
	Py_Initialize();
	np::initialize();

	class_<GBM>("GBM")
		.def(init<
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
		.def("train_hist", &GBM::train_hist_wrapper, args("X", "y"))
		.def("train_hist", &GBM::train_hist_wrapper_validation, args(
					"X", 
					"y", 
					"X_validation", 
					"y_validation",
					"early_stopping_steps"
					))
		.def("predict_hist", &GBM::predict_hist_wrapper, args("X"))
		;
}
