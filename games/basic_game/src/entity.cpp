#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/entity.hpp"
#include "../include/constants.h"


Entity::Entity(
		Vector2f pos, 
		Vector2f vel, 
		int width,
		int height,
		uint8_t entity_type,
		uint8_t collision_type,
		SDL_Texture* texture,
		float angle_rad
		) : pos(pos), 
			vel(vel), 
			width(width), 
			height(height), 
			entity_type(entity_type),
			collision_type(collision_type),
			angle_rad(angle_rad),
			texture(texture) {

	respawn_pos = pos;
	respawn_vel = vel;
}
