#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.hpp"
#include "event_handler.hpp"
#include "constants.h"

void detect_collisions(
		std::vector<bool>& collisions,
		Entity& player_entity,
		std::vector<Entity>& collidable_entities,
		int scroll_factor_x
		) {
	// Reset collisions to false
	for (int idx = 0; idx < 2; ++idx) {
		collisions[idx] = false;
	}

	bool left_range;
	bool right_range;

	bool top_range;
	bool bottom_range;

	for (Entity& entity: collidable_entities) {
		// Only detect collisions for objects near player.
		if (collisions[0] && collisions[1]) {
			break;
		}

		left_range   = entity.pos.x + entity.width <= player_entity.pos.x;
		right_range  = player_entity.width + player_entity.pos.x >= entity.pos.x;

		top_range    = entity.pos.y + entity.height <= player_entity.pos.y;
		bottom_range = player_entity.height + player_entity.pos.y >= entity.pos.y;

		// Is collision 
		if (left_range && right_range && top_range && bottom_range) {
			if (player_entity.vel.x != 0.00f) {
				collisions[0] = true;
			}
			if (player_entity.vel.y >= 0.00f) {
				collisions[1] = true;
			}
		}
	}
	std::cout << collisions[0] << std::endl;
	std::cout << collisions[1] << std::endl << std::endl;
}

void handle(
		SDL_Event& event, 
		Entity& player_entity, 
		std::vector<bool>& collisions
		) {
	// Collisions -> horizontal: 0, vertical: 1
	// Update player velocity.
	if (event.type == SDL_KEYDOWN) {
		switch(event.key.keysym.sym) {
			case SDLK_LEFT:
				player_entity.vel = Vector2f(
						-PLAYER_SPEED * float(!collisions[0]), 
						player_entity.vel.y
						);
				break;
			case SDLK_RIGHT:
				player_entity.vel = Vector2f(
						PLAYER_SPEED * float(!collisions[0]), 
						player_entity.vel.y
						);
				break;
			case SDLK_UP:
				if (player_entity.vel.y == 0) {
					player_entity.vel = Vector2f(
							player_entity.vel.x,
							-JUMP_SPEED
							);
				}
				break;
			case SDLK_SPACE:
				player_entity.gravity = GRAVITY - ROCKET_POWER;
				break;
 		}
	}
	if (event.type == SDL_KEYUP) {
		switch(event.key.keysym.sym) {
			case SDLK_LEFT:
				player_entity.vel = Vector2f(
						0,
						player_entity.vel.y
						);
				break;
			case SDLK_RIGHT:
				player_entity.vel = Vector2f(
						0,
						player_entity.vel.y
						);
				break;
			case SDLK_SPACE:
				player_entity.gravity = GRAVITY;
				break;
		}
	}
}

void update(
		Entity& player_entity, 
		std::vector<bool>& collisions
		) {
	// Collisions -> horizontal: 0, vertical: 1

	player_entity.vel.x *= float(!collisions[0]);

	player_entity.vel.y += player_entity.gravity;
	player_entity.vel.y *= float(!collisions[1]);

	/*
	// Cap velocity
	if (player_entity.vel.y >= 0) {
		player_entity.vel.y = std::min(player_entity.vel.y, float(3 * PLAYER_SPEED));
	}
	else {
		player_entity.vel.y = std::max(player_entity.vel.y, float(-3 * PLAYER_SPEED));
	}
	*/

	player_entity.pos.x += player_entity.vel.x;
	player_entity.pos.y += player_entity.vel.y;

	// Respawn if player falls off map.
	if (player_entity.pos.y > WINDOW_HEIGHT) {
		player_entity.pos.x = 100.00f;
		player_entity.pos.y = float(PLATFORM_HEIGHT - 250);
		player_entity.vel.x = 0.00f;
		player_entity.vel.y = 0.00f;
	}
}
