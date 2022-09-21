#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>

#include "read_level.hpp"

std::vector<int> read_level_csv(const char* in_file) {
	std::ifstream file(in_file);
	std::vector<int> level_design;
	std::string line;
	char comma = ',';

	while (getline(file, line)) {
		for (auto &element: line) {
			if (element != comma) {
				int x = int(element) - 48;
				level_design.push_back(x);
			}
		}
	}
	file.close();
	return level_design;
}
