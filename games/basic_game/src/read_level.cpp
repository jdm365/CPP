#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>

#include "read_level.hpp"

std::vector<int> read_level_csv(const char* in_file) {
	std::ifstream file(in_file);
	std::vector<int> level_design;
	std::string line;

	while (std::getline(file, line, ',')) {
		int val = std::stoi(line);
		level_design.push_back(val);
	}
	file.close();
	return level_design;
}
