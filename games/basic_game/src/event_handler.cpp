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
	if (!src_entity.collidable || !dst_entity.collidable) {
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


	// Bottom
	if (
			(EPS >= (src_bottom - dst_next_pos.y) - (src_entity.vel.y - dst_entity.vel.y))
				&& 
			(-EPS <= src_bottom - dst_next_pos.y)
		) {
		local_collisions[3] = true; // Bottom collision
	}

	// Top
	else if (
			((src_entity.vel.y - dst_entity.vel.y) - (src_next_pos.y - dst_bottom) <= EPS)
				&& 
			(src_next_pos.y - dst_bottom <= EPS)
		) {
		local_collisions[1] = true; // Top collision
	}

	// Left
	else if (
			((src_entity.vel.x - dst_entity.vel.x) - (src_next_pos.x - dst_right) <= EPS)
				&&
			((src_next_pos.x - dst_right) <= EPS)
		) {
		local_collisions[0] = true;
	}

	// Right
	else if (
			((src_entity.vel.x - dst_entity.vel.x) - (src_right - dst_next_pos.x) >= -EPS)
				&&
			(src_right - dst_next_pos.x >= -EPS)
		) {
		local_collisions[2] = true;
	}

	if (strcmp(src_entity.type, "player") == 0) {
		if (!local_collisions[0] && !local_collisions[1] && !local_collisions[2] && !local_collisions[3]) {
			std::cout << "No collision detected" << std::endl;
			std::cout << (src_entity.vel.y - dst_entity.vel.y) - (src_bottom - dst_next_pos.y) << std::endl;
			std::cout << src_entity.vel.y << " " << dst_entity.vel.y << std::endl;
			std::cout << src_entity.bottom_collision << std::endl;
			std::exit(1);
			return;
		}
	}

	if (strcmp(src_entity.type, "player") == 0 && strcmp(dst_entity.type, "enemy") == 0) {
		for (int idx = 0; idx < 4; ++idx) {
			if (local_collisions[idx]) {
				src_entity.health -= ENEMY_DPF;
				break;
			}
		}
	}

	// if (local_collisions[3]) { 
	if (strcmp(src_entity.type, "player") == 0 && strcmp(dst_entity.type, "enemy") == 0) {
		/*
		if (local_collisions[0]) {
			src_entity.vel.x = dst_entity.vel.x;
			src_entity.pos.x = dst_next_pos.x - src_entity.width;
		}
		else if (local_collisions[2]) {
			src_entity.vel.x = dst_entity.vel.x;
			src_entity.pos.x = dst_next_pos.x + dst_entity.width;
		}
		else if (local_collisions[3]) {
			src_entity.vel.y = dst_entity.vel.y;
			src_entity.pos.y = dst_entity.pos.y - src_entity.height;
		}

		if (std::abs(dst_entity.vel.y) > EPS) {
			src_entity.vel.y += GRAVITY;
		}
		*/
		src_entity.vel.y = dst_entity.vel.y;
		if (local_collisions[3]) {
			src_entity.pos.y = dst_entity.pos.y - src_entity.height;
			src_entity.vel.x = dst_entity.vel.x;
			src_entity.vel.y = dst_entity.vel.y + GRAVITY * (dst_entity.bottom_collision ? 0 : 1);
		}
		else if (local_collisions[0]) {
			src_entity.pos.x = dst_entity.pos.x + dst_entity.width;
			src_entity.vel.y -= GRAVITY;
			src_entity.vel.x = dst_entity.vel.x;
		}
		else if (local_collisions[2]) {
			src_entity.pos.x = dst_entity.pos.x - src_entity.width - 1;
			src_entity.vel.y -= GRAVITY;
			src_entity.vel.x = dst_entity.vel.x;
		}
	}
	else if (strcmp(src_entity.type, "player") == 0 && strcmp(dst_entity.type, "ground") == 0) [[likely]] {
		src_entity.vel.y = dst_entity.vel.y;
		if (local_collisions[3]) {
			src_entity.pos.y = dst_entity.pos.y - src_entity.height;
		}
		else if (local_collisions[0]) {
			src_entity.pos.x = dst_entity.pos.x + dst_entity.width;
			src_entity.vel.y -= GRAVITY;
			src_entity.vel.x = dst_entity.vel.x;
		}
		else if (local_collisions[2]) {
			src_entity.pos.x = dst_entity.pos.x - src_entity.width - 1;
			src_entity.vel.y -= GRAVITY;
			src_entity.vel.x = dst_entity.vel.x;
		}
	}
	else if (strcmp(src_entity.type, "enemy") == 0 && strcmp(dst_entity.type, "enemy") == 0) {
		Vector2f src_vel = src_entity.vel;
		src_entity.vel = dst_entity.vel;
		dst_entity.vel = src_vel;
	}
	else if (strcmp(src_entity.type, "enemy") == 0 && strcmp(dst_entity.type, "ground") == 0) {
		/*
		src_entity.pos.y = dst_entity.pos.y - src_entity.height;
		src_entity.vel.y = dst_entity.vel.y;
		*/
		src_entity.vel.y = dst_entity.vel.y;
		if (local_collisions[3]) {
			src_entity.pos.y = dst_entity.pos.y - src_entity.height;
		}
		else if (local_collisions[0]) {
			src_entity.pos.x = dst_entity.pos.x + dst_entity.width;
			src_entity.vel.y -= GRAVITY;
			src_entity.vel.x = -src_entity.vel.x;
		}
		else if (local_collisions[2]) {
			src_entity.pos.x = dst_entity.pos.x - src_entity.width - 1;
			src_entity.vel.x = -src_entity.vel.x;
			src_entity.vel.y -= GRAVITY;
		}
	}

	if (local_collisions[3]) {
		collisions[3] = true;
		return;
	}

	/*
	if (local_collisions[0] || local_collisions[2]) {
		if (strcmp(src_entity.type, "enemy") == 0 && strcmp(dst_entity.type, "ground") == 0) {
			src_entity.vel.x = -src_entity.vel.x;
			src_entity.vel.y -= GRAVITY;
		}
		else if (strcmp(src_entity.type, "enemy") == 0 && strcmp(dst_entity.type, "enemy") == 0) {
			Vector2f src_vel = src_entity.vel;
			src_entity.vel = dst_entity.vel;
			dst_entity.vel = src_vel;
		}
	}
	*/

	collisions.assign(local_collisions, local_collisions + 4);
}

void handle(SDL_Event& event, Entity& player_entity) {
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
				if (player_entity.bottom_collision) {
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
		Entity& entity, 
		Vector2f& player_pos,
		std::vector<bool>& collisions
		) {
	// Collisions -> left: 0, top: 1, right: 2, bottom: 3
	
	if (strcmp(entity.type, "enemy") == 0) {
		float pos_diff = std::abs(player_pos.x - entity.pos.x);
		if (pos_diff < GROUND_SIZE * 8 && entity.pos.y == WINDOW_HEIGHT) {
			// Flying enemy logic
			entity.vel.y -= 1.5f * JUMP_SPEED;
		}
		else {
			// Walking enemy logic
			if (!collisions[3]) {
				entity.vel.y += GRAVITY;

				if (entity.bottom_collision && !(collisions[0] || collisions[2])) {
					entity.vel.x = -entity.vel.x;
					entity.vel.y -= GRAVITY;
				}
				entity.bottom_collision = false;
			}
			else {
				entity.bottom_collision = true;
			}
		}

		if (entity.pos.y > WINDOW_HEIGHT) {
			entity.pos.y = WINDOW_HEIGHT;
			entity.vel.y = 0.00f;
		}
		else {
			/*
			if (collisions[0] || collisions[2]) {
				entity.vel.x = -entity.vel.x;
			}

			if (collisions[3]) {
				entity.pos.x += entity.vel.x;
			}
			*/
			entity.pos.x += entity.vel.x;
			entity.pos.y += entity.vel.y;
		}
	}
	else if (strcmp(entity.type, "player") == 0) {
		/*
		if (collisions[0] || collisions[2]) {
			entity.vel.x = 0.00f;
		}
		*/

		if (!collisions[3]) {
			entity.vel.y += GRAVITY;
			entity.bottom_collision = false;
		}
		else {
			entity.bottom_collision = true;
		}

		entity.pos.x += entity.vel.x;
		entity.pos.y += entity.vel.y;

		// Respawn if player falls off map.
		if (entity.pos.y > WINDOW_HEIGHT) {
			respawn(entity);
		}
	}

}

void respawn(Entity& player_entity) {
	player_entity.alive  = true;
	player_entity.health = 100;

	player_entity.pos = player_entity.respawn_pos;
	player_entity.vel.x = 0.00f;
	player_entity.vel.y = 0.00f;
}

