#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.hpp"
#include "event_handler.hpp"
#include "constants.h"

Entity handle(SDL_Event event, Entity player_entity) {
	// Update player velocity.
	if (event.type == SDL_KEYDOWN) {
		switch(event.key.keysym.sym) {
			case SDLK_LEFT:
				// Check left collision
				player_entity.vel = Vector2f(-PLAYER_SPEED, player_entity.vel.y);
				break;
			case SDLK_RIGHT:
				player_entity.vel = Vector2f(PLAYER_SPEED, player_entity.vel.y);
				break;
			case SDLK_UP:
				if (player_entity.pos.y == GROUND_HEIGHT) {
					player_entity.vel = Vector2f(player_entity.vel.x, -JUMP_SPEED);
					break;
				}
		}
	}
	if (event.type == SDL_KEYUP) {
		switch(event.key.keysym.sym) {
			case SDLK_LEFT:
				player_entity.vel = Vector2f(0, player_entity.vel.y);
			case SDLK_RIGHT:
				player_entity.vel = Vector2f(0, player_entity.vel.y);
		}
	}
	return player_entity;
}

// TODO: Combine these two functions and add collision detection
// 		 between platform (grass) and player objects.
Entity update(Entity player_entity) {
	// Set gravity if player is in the air.
	float g = 0.00f;
	if (player_entity.pos.y < GROUND_HEIGHT) {
		g = GRAVITY;
	}

	float x_vel = player_entity.vel.x;
	float x_pos = float(int(player_entity.pos.x + player_entity.vel.x) % WINDOW_WIDTH);
	float y_vel = player_entity.vel.y + g;
	float y_pos = player_entity.pos.y + player_entity.vel.y;

	if (player_entity.pos.y > GROUND_HEIGHT) {
		y_vel = 0.00f;
		y_pos = GROUND_HEIGHT;
	}

	player_entity.pos = Vector2f(x_pos, y_pos);
	player_entity.vel = Vector2f(x_vel, y_vel);
	
	return player_entity;
}
