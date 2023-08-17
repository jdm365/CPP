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

void detect_collision(
		std::vector<bool>& collisions,
		Entity& src_entity,
		Entity& dst_entity
		) {
	// if (!other_entity.collidable) {
	if (false) {
		return;
	}
	bool local_collisions[4] = {false, false, false, false};
	const float EPS = 1e-3;

	// Detect if collision will happen on next frame.
	Vector2f src_next_pos = src_entity.pos.vector_add(src_entity.vel);
	Vector2f dst_next_pos = dst_entity.pos.vector_add(dst_entity.vel);

	float src_bottom = src_next_pos.y + src_entity.height;
	float src_right  = src_next_pos.x + src_entity.width;

	float dst_bottom = dst_next_pos.y + dst_entity.height;
	float dst_right  = dst_next_pos.x + dst_entity.width;

	// Only detect collisions for objects near src.
	if (!(src_next_pos.x <= dst_right + EPS && src_right >= dst_next_pos.x - EPS
		&& src_next_pos.y <= dst_bottom + EPS && src_bottom >= dst_next_pos.y - EPS)) {
		return;
	}

	// Left
	if (
			(src_entity.vel.x - dst_entity.vel.x < EPS) 
				&&
			(src_next_pos.x - dst_right <= EPS)
			) {
		local_collisions[0] = true;
	}

	// Right
	else if (
			(src_entity.vel.x - dst_entity.vel.x > -EPS) 
				&&
			(src_right - dst_next_pos.x >= -EPS)
			) {
		local_collisions[2] = true;
	}

	// Bottom
	if (
			(src_bottom - dst_next_pos.y >= -EPS) 
				&& 
			(-EPS <= (src_entity.vel.y - dst_entity.vel.y) - (src_bottom - dst_next_pos.y))
			) {
		local_collisions[3] = true; // Bottom collision
	}

	// Top
	else if (
			(src_next_pos.y - dst_bottom <= EPS)
				&& 
			((src_entity.vel.y - dst_entity.vel.y) - (src_next_pos.y - dst_bottom) <= EPS)
			) {
		local_collisions[1] = true; // Top collision
	}

	if (strcmp(src_entity.type, "player") == 0 && strcmp(dst_entity.type, "enemy") == 0) {
		for (int idx = 0; idx < 4; idx++) {
			if (local_collisions[idx]) {
				src_entity.health -= ENEMY_DPF;
				break;
			}
		}
	}

	if (local_collisions[3]) { 
		if (strcmp(src_entity.type, "player") == 0) {
			src_entity.vel.y = dst_entity.vel.y;
			src_entity.pos.y = dst_entity.pos.y - src_entity.height;
			if (std::abs(dst_entity.vel.y) > EPS) {
				src_entity.vel.y += GRAVITY;
			}
		}
		else if (strcmp(src_entity.type, "enemy") == 0 && strcmp(dst_entity.type, "enemy") == 0) {
			src_entity.vel.y = -src_entity.vel.y;
			dst_entity.vel.y = -dst_entity.vel.y;
		}

		collisions[3] = true;
		return;
	}

	collisions.assign(local_collisions, local_collisions + 4);
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
						-PLAYER_SPEED * player_entity.speed_multiplier, 
						player_entity.vel.y
						);
				player_entity.facing_right = false;
				break;
			case SDLK_d:
				player_entity.vel = Vector2f(
						PLAYER_SPEED * player_entity.speed_multiplier, 
						player_entity.vel.y
						);
				player_entity.facing_right = true;
				break;
			case SDLK_w:
				if (collisions[3]) {
					player_entity.vel.y -= JUMP_SPEED;
				}
				break;

			case SDLK_SPACE:
				if (player_entity.speed_multiplier == 2.0f) {
					break;
				}
				player_entity.speed_multiplier = 2.0f;
				player_entity.vel.x *= player_entity.speed_multiplier;
				break;
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
				if (player_entity.speed_multiplier == 1.0f) {
					break;
				}
				player_entity.speed_multiplier = 1.0f;
				player_entity.vel.x *= 0.5f;
				break;
		}
	}
}

void update(
		Entity& player_entity, 
		std::vector<Entity>& enemy_entities,
		std::vector<bool>& collisions
		) {
	// Collisions -> left: 0, top: 1, right: 2, bottom: 3
	
	// Update enemies
	for (auto& enemy_entity: enemy_entities) {
		float pos_diff = std::abs(player_entity.pos.x - enemy_entity.pos.x);
		if (pos_diff < GROUND_SIZE * 8 && enemy_entity.pos.y == WINDOW_HEIGHT) {
			enemy_entity.vel.y -= 1.75f * JUMP_SPEED;
		}
		else {
			enemy_entity.vel.y += GRAVITY;
		}

		if (enemy_entity.pos.y > WINDOW_HEIGHT) {
			enemy_entity.pos.y = WINDOW_HEIGHT;
			enemy_entity.vel.y = 0.00f;
		}
		else {
			enemy_entity.pos.y += enemy_entity.vel.y;
		}
	}

	if (collisions[0]) {
		player_entity.vel.x = 0.00f;
	}
	if (collisions[2]) {
		player_entity.vel.x = 0.00f;
	}

	if (!collisions[3]) {
		player_entity.vel.y += GRAVITY;
	}

	player_entity.pos.x += player_entity.vel.x;
	player_entity.pos.y += player_entity.vel.y;


	// Respawn if player falls off map.
	if (player_entity.pos.y > WINDOW_HEIGHT) {
		respawn(player_entity);
	}

}

void respawn(Entity& player_entity) {
	player_entity.alive  = true;
	player_entity.health = 100;

	player_entity.pos.x = 100.00f;
	player_entity.pos.y = PLATFORM_HEIGHT - 250.0f;
	player_entity.vel.x = 0.00f;
	player_entity.vel.y = 0.00f;
}

