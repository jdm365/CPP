#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.hpp"
#include "event_listener.hpp"
#include "constants.h"

Entity handle(SDL_Event event, Entity player_entity, bool game_running) {
	if (event.type == SDL_QUIT) {
		game_running = false;
	}

	if (event.type == SDL_KEYDOWN) {
		switch(event.key.keysym.sym) {
			case SDLK_LEFT:
				player_entity.vel = Vector2f(-10, player_entity.get_vel().y);
				break;
			case SDLK_RIGHT:
				player_entity.vel = Vector2f(10, player_entity.get_vel().y);
				break;
			case SDLK_UP:
				if (player_entity.get_pos().y == GROUND_HEIGHT) {
					player_entity.vel = Vector2f(player_entity.get_vel().x, -10);
					break;
				}
		}
	}
	if (event.type == SDL_KEYUP) {
		switch(event.key.keysym.sym) {
			case SDLK_LEFT:
				player_entity.vel = Vector2f(0, player_entity.get_vel().y);
			case SDLK_RIGHT:
				player_entity.vel = Vector2f(0, player_entity.get_vel().y);
		}
	}
	return player_entity;
}

Entity update(Entity player_entity) {

	float g = 0;
	if (player_entity.get_pos().y < GROUND_HEIGHT) {
		g = 2;
	}

	float y_vel = player_entity.get_vel().y + g;
	int y_pos = player_entity.get_pos().y + player_entity.get_vel().y;

	if (player_entity.get_pos().y > GROUND_HEIGHT) {
		y_vel = 0;
		y_pos = GROUND_HEIGHT;
	}

	player_entity.pos = Vector2f(
			player_entity.get_pos().x + player_entity.get_vel().x,
			y_pos
			);
	player_entity.vel = Vector2f(
			player_entity.get_vel().x,
			y_vel
			);
	
	return player_entity;
}
