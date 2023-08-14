#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/entity.hpp"
#include "../include/constants.h"

Entity::Entity() {
	pos 	= Vector2f();
	vel 	= Vector2f();
	texture = NULL;
	type 	= NULL;
	width   = 0;
	height  = 0;
	gravity = GRAVITY;
	collidable = false;
}

Entity::Entity(
		Vector2f pos, 
		Vector2f vel, 
		int width,
		int height,
		const char* type,
		SDL_Texture* default_texture,
		bool collidable
		) : pos(pos), 
			vel(vel), 
			width(width), 
			height(height), 
			type(type), 
			collidable(collidable) {

	texture = default_texture;
	gravity = GRAVITY;
}

SDL_Texture* Entity::get_texture() {
	return texture;
}
