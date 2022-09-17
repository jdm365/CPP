#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "math.hpp"


class Entity {
	public:
		Entity(
				Vector2f player_pos, 
				Vector2f player_vel, 
				int w, 
				int h, 
				SDL_Texture* player_texture
				);
		Vector2f& get_pos() {
			return pos;
		}
		Vector2f& get_vel() {
			return vel;
		}
		SDL_Rect get_current_frame();
		SDL_Texture* get_texture();
		Vector2f pos;
		Vector2f vel;
	private:
		SDL_Rect current_frame;
		SDL_Texture* texture;
};
