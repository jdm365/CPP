#include <cmath>

#include "../include/math.hpp"


Vector2f vector_add(Vector2f& a, Vector2f& b) {
	return {a.x + b.x, a.y + b.y};
}

Vector2f vector_sub(Vector2f& a, Vector2f& b) {
	return {a.x - b.x, a.y - b.y};
}

float l2_norm(Vector2f& a) {
	float norm = sqrt(a.x * a.x + a.y * a.y);
	return norm;
}
