#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/math.hpp"

enum EntityType {
	BACKGROUND,
	PLAYER,
	ENEMY_WALKING,
	ENEMY_FLYING,
	GROUND,
	WEAPON,
	PROJECTILE
};

enum CollisionType {
	TRANSPARENT,
	STATIC,
	DYNAMIC
};


class Entity {
	public:
		Entity() {}
		Entity(
				Vector2f pos, 
				Vector2f vel, 
				int width, 
				int height, 
				uint8_t entity_type,
				uint8_t collision_type,
				SDL_Texture* texture,
				float angle_rad = 0.0f
				);
		Vector2f pos;
		Vector2f vel;
		int width;
		int height;
		uint8_t entity_type;
		uint8_t collision_type;

		bool on_ground = true;
		bool facing_right = true;
		bool alive = true;
		int  health = 100;
		Vector2f respawn_pos;
		Vector2f respawn_vel;
		bool overhang = true;
		int  score = 0;			// TODO: Move somewhere else.
		bool collisions[4] = {false, false, false, false};
		Vector2f distance_from_player = Vector2f {0.0f, 0.0f};
		float angle_rad = 0.0f;
		SDL_Texture* texture;

		SDL_Texture* get_texture() {
			return texture;
		}
		bool is_collidable() {
			return this->collision_type != TRANSPARENT;
		}
}; 
