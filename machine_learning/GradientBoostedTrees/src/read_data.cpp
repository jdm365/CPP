#include <iostream>
#include <vector>
#include <string>
#include <fstream>
#include <sstream>
#include <stdio.h>

#include "read_data.hpp"

int get_dims(std::string* line) {
	int dims = 1;
	for (char c: *line) {
		if (c == ',') {
			dims += 1;
		}
	}
	return dims;
}

// Reads csv into columnar data matrix.
// Data must be numeric.
std::vector<std::vector<float>> read_csv(const char* filename) {
	std::vector<std::vector<float>> X;
	std::ifstream file(filename);
	std::string line;
	int dims = 0;

	bool first_line = true;
	while (std::getline(file, line)) {
		// Extra check to break if file ends.
		if (line == "") {
			break;
		}

		std::stringstream stream(line);
		std::string num;
		int idx = 0;
		
		// Get dimensions from first line and instantiate matrix.
		if (first_line) {
			dims = get_dims(&line);
			first_line = false;

			while (idx < dims) {
				std::getline(stream, num, ',');
				std::vector<float> tmp = {std::stof(num)};
				X.push_back(tmp);
				idx++;
			}
		}
		else {
			while (idx < dims) {
				std::getline(stream, num, ',');
				X[idx].push_back(std::stof(num));
				idx++;
			}
		}
	}
	file.close();
	return X;
}
