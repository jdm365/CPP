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

	bool vert_cond;

	for (Entity& entity: collidable_entities) {
		// Only detect collisions for objects near player.
		if (player_entity.pos.x - entity.pos.x > entity.width) {
			continue;
		}
		if (player_entity.pos.x + (player_entity.width / 2) < entity.pos.x) {
			continue;
		}
		if (player_entity.pos.y - entity.pos.y > entity.height) {
			continue;
		}
		if (player_entity.pos.y + player_entity.height < entity.pos.y) {
			continue;
		}
		

		// Top 
		if ((player_entity.vel.y < 0.00f) && (entity.pos.y > player_entity.pos.y)) {
			collisions[1] = true;
		}
		// Bottom 
		if (player_entity.vel.y >= 0.00f) {
			collisions[3] = true;
		}

		vert_cond = (player_entity.pos.y + player_entity.height > entity.pos.y);
		if (!vert_cond) {
			continue;
		}
		// Left
		if (player_entity.vel.x < 0.00f){
			collisions[0] = true;
		}
		// Right 
		if (player_entity.vel.x > 0.00f) {
			collisions[2] = true;
		}
	}
}

void handle(SDL_Event& event, Entity& player_entity, std::vector<bool>& collisions) {
	// Collisions -> horizontal: 0, vertical: 1
	// Update player velocity.
	if (event.type == SDL_KEYDOWN) {
		switch(event.key.keysym.sym) {
			case SDLK_LEFT:
				player_entity.vel = Vector2f(
						-PLAYER_SPEED, 
						player_entity.vel.y
						);
				break;
			case SDLK_RIGHT:
				player_entity.vel = Vector2f(
						PLAYER_SPEED, 
						player_entity.vel.y
						);
				break;
			case SDLK_UP:
				if (player_entity.vel.y == 0) {
					player_entity.vel = Vector2f(
							player_entity.vel.x,
							-JUMP_SPEED * collisions[3]
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
						0.00f,
						player_entity.vel.y
						);
				break;
			case SDLK_RIGHT:
				player_entity.vel = Vector2f(
						0.00f,
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
	// Collisions -> left: 0, top: 1, right: 2, bottom: 3
	if (collisions[0]) {
		player_entity.vel.x = 0.00f;
		player_entity.pos.x += float(int(player_entity.pos.x) % GROUND_SIZE);
	}
	if (collisions[2]) {
		player_entity.vel.x = 0.00f;
		player_entity.pos.x -= float(int(player_entity.pos.x) % GROUND_SIZE);
	}

	player_entity.vel.y += player_entity.gravity;
	// Fix for top.
	if (collisions[3]) {
		player_entity.vel.y = 0.00f;
		// Corrective factor to avoid getting stuck in walls.
		// Very inelegant. Look to fix.
		player_entity.pos.y -= float(int(player_entity.pos.y) % GROUND_SIZE);
	}

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
