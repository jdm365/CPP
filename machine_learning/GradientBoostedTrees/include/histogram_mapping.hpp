#include <iostream>
#include <vector>


std::vector<std::vector<uint8_t>> map_hist_bins_train(
		std::vector<std::vector<float>>& X,
		std::vector<std::vector<float>>& bin_mapping,
		int& max_bin
		);
std::vector<std::vector<uint8_t>> map_hist_bins_inference(
		const std::vector<std::vector<float>>& X,
		std::vector<std::vector<float>>& bin_mapping
		);
std::vector<std::vector<uint8_t>> get_hist_bins_rowmajor(
		const std::vector<std::vector<uint8_t>>& X_hist
		);
std::vector<std::vector<uint8_t>> get_min_max_rem(
		const std::vector<std::vector<uint8_t>>& X_hist
		);
