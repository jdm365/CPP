#pragma once

#include <vector>
#include <string>

int get_dims(std::string* line);
std::vector<std::vector<float>> read_csv_columnar(const char* filename);
std::vector<std::vector<float>> read_csv_rowmajor(const char* filename);
