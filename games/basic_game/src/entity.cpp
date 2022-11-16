#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.hpp"
#include "constants.h"

Entity::Entity() {
	pos 	= Vector2f();
	vel 	= Vector2f();
	texture = NULL;
	type 	= NULL;
	width   = 0;
	height  = 0;
	gravity = GRAVITY;
}

Entity::Entity(
		Vector2f entity_pos, 
		Vector2f entity_vel, 
		int w,
		int h,
		const char* entity_type,
		SDL_Texture* default_texture
		) {

	pos 	= entity_pos;
	vel 	= entity_vel;
	texture = default_texture;
	type 	= entity_type;
	width   = w;
	height  = h;
	gravity = GRAVITY;
}

SDL_Texture* Entity::get_texture() {
	return texture;
}
