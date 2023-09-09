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
				uint8_t type,
				SDL_Texture* player_texture,
				bool collidable,
				bool static_entity = false
				);
		Vector2f pos;
		Vector2f vel;
		int width;
		int height;
		uint8_t type;
		bool collidable;
		bool static_entity = true;
		bool facing_right = true;
		bool alive = true;
		int health = 100;
		bool bottom_collision = true;
		Vector2f respawn_pos;
		Vector2f respawn_vel;
		bool overhang = true;
		int score = 0;
		bool collisions[4] = {false, false, false, false};
		Vector2f distance_from_player = Vector2f {0.0f, 0.0f};

		SDL_Texture* get_texture();

	private:
		SDL_Texture* texture;
}; 

enum EntityType {
	BACKGROUND,
	PLAYER,
	ENEMY,
	GROUND,
	PROJECTILE
};
