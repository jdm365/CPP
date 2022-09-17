#pragma once

#include <iostream>

struct Vector2f {
	Vector2f(): x(0.0f), y(0.0f) {}

	Vector2f(float player_x, float player_y): x(player_x), y(player_y) {}

	void print() {
		std::cout << x << "," << y << std::endl;
	}

	float x, y;
};
