#pragma once

struct {
	float x;
	float y;
} typedef Vector2f;

struct {
	int x;
	int y;
} typedef Vector2i;

Vector2f vector_add(Vector2f& a, Vector2f& b);
Vector2f vector_sub(Vector2f& a, Vector2f& b);
