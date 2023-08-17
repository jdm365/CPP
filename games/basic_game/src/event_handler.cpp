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
		Entity& player_entity,
		Entity& other_entity
		) {
	// if (!other_entity.collidable) {
	if (false) {
		return;
	}
	bool local_collisions[4] = {false, false, false, false};

	// Detect if collision will happen on next frame.
	Vector2f player_next_pos = player_entity.pos.vector_add(player_entity.vel);
	Vector2f other_next_pos = other_entity.pos.vector_add(other_entity.vel);

	float player_bottom = player_next_pos.y + player_entity.height;
	float player_right  = player_next_pos.x + player_entity.width;

	float other_bottom = other_next_pos.y + other_entity.height;
	float other_right  = other_next_pos.x + other_entity.width;

	// Only detect collisions for objects near player.
	if (!(player_next_pos.x <= other_right && player_right >= other_next_pos.x
		&& player_next_pos.y <= other_bottom && player_bottom >= other_next_pos.y)) {
		return;
	}
	// Left
	if (
			player_entity.vel.x < 0.00f 
				&& 
			player_next_pos.x < other_right 
			) {
		local_collisions[0] = true;
	}

	// Right
	else if (
			player_entity.vel.x > 0.00f 
				&& 
			player_right > other_next_pos.x
			) {
		local_collisions[2] = true;
	}

	// Bottom
	if (
			(player_bottom >= other_next_pos.y) 
				&& 
			((player_bottom - other_next_pos.y) <= (player_entity.vel.y - other_entity.vel.y))
			) {
		local_collisions[3] = true; // Bottom collision
	}
	// Top
	else if (
			(player_next_pos.y <= other_bottom) 
				&& 
			((other_bottom - player_next_pos.y) <= (other_entity.vel.y - player_entity.vel.y))
			) {
		local_collisions[1] = true; // Top collision
	}


	if (local_collisions[3]) { 
		player_entity.vel.y = other_entity.vel.y;
		player_entity.pos.y = other_entity.pos.y - player_entity.height;
		collisions[3] = true;
		if (strcmp(other_entity.type, "enemy") == 0) {
			std::cout << "Bottom collision: " << rand() % 9 << std::endl;
			player_entity.vel.y += GRAVITY;
		}
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
				break;
			case SDLK_d:
				player_entity.vel = Vector2f(
						PLAYER_SPEED * player_entity.speed_multiplier, 
						player_entity.vel.y
						);
				break;
			case SDLK_w:
				if (collisions[3]) {
					std::cout << "Jumping" << std::endl;
					player_entity.vel = Vector2f(
							player_entity.vel.x,
							-JUMP_SPEED
							);
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
		if (pos_diff < GROUND_SIZE * 8 && enemy_entity.vel.y == 0.00f && enemy_entity.pos.y == WINDOW_HEIGHT) {
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


	/*
	float next_y = player_entity.pos.y + player_entity.vel.y
					- (int)(player_entity.pos.y + player_entity.vel.y) % GROUND_SIZE;

	if (collisions[3]) {
		player_entity.vel.y = 0.00f;
		std::cout << "Current y: " << player_entity.pos.y << std::endl;
		std::cout << "Next y: " << next_y << std::endl << std::endl;
		player_entity.pos.y = next_y;
	}
	else {
		player_entity.vel.y += player_entity.gravity;
	}
	*/

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
