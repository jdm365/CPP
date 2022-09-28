#include <iostream>
#include <vector>

#include "read_data.hpp"

int main() {
	const char* FILENAME = "data/iris_copy.csv";
	std::vector<std::vector<float>> X = read_csv(FILENAME);

	std::cout << "Number of columns: " << X.size() << std::endl;
	std::cout << "Number of rows:    " << X[0].size() << std::endl;
	return 0;
}
