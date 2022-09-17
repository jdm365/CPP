#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

namespace utils {
	float hire_time_in_seconds() {
		float t = SDL_GetTicks();
		t *= 0.001f;
		return t;
	}
}
