#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "math.hpp"


class Entity {
	public:
		Entity();
		Entity(
				Vector2f entity_pos, 
				Vector2f entity_vel, 
				int w, 
				int h, 
				const char* entity_type,
				SDL_Texture* player_texture
				);
		SDL_Texture* get_texture();
		const char* type;
		int width;
		int height;
		Vector2f pos;
		Vector2f vel;
		int gravity;
	private:
		SDL_Texture* texture;
}; 
