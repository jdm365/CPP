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
#include "../include/gbm.hpp"



using namespace boost::python;
namespace np = boost::python::numpy;

BOOST_PYTHON_MODULE(GBDT_) {
	Py_Initialize();
	np::initialize();

	// def("test", test);

	class_<GBM>("GBM", init<>())
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
				bool>()
			)
		//.def("train_hist", &GBM::train_hist, args("X", "y"))
		;
}


/*
void train_hist_wrapper(
		GBM& gbm,
		np::ndarray const& X,
		np::ndarray const& y
		) {

	std::vector<std::vector<float>> X_vec;
	std::vector<float> x_vec_col;
	std::vector<float> y_vec;

	int x_dim = X.shape(0);
	int y_dim = y.shape(0);

	char* X_arr = X.get_data();
	char* y_arr = y.get_data();

	for (int i = 0; i < x_dim; ++i) {
		for (int j = 0; j < y_dim; ++j) {
			x_vec_col.push_back(float(X_arr[i * x_dim + j]));
		}
		X_vec.emplace_back(x_vec_col);
		x_vec_col.clear();
	}

	for (int i = 0; i < y_dim; ++i) {
		y_vec.push_back(y_arr[i]);
	}

	gbm.train_hist(X_vec, y_vec);
}
*/



void test() {
	int SIZE = 100000000;

	std::vector<int> vec1;
	std::vector<int> vec2;
	std::vector<int> vec3;

	vec1.reserve(SIZE);
	vec2.reserve(SIZE);
	vec3.reserve(SIZE);

	for (int idx = 0; idx < SIZE; ++idx) {
		vec1.push_back(2);
		vec2.push_back(4);
	}

	for (int idx = 0; idx < SIZE; ++idx) {
		vec3.push_back(vec1[idx] * vec2[idx]);
	}
}
