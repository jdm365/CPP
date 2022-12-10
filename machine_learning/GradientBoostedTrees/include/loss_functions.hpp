#pragma once

#include <iostream>
#include <vector>
#include <math.h>



inline std::vector<float> calc_grad(
		std::vector<float>& preds,
		std::vector<float>& y,
		const char* loss_fn
		) {
	std::vector<float> gradient;
	gradient.reserve(y.size());

	const char* mse_loss  = "mse_loss";
	const char* fair_loss = "fair_loss";

	if (loss_fn == mse_loss) {
		for (int idx = 0; idx < int(y.size()); ++idx) {
			gradient.push_back(2.00f * (preds[idx] - y[idx]));
		}
	}
	else if (loss_fn == fair_loss) {
		float c = 2.00f;
		float val;
		for (int idx = 0; idx < int(y.size()); ++idx) {
			val = preds[idx] - y[idx];
			gradient.push_back(c * val / ((std::abs(val) + c)));
		}
	}
	else {
		std::cout << std::endl << loss_fn;
		throw " not supported";	
	}
	return gradient;
}


inline std::vector<float> calc_hess(
		std::vector<float>& preds,
		std::vector<float>& y,
		const char* loss_fn
		) {
	std::vector<float> hessian;
	hessian.reserve(y.size());

	const char* mse_loss  = "mse_loss";
	const char* fair_loss = "fair_loss";

	if (loss_fn == mse_loss) {
		for (int idx = 0; idx < int(y.size()); ++idx) {
			hessian.push_back(2.00f);
		}
	}
	else if (loss_fn == fair_loss) {
		float c = 2.00f;
		float val;
		for (int idx = 0; idx < int(y.size()); ++idx) {
			val = preds[idx] - y[idx];
			hessian.push_back(c * c / ((std::abs(val) + c) * (std::abs(val) + c)));
		}
	}
	else {
		std::cout << std::endl << loss_fn;
		throw " not supported";	
	}
	return hessian;
}
