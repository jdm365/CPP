#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.hpp"

Entity::Entity(
		Vector2f entity_pos, 
		Vector2f entity_vel, 
		int w,
		int h,
		int entity_type,
		SDL_Texture* default_texture
		) {

	pos 	= entity_pos;
	vel 	= entity_vel;
	texture = default_texture;
	type 	= entity_type;
	width   = w;
	height  = h;
}

SDL_Texture* Entity::get_texture() {
	return texture;
}
