#include <boost/python.hpp>
#include <iostream>
#include <iterator>
#include <pylifecycle.h>
#include <vector>
#include <array>

#include "../include/read_data.hpp"
#include "../include/utils.hpp"
#include "../include/gbm.hpp"


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

BOOST_PYTHON_MODULE(GBDT_) {
	Py_Initialize();	
	boost::python::def("test", test);
}
