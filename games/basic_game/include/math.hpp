#pragma once

#include <iostream>

struct Vector2f {
	float x, y;

	// Overloading vector constructors.
	Vector2f() {
		x = 0.00f;
		y = 0.00f;
	}

	Vector2f(float x_in, float y_in) {
		x = x_in; 
		y = y_in;
	}
};
