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

std::vector<std::vector<float>> np_to_vec2d_test(np::ndarray const& X) {

	std::vector<std::vector<float>> X_vec;
	std::vector<float> x_vec_col;

	int x_dim = X.shape(0);
	int y_dim = X.shape(1);

	char* X_arr = X.get_data();

	for (int i = 0; i < x_dim; ++i) {
		for (int j = 0; j < y_dim; ++j) {
			x_vec_col.push_back(float(X_arr[i * x_dim + j]));
		}
		X_vec.emplace_back(x_vec_col);
		x_vec_col.clear();
	}
	return X_vec;
}


BOOST_PYTHON_MODULE(GBDT) {
	Py_Initialize();
	np::initialize();

	def("test", test);

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
				bool
				>()
					/*
				arg("a"),
				arg("b"),
				arg("c"),
				arg("d"),
				arg("e"),
				arg("f"),
				arg("g"),
				arg("h"),
				arg("i"),
				arg("j")
			)
				*/
		)
		.def("train_hist", &GBM::train_hist_wrapper, args("X", "y"))
		;
}
