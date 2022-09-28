#pragma once

#include <vector>
#include <string>

int get_dims(std::string* line);
std::vector<std::vector<float>> read_csv(const char* filename);
