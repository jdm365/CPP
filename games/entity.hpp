#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "math.hpp"


class Entity {
	public:
		const int PLAYER_SIZE = 48;
		Entity(Vector2f player_pos, int w, int h, SDL_Texture* player_texture);
		Vector2f& get_pos() {
			return pos;
		}
		SDL_Rect get_current_frame();
		SDL_Texture* get_texture();
		Vector2f pos;
	private:
		SDL_Rect current_frame;
		SDL_Texture* texture;
};
