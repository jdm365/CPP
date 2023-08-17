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
				bool collidable,
				bool static_entity = false
				);
		SDL_Texture* get_texture();
		Vector2f pos;
		Vector2f vel;
		int width;
		int height;
		const char* type;
		int gravity;
		bool collidable;
		float speed_multiplier = 1.0f;
		bool static_entity = true;
		bool facing_right = true;
		bool alive = true;
		int health = 100;

	private:
		SDL_Texture* texture;
}; 
