#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <omp.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/event_handler.hpp"
#include "../include/entity.hpp"
#include "../include/constants.h"
#include "../include/math.hpp"


void detect_collisions(
		Entities& entities, 
		Vector2i& scroll_factors
		) {
	// Player -> Enemies_0, Enemies_1, ...
	// Player -> Platforms_0, Platforms_1, ...
	// Enemies_0 -> Enemies_1, Enemies_2, ...
	// Enemies_0 -> Platforms_0, Platforms_1, ...
	
	// PLAYER
	std::vector<Entity*> non_player_entities = entities.get_non_player_entities();

	#pragma omp parallel for schedule(static)
	for (Entity* entity: non_player_entities) {
		if (!entity->alive || entity->entity_type == PROJECTILE) {
			continue;
		}
		_detect_collision(entities.player_entity, *entity);
	}
	update(entities, entities.player_entity, scroll_factors);

	// ENEMIES
	#pragma omp parallel for schedule(static)
	for (int idx = 0; idx < (int)non_player_entities.size(); ++idx) {
		if (
				non_player_entities[idx]->collision_type != DYNAMIC 
					|| 
				!non_player_entities[idx]->alive
			) {
			continue;
		}

		non_player_entities[idx]->overhang = true;
		for (int jdx = 0; jdx < (int)non_player_entities.size(); ++jdx) {
			if (!non_player_entities[jdx]->alive) {
				continue;
			}
			_detect_collision(
					*non_player_entities[idx],
					*non_player_entities[jdx]
					);
		}
		update(entities, *non_player_entities[idx], scroll_factors);
	}
}


void _detect_collision(
		Entity& src_entity,
		Entity& dst_entity
		) {
	if (!src_entity.is_collidable() || !dst_entity.is_collidable()) {
		return;
	}
	bool local_collisions[4] = {false, false, false, false};
	const float EPS = 1e-3;

	float src_bottom_last = src_entity.pos.y + src_entity.height;
	float src_right_last  = src_entity.pos.x + src_entity.width;

	float dst_bottom_last = dst_entity.pos.y + dst_entity.height;
	float dst_right_last  = dst_entity.pos.x + dst_entity.width;

	// Detect if collision will happen on next frame.
	Vector2f src_next_pos = vector_add(src_entity.pos, src_entity.vel);
	Vector2f dst_next_pos = vector_add(dst_entity.pos, dst_entity.vel);

	float src_bottom = src_next_pos.y + src_entity.height;
	float src_right  = src_next_pos.x + src_entity.width;

	float dst_bottom = dst_next_pos.y + dst_entity.height;
	float dst_right  = dst_next_pos.x + dst_entity.width;

	// Only detect collisions for objects near src.
	if (!(src_next_pos.x < dst_right + EPS && src_right > dst_next_pos.x - EPS
		&& src_next_pos.y < dst_bottom + EPS && src_bottom > dst_next_pos.y - EPS)) {
		return;
	}


	// Collision Detection
	// Two conditions
	// 1. If src edge is past dst edge in next frame.
	// 2. If src edge is not past dst edge in current frame.
	
	// Bottom
	if (
			(EPS > src_bottom_last - dst_entity.pos.y)
				&& 
			(-EPS < src_bottom - dst_next_pos.y)
		) [[likely]] {
		local_collisions[3] = true; // Bottom collision
	}

	// Top
	else if (
			(src_entity.pos.y - dst_bottom_last > -EPS)
				&& 
			(src_next_pos.y - dst_bottom < EPS)
		) {
		local_collisions[1] = true; // Top collision
	}

	// Left
	else if (
			(src_entity.pos.x - dst_right_last > -EPS)
				&&
			(src_next_pos.x - dst_right < EPS)
		) {
		local_collisions[0] = true;
	}

	// Right
	else if (
			(src_right_last - dst_entity.pos.x < EPS)
				&&
			(src_right - dst_next_pos.x > -EPS)
		) {
		local_collisions[2] = true;
	}

	switch(src_entity.entity_type) {
		case PLAYER:
			if (
					dst_entity.entity_type == ENEMY_WALKING
						|| 
					dst_entity.entity_type == ENEMY_FLYING
				) {
				dst_entity.distance_from_player = Vector2f {
						std::abs(src_entity.pos.x - dst_entity.pos.x),
						std::abs(src_entity.pos.y - dst_entity.pos.y)
				};

				if (local_collisions[0] || local_collisions[2]) {
					src_entity.health -= ENEMY_DPF;
				}
				else if (local_collisions[1] || local_collisions[3]) {
					dst_entity.health -= 100;
					src_entity.vel.y = dst_entity.vel.y - JUMP_SPEED;
					dst_entity.alive = false;
					return;
				}
			}

			if (dst_entity.entity_type == AMMO) {
				src_entity.reload = true;
				dst_entity.alive = false;
				return;
			}

			if (local_collisions[3] && !src_entity.collisions[3]) {
				src_entity.pos.y = dst_entity.pos.y - src_entity.height;
			}
			else if (local_collisions[0]) {
				src_entity.pos.x = dst_entity.pos.x + dst_entity.width + 1;
				src_entity.vel.x = dst_entity.vel.x;
			}
			else if (local_collisions[2]) {
				src_entity.pos.x = dst_entity.pos.x - src_entity.width - 1;
				src_entity.vel.x = dst_entity.vel.x;
			}
			break;

		case ENEMY_WALKING:
			if (dst_entity.collision_type == STATIC) {
				src_entity.vel.y = dst_entity.vel.y;

				if (local_collisions[3]) {
					src_entity.pos.y = dst_entity.pos.y - src_entity.height;
					if (
							(src_entity.vel.x < EPS)
								&&
							((src_next_pos.x >= dst_next_pos.x) && (dst_right > src_next_pos.x))
						) {
						src_entity.overhang = false;
					}
					if (
							(src_entity.vel.x > -EPS)
								&&
							((src_right < dst_right) && (src_right >= dst_next_pos.x))
						) {
						src_entity.overhang = false;
					}
				}

				else if (local_collisions[0]) {
					src_entity.pos.x = dst_entity.pos.x + dst_entity.width + 1;
					src_entity.vel.x = -src_entity.vel.x;
					src_entity.vel.y -= GRAVITY;
					src_entity.overhang = false;
				}
				else if (local_collisions[2]) {
					src_entity.pos.x = dst_entity.pos.x - src_entity.width - 1;
					src_entity.vel.x = -src_entity.vel.x;
					src_entity.vel.y -= GRAVITY;
					src_entity.overhang = false;
				}
			}
			else {
				if (dst_entity.entity_type == PROJECTILE) {
					src_entity.health -= 10;

					src_entity.vel.y -= GRAVITY;

					dst_entity.alive = false;
				}
				else {
					Vector2f src_vel = src_entity.vel;
					src_entity.vel = dst_entity.vel;
					dst_entity.vel = src_vel;
				}
			}
			break;

		case ENEMY_FLYING:
			if (dst_entity.entity_type == PROJECTILE) {
				src_entity.health -= 10;

				src_entity.vel.y -= GRAVITY;

				dst_entity.alive = false;
			}
			else {
				Vector2f src_vel = src_entity.vel;
				src_entity.vel = dst_entity.vel;
				dst_entity.vel = src_vel;
			}
			break;

		case PROJECTILE:
			if (
					dst_entity.entity_type == ENEMY_WALKING 
						|| 
					dst_entity.entity_type == ENEMY_FLYING
				) {
				dst_entity.health -= 10;
				dst_entity.vel.x -= dst_entity.vel.x;

				// Stop rendering projectile by making it not alive.
				src_entity.alive = false;
			}
			else if (dst_entity.entity_type == GROUND) {
				src_entity.alive = false;
			}
		}

	src_entity.collisions[0] |= local_collisions[0];
	src_entity.collisions[1] |= local_collisions[1];
	src_entity.collisions[2] |= local_collisions[2];
	src_entity.collisions[3] |= local_collisions[3];
}

void handle_keyboard(
		const uint8_t* keyboard_state,
		Entities& entity_manager,
		Vector2i& scroll_factors,
		Weapon& weapon
		) {
	
	if (keyboard_state[SDL_SCANCODE_ESCAPE]) {
		exit(0);
	}

	if (keyboard_state[SDL_SCANCODE_A]) {
		entity_manager.player_entity.vel = Vector2f {
				-PLAYER_SPEED, 
				entity_manager.player_entity.vel.y
		};
		entity_manager.player_entity.facing_right = false;
	}
	if (keyboard_state[SDL_SCANCODE_D]) {
		entity_manager.player_entity.vel = Vector2f {
				PLAYER_SPEED, 
				entity_manager.player_entity.vel.y
		};
		entity_manager.player_entity.facing_right = true;
	}
	// If key is released, stop moving.
	if ((keyboard_state[SDL_SCANCODE_A] == 0) && (keyboard_state[SDL_SCANCODE_D] == 0)) {
		entity_manager.player_entity.vel = Vector2f {
				0, 
				entity_manager.player_entity.vel.y
		};
	}
	if (keyboard_state[SDL_SCANCODE_SPACE]) {
		entity_manager.player_entity.vel.x *= 2.0f;
	}



	if (keyboard_state[SDL_SCANCODE_W]) {
		if (entity_manager.player_entity.on_ground) {
			entity_manager.player_entity.vel.y -= JUMP_SPEED;
		}
	}
	if (keyboard_state[SDL_SCANCODE_S]) {
		;
	}

	int mouse_x, mouse_y;
	uint32_t mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);

	
	if (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		// Fire bullet.
		mouse_x += scroll_factors.x;
		mouse_y -= scroll_factors.y;

		weapon.fire(
				entity_manager.weapon_entities[0],
				entity_manager.projectile_entities,
				mouse_x, 
				mouse_y
				);
	}
}


void update(
		Entities& entity_manager,
		Entity& entity, 
		Vector2i& scroll_factors
		) {
	// Collisions -> left: 0, top: 1, right: 2, bottom: 3
	for (int idx = 0; idx < (int)entity_manager.projectile_entities.size(); ++idx) {
		if (
				!entity_manager.projectile_entities[idx].alive
					||
				entity_manager.projectile_entities[idx].pos.x - scroll_factors.x < 0
					||
				entity_manager.projectile_entities[idx].pos.x - scroll_factors.x > WINDOW_WIDTH
					||
				entity_manager.projectile_entities[idx].pos.y + scroll_factors.y < 0
					||
				entity_manager.projectile_entities[idx].pos.y + scroll_factors.y > WINDOW_HEIGHT
			) {
			// Remove projectile from vector.
			if ((int)entity_manager.projectile_entities.size() > 0) {
				entity_manager.projectile_entities.erase(
						entity_manager.projectile_entities.begin() + idx
				);
			}
			continue;
		}
	}
	if (!entity.alive) return;
	
	switch(entity.entity_type) {
		case ENEMY_FLYING:
			// if (entity.distance_from_player.x < GROUND_SIZE * 8 && entity.pos.y == WINDOW_HEIGHT) {
			if (entity.pos.y == entity.respawn_pos.y) {
				entity.vel.y -= 1.5f * JUMP_SPEED;
			}

			entity.vel.y += GRAVITY;
			entity.pos.y += entity.vel.y;

			if (entity.health <= 0) {
				entity.alive = false;
			}

			// If below screen set at rest and y at WINDOW_HEIGHT.
			if (entity.pos.y > entity.respawn_pos.y) {
				entity.pos.y = entity.respawn_pos.y;
				entity.vel.y = 0.00f;
			}
			break;

		case ENEMY_WALKING:
			if (!entity.collisions[3]) {
				entity.vel.y += GRAVITY;
				entity.on_ground = false;
			}
			else {
				entity.collisions[3] = true;
				if (entity.overhang) {
					entity.vel.x = -entity.vel.x;
				}
			}

			entity.pos.x += entity.vel.x;
			entity.pos.y += entity.vel.y;

			if (entity.health <= 0) {
				entity.alive = false;
			}
			break;

		case PLAYER:
			entity.alive = (entity.health > 0);

			if (entity.collisions[3]) {
				entity.vel.y = 0.0f;
				entity.on_ground = true;
			}
			else {
				entity.vel.y += GRAVITY;
				entity.on_ground = false;
			}

			entity.pos.x += entity.vel.x;
			entity.pos.y += entity.vel.y;

			if (entity.pos.y + scroll_factors.y > WINDOW_HEIGHT) {
				// entity.alive = false;
			}

			// Keep player on screen.
			if (entity.pos.x < 0) {
				entity.pos.x = 0;
			}
			else if (entity.pos.x - scroll_factors.x + entity.width > WINDOW_WIDTH) {
				entity.pos.x = WINDOW_WIDTH + scroll_factors.x - entity.width + 20;
			}

			{
				int mouse_x, mouse_y;
				SDL_GetMouseState(&mouse_x, &mouse_y);

				Vector2f barrel_pos = {
					entity_manager.player_entity.pos.x + entity_manager.player_entity.width - 72,
					entity_manager.player_entity.pos.y + (entity_manager.player_entity.height / 1.5f)
				};

				float mouse_distance_x = mouse_x - barrel_pos.x + scroll_factors.x;
				float mouse_distance_y = mouse_y - barrel_pos.y - scroll_factors.y;
				float mouse_angle = std::atan2(mouse_distance_y, mouse_distance_x);

				entity_manager.weapon_entities[0].pos.x = barrel_pos.x - entity_manager.weapon_entities[0].width / 3.0f;
				entity_manager.weapon_entities[0].pos.y = barrel_pos.y - entity_manager.weapon_entities[0].height / 2.0f;
				entity_manager.weapon_entities[0].angle_rad = mouse_angle;
			}
			break;

		case PROJECTILE:
			entity.pos.x += entity.vel.x;
			entity.pos.y += entity.vel.y;
			break;
	}

	memset(entity.collisions, false, sizeof(entity.collisions));
}

void respawn(
		Entities& entities, 
		SDL_Renderer* renderer,
		Weapon& weapon,
		Vector2i& scroll_factors,
		uint32_t level
		) {
	clear_window(renderer);
	center_message(renderer, "LEVEL " + std::to_string(level));
	display(renderer);
	SDL_Delay(2000);

	scroll_factors = {0, 0};

	entities.player_entity.alive  = true;
	entities.player_entity.health = 100;

	entities.player_entity.pos = entities.player_entity.respawn_pos;
	entities.player_entity.vel = entities.player_entity.respawn_vel;
	memset(
			entities.player_entity.collisions, 
			0, 
			sizeof(entities.player_entity.collisions)
			);
	
	weapon.ammo = weapon.max_ammo;
	for (Entity* entity: entities.get_non_player_entities()) {
		entity->alive  = true;
		entity->health = 100;

		entity->pos = entity->respawn_pos;
		entity->vel = entity->respawn_vel;
		memset(entity->collisions, 0, sizeof(entity->collisions));
	}
}
