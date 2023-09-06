#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/entity.hpp"
#include "../include/constants.h"

Entity::Entity() {
	pos 	= Vector2f {};
	vel 	= Vector2f {};
	texture = NULL;
	type 	= GROUND;
	width   = 0;
	height  = 0;
	collidable = false;
}

Entity::Entity(
		Vector2f pos, 
		Vector2f vel, 
		int width,
		int height,
		uint8_t type,
		SDL_Texture* default_texture,
		bool collidable,
		bool static_entity
		) : pos(pos), 
			vel(vel), 
			width(width), 
			height(height), 
			type(type), 
			collidable(collidable),
			static_entity(static_entity) {

	respawn_pos = pos;
	texture = default_texture;
}

SDL_Texture* Entity::get_texture() {
	return texture;
}
