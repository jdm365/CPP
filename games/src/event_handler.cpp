#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>
#include <algorithm>

#include "entity.hpp"
#include "event_handler.hpp"
#include "constants.h"

std::vector<bool> detect_collisions(
		Entity player_entity, 
		std::vector<std::vector<int>> collidable_entity_positions
		) {
	int left_edge_p   = int(player_entity.pos.x);
	int top_edge_p 	  = int(player_entity.pos.y);
	int right_edge_p  = int(player_entity.pos.x) + player_entity.width;
	int bottom_edge_p = int(player_entity.pos.y) + player_entity.height;

	bool left_collision   = false;
	bool top_collision    = false;
	bool right_collision  = false;
	bool bottom_collision = false;

	for (int idx = 0; idx < int(collidable_entity_positions.size()); idx++) {
		int left_edge_obj   = collidable_entity_positions[idx][0];
		int top_edge_obj 	= collidable_entity_positions[idx][1];
		int right_edge_obj  = collidable_entity_positions[idx][2];
		int bottom_edge_obj = collidable_entity_positions[idx][3];

		bool cond_0;
		bool cond_1;
		bool cond_2;

		// Left collision
		if (!left_collision) {
			cond_0 = (left_edge_p <= right_edge_obj);
			cond_1 = (top_edge_obj < bottom_edge_p) && (bottom_edge_obj > top_edge_p);
			cond_2 = (right_edge_p > left_edge_obj);
			if (cond_0 && cond_1 && cond_2) {
				left_collision = true;
			}
		}

		// Top collision
		if (!top_collision) {
			cond_0 = (top_edge_p <= bottom_edge_obj);
			cond_1 = (right_edge_obj > left_edge_p) && (left_edge_obj < right_edge_p);
			cond_2 = (top_edge_obj < top_edge_p);
			if (cond_0 && cond_1 && cond_2) {
				top_collision = true;
			}
		}

		// Right collision
		if (!right_collision) {
			cond_0 = (right_edge_p >= left_edge_obj);
			cond_1 = (top_edge_obj < bottom_edge_p) && (bottom_edge_obj > top_edge_p);
			cond_2 = (left_edge_p < right_edge_obj);
			if (cond_0 && cond_1 && cond_2) {
				right_collision = true;
			}
		}

		// Bottom collision
		if (!bottom_collision) {
			cond_0 = (bottom_edge_p >= top_edge_obj);
			cond_1 = (right_edge_obj > left_edge_p) && (left_edge_obj < right_edge_p);
			cond_2 = (bottom_edge_obj > bottom_edge_p);
			if (cond_0 && cond_1 && cond_2) {
				bottom_collision = true;
			}
		}
	}
	std::vector<bool> collisions = {left_collision, top_collision, right_collision, bottom_collision};
	return collisions;
}

Entity handle(
		SDL_Event event, 
		Entity player_entity, 
		std::vector<std::vector<int>> collidable_entity_positions
		) {
	std::vector<bool> collisions = detect_collisions(player_entity, collidable_entity_positions); 
	bool left_collision   = collisions[0];
	bool top_collision    = collisions[1];
	bool right_collision  = collisions[2];
	bool bottom_collision = collisions[3];

	// Update player velocity.
	if (event.type == SDL_KEYDOWN) {
		switch(event.key.keysym.sym) {
			case SDLK_LEFT:
				player_entity.vel = Vector2f(
						-PLAYER_SPEED * float(!left_collision), 
						player_entity.vel.y * float(!top_collision) * float(!bottom_collision)
						);
				break;
			case SDLK_RIGHT:
				player_entity.vel = Vector2f(
						PLAYER_SPEED * float(!right_collision), 
						player_entity.vel.y * float(!top_collision) * float(!bottom_collision)
						);
				break;
			case SDLK_UP:
				if ((player_entity.vel.y == 0) && bottom_collision) {
					player_entity.vel = Vector2f(
							player_entity.vel.x * float(!left_collision) * float(!right_collision),
							-JUMP_SPEED * float(!top_collision)
							);
				}
				break;
			case SDLK_SPACE:
				player_entity.vel = Vector2f(
						player_entity.vel.x * float(!left_collision) * float(!right_collision),
						(player_entity.vel.y - ROCKET_POWER) * float(!top_collision)
						);
				break;
 		}
	}
	if (event.type == SDL_KEYUP) {
		switch(event.key.keysym.sym) {
			case SDLK_LEFT:
				player_entity.vel = Vector2f(
						0, 
						player_entity.vel.y * float(!top_collision) * float(!bottom_collision)
						);
				break;
			case SDLK_RIGHT:
				player_entity.vel = Vector2f(
						0, 
						player_entity.vel.y * float(!top_collision) * float(!bottom_collision)
						);
				break;
		}
	}
	return player_entity;
}

Entity update(
		Entity player_entity, 
		std::vector<std::vector<int>> collidable_entity_positions
		) {
	std::vector<bool> collisions = detect_collisions(player_entity, collidable_entity_positions); 

	bool left_collision   = collisions[0];
	bool top_collision    = collisions[1];
	bool right_collision  = collisions[2];
	bool bottom_collision = collisions[3];

	bool cond_0 = !(bottom_collision && (player_entity.vel.y >= 0));
	bool cond_1 = !(top_collision    && (player_entity.vel.y <= 0));

	float x_vel = player_entity.vel.x;
	float y_vel = (player_entity.vel.y + GRAVITY) * float(cond_0) * float(cond_1);

	float corrective_factor_x = 0.00f;
	if (left_collision || right_collision) {
		float sign = -float(left_collision) + float(right_collision) + float(left_collision && right_collision);
		corrective_factor_x = sign * float(int(player_entity.pos.x) % GROUND_SIZE);
	}

	float corrective_factor_y = 0.00f;
	if ((y_vel == 0) && bottom_collision && ((int(player_entity.pos.y) % GROUND_SIZE) != 0)) {
		corrective_factor_y = float(int(player_entity.pos.y) % GROUND_SIZE);
	}

	float x_pos = int(abs(player_entity.pos.x + x_vel - corrective_factor_x));
	float y_pos = player_entity.pos.y + y_vel - corrective_factor_y;

	// Respawn if player falls off map.
	if (y_pos > WINDOW_HEIGHT) {
		x_pos = 100.00f;
		y_pos = float(PLATFORM_HEIGHT - 250);
		x_vel = 0.00f;
		y_vel = 0.00f;
	}

	player_entity.pos = Vector2f(x_pos, y_pos);
	player_entity.vel = Vector2f(x_vel, y_vel);

	return player_entity;
	}
