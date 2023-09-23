#include <iostream>
#include <fstream>
#include <vector>
#include <iterator>
#include <string>
#include <sstream>

#include "read_level.hpp"

std::vector<std::vector<int8_t>> read_level_csv(const std::string in_file) {
	std::ifstream file(in_file);
	std::vector<std::vector<int8_t>> level_design;
	std::vector<int8_t> row;
	std::string line;

	// Newline separated rows
	// Rows are comma separated ints
	while (std::getline(file, line)) {
		std::stringstream ss(line);
		std::string cell;
		while (std::getline(ss, cell, ',')) {
			row.push_back(std::stoi(cell));
		}
		level_design.push_back(row);
		row.clear();
	}
	file.close();
	return level_design;
}
