#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/math.hpp"


class Entity {
	public:
		Entity();
		Entity(
				Vector2f pos, 
				Vector2f vel, 
				int width, 
				int height, 
				const char* type,
				SDL_Texture* player_texture,
				bool collidable
				);
		SDL_Texture* get_texture();
		Vector2f pos;
		Vector2f vel;
		int width;
		int height;
		const char* type;
		int gravity;
		bool collidable;

	private:
		SDL_Texture* texture;
}; 
