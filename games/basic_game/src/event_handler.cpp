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
	for (int idx = 0; idx < 4; ++idx) {
		collisions[idx] = false;
	}

	int left_edge_p   = int(player_entity.pos.x);
	int top_edge_p 	  = int(player_entity.pos.y);
	int right_edge_p  = int(player_entity.pos.x) + player_entity.width;
	int bottom_edge_p = int(player_entity.pos.y) + player_entity.height;

	int diff_width;
	int diff_height;

	int left_edge_obj;
	int top_edge_obj;
	int right_edge_obj;
	int bottom_edge_obj;


	for (Entity& entity: collidable_entities) {
		// Only detect collisions for objects near player.

		diff_width  = player_entity.width + entity.width;
		if (std::abs(entity.pos.x - player_entity.pos.x) > diff_width) {
			continue;
		}

		diff_height = player_entity.height + entity.height;
		if (std::abs(entity.pos.y - player_entity.pos.y) > diff_height) {
			continue;
		}

		left_edge_obj   = entity.pos.x;
		top_edge_obj 	= entity.pos.y;
		right_edge_obj  = entity.pos.x + entity.width;
		bottom_edge_obj = entity.pos.y + entity.height;


		// Left collision
		if (left_edge_p <= right_edge_obj) {
			collisions[0] = true;
		}
		else {
			collisions[0] = false;
		}

		// Top collision
		if (top_edge_p <= bottom_edge_obj) {
			collisions[1] = true;
		}
		else {
			collisions[1] = false;
		}

		// Right collision
		if (right_edge_p >= left_edge_obj) {
			collisions[2] = true;
		}
		else {
			collisions[2] = false;
		}

		// Bottom collision
		if (bottom_edge_p >= top_edge_obj) {
			collisions[3] = true;
		}
		else {
			collisions[3] = false;
		}
	}
}

Entity handle(
		SDL_Event& event, 
		Entity& player_entity, 
		std::vector<bool>& collisions
		) {

	// Collisions -> left: 0, top: 1, right: 2, bottom: 3
	// Update player velocity.
	if (event.type == SDL_KEYDOWN) {
		switch(event.key.keysym.sym) {
			case SDLK_LEFT:
				player_entity.vel = Vector2f(
						-PLAYER_SPEED * float(!collisions[0]), 
						player_entity.vel.y * float(!(collisions[1] && collisions[3]))
						);
				break;
			case SDLK_RIGHT:
				player_entity.vel = Vector2f(
						PLAYER_SPEED * float(!collisions[2]), 
						player_entity.vel.y * float(!(collisions[1] && collisions[3]))
						);
				break;
			case SDLK_UP:
				if ((player_entity.vel.y == 0) && collisions[3]) {
					player_entity.vel = Vector2f(
							player_entity.vel.x * float(!(collisions[0] && collisions[2])),
							-JUMP_SPEED * float(!collisions[1])
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
						player_entity.vel.y * float(!(collisions[1] && collisions[3]))
						);
				break;
			case SDLK_RIGHT:
				player_entity.vel = Vector2f(
						0, 
						player_entity.vel.y * float(!(collisions[1] && collisions[3]))
						);
				break;
			case SDLK_SPACE:
				player_entity.gravity = GRAVITY;
				break;
		}
	}
	return player_entity;
}

Entity update(
		Entity& player_entity, 
		std::vector<bool>& collisions
		) {
	// Collisions -> left: 0, top: 1, right: 2, bottom: 3

	player_entity.vel.y += player_entity.gravity;
	player_entity.vel.y *= float(!(collisions[1] || collisions[3]));

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

	return player_entity;
}
