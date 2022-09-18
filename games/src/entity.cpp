#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.hpp"

Entity::Entity(
		Vector2f player_pos, 
		Vector2f player_vel, 
		int w,
		int h,
		int entity_type,
		SDL_Texture* default_texture
		) {

	pos = player_pos;
	vel = player_vel;
	texture = default_texture;
	type = entity_type;

	current_frame.x = 0;
	current_frame.y = 0;
	current_frame.w = w;
	current_frame.h = h;
}

SDL_Texture* Entity::get_texture() {
	return texture;
}

SDL_Rect Entity::get_current_frame() {
	return current_frame;
}
