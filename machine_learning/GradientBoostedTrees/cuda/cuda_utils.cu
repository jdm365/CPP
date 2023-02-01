#include <iostream>
#include <vector>
#include <array>
#include <algorithm>
#include <stdlib.h>

#include <thrust/device_vector.h>
#include <thrust/host_vector.h>

#include "../include/utils.hpp"

/*
#include <boost/python.hpp>
#include <boost/python/numpy.hpp>

namespace np = boost::python::numpy;
*/


thrust::device_vector<uint8_t> convert_hist_to_cuda(const std::vector<std::vector<uint8_t>>& X_hist) {
	// Assumes X_hist is columnar.
	int n_cols = int(X_hist.size());
	int n_rows = int(X_hist[0].size());

	thrust::device_vector<uint8_t> hist(n_cols * n_rows);

	for (int col = 0; col < n_cols; ++col) {
		thrust::copy(X_hist[col].begin(), X_hist[col].end(), hist.begin() + n_rows * col);
	}
	return hist;
}
