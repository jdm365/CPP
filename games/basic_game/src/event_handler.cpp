#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/entity.hpp"
#include "../include/event_handler.hpp"
#include "../include/constants.h"
#include "../include/math.hpp"

void detect_collisions(
		std::vector<bool>& collisions,
		Entity& player_entity,
		std::vector<Entity>& collidable_entities,
		int scroll_factor_x
		) {
	// Need fuzzy_factor to allow for between frame gaps in space.
	int fuzzy_factor = std::max((int)std::abs(player_entity.vel.y), (int)GRAVITY * 2);

	// Reset collisions to false
	collisions[0] = false;
	collisions[1] = false;
	collisions[2] = false;
	collisions[3] = false;

	// Detect if collision will happen on next frame.
	Vector2f next_pos = player_entity.pos.vector_add(player_entity.vel);

	for (Entity& entity: collidable_entities) {
		if (!entity.collidable) {
			continue;
		}

		std::vector<bool> local_collisions(4, false);

		float player_bottom = next_pos.y + player_entity.height;
		float player_right  = next_pos.x + player_entity.width;
		float entity_bottom = entity.pos.y + entity.height;
		float entity_right  = entity.pos.x + entity.width;

		// Only detect collisions for objects near player.
		if (!(next_pos.x < entity_right && player_right > entity.pos.x 
      		&& next_pos.y < entity_bottom && player_bottom > entity.pos.y)) {
			continue;
		}

		// Top
		if (player_entity.vel.y < 0.00f && next_pos.y < entity.pos.y) {
			local_collisions[1] = true; // Top collision
		}
		// Bottom
		else if (player_entity.vel.y >= 0.00f && player_bottom > entity.pos.y) {
			local_collisions[3] = true; // Bottom collision
		}

		// Left
		if (
				player_entity.vel.x < 0.00f 
					&& 
				next_pos.x < entity_right 
					&& 
				std::abs((player_entity.pos.y + player_entity.height) - entity.pos.y) > fuzzy_factor
				) {
			local_collisions[0] = true;
		}
		// Right
		else if (
				player_entity.vel.x > 0.00f 
					&& 
				player_right > entity.pos.x 
					&& 
				std::abs((player_entity.pos.y + player_entity.height) - entity.pos.y) > fuzzy_factor
				) {
			local_collisions[2] = true;
		}

		collisions[0] = collisions[0] || local_collisions[0];
		collisions[1] = collisions[1] || local_collisions[1];
		collisions[2] = collisions[2] || local_collisions[2];
		collisions[3] = collisions[3] || local_collisions[3];	
	}
}

void handle(SDL_Event& event, Entity& player_entity, std::vector<bool>& collisions) {
	// Collisions -> horizontal: 0, vertical: 1
	// Update player velocity.
	if (event.type == SDL_KEYDOWN) {
		switch(event.key.keysym.sym) {
			case SDLK_ESCAPE:
				exit(0);
				break;

			case SDLK_a:
				player_entity.vel = Vector2f(
						-PLAYER_SPEED, 
						player_entity.vel.y
						);
				break;
			case SDLK_d:
				player_entity.vel = Vector2f(
						PLAYER_SPEED, 
						player_entity.vel.y
						);
				break;
			case SDLK_w:
				if (collisions[3]) {
					player_entity.vel = Vector2f(
							player_entity.vel.x,
							-JUMP_SPEED
							);
				}
				break;

			case SDLK_SPACE:
				if (std::abs(player_entity.vel.x) <= std::abs(PLAYER_SPEED)) {
					player_entity.vel.x = 2.0f * player_entity.vel.x;
					// break;
				}
 		}
	}

	else if (event.type == SDL_KEYUP) {
		switch(event.key.keysym.sym) {
			case SDLK_a:
				player_entity.vel = Vector2f(
						0.00f,
						player_entity.vel.y
						);
				break;
			case SDLK_d:
				player_entity.vel = Vector2f(
						0.00f,
						player_entity.vel.y
						);
				break;
			case SDLK_SPACE:
				player_entity.vel.x = player_entity.vel.x / 2.0f;
				break;
		}
	}
}

void update(Entity& player_entity, std::vector<bool>& collisions) {
	// Collisions -> left: 0, top: 1, right: 2, bottom: 3
	if (collisions[0]) {
		player_entity.vel.x = 0.00f;
	}
	if (collisions[2]) {
		player_entity.vel.x = 0.00f;
	}

	float next_y = player_entity.pos.y + player_entity.vel.y - (int)(player_entity.pos.y + player_entity.vel.y) % GROUND_SIZE;

	// Fix for top.
	if (collisions[3]) {
		player_entity.vel.y = 0.00f;
		player_entity.pos.y = next_y;
	}
	else {
		player_entity.vel.y += player_entity.gravity;
	}

	// Cap velocity
	if (player_entity.vel.y > 0) {
		player_entity.vel.y = std::min(player_entity.vel.y, 2.0f * (float)PLAYER_SPEED);
	}
	else {
		player_entity.vel.y = std::max(player_entity.vel.y, -2.0f * (float)PLAYER_SPEED);
	}

	player_entity.pos.x += player_entity.vel.x;
	player_entity.pos.y += player_entity.vel.y;

	// Respawn if player falls off map.
	if (player_entity.pos.y > WINDOW_HEIGHT) {
		player_entity.pos.x = 100.00f;
		player_entity.pos.y = PLATFORM_HEIGHT - 250.0f;
		player_entity.vel.x = 0.00f;
		player_entity.vel.y = 0.00f;
	}
}
