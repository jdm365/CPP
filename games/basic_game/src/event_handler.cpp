#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <omp.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/entity.hpp"
#include "../include/event_handler.hpp"
#include "../include/constants.h"
#include "../include/math.hpp"


void detect_collisions(
		Entities& entities, 
		int scroll_factor_x,
		int scroll_factor_y
		) {
	// Player -> Enemies_0, Enemies_1, ...
	// Player -> Platforms_0, Platforms_1, ...
	// Enemies_0 -> Enemies_1, Enemies_2, ...
	// Enemies_0 -> Platforms_0, Platforms_1, ...
	
	// PLAYER
	memset(
			entities.player_entity.collisions,
			false,
			sizeof(entities.player_entity.collisions)
			);

	std::vector<Entity*> non_player_entities = entities.get_non_player_entities();

	#pragma omp parallel for schedule(static)
	for (Entity* entity: non_player_entities) {
		if (!entity->alive || entity->entity_type == PROJECTILE) {
			continue;
		}
		_detect_collision(entities.player_entity, *entity);
	}
	update(entities.player_entity, scroll_factor_x, scroll_factor_y);

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
		memset(
				non_player_entities[idx]->collisions, 
				false, 
				sizeof(non_player_entities[idx]->collisions)
				);

		for (int jdx = idx + 1; jdx < (int)non_player_entities.size(); ++jdx) {
			if (!non_player_entities[jdx]->alive) {
				continue;
			}
			_detect_collision(
					*non_player_entities[idx],
					*non_player_entities[jdx]
					);
		}
		update(*non_player_entities[idx], scroll_factor_x, scroll_factor_y);
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

	// Detect if collision will happen on next frame.
	Vector2f src_next_pos = vector_add(src_entity.pos, src_entity.vel);
	Vector2f dst_next_pos = vector_add(dst_entity.pos, dst_entity.vel);

	float src_bottom = src_next_pos.y + src_entity.height;
	float src_right  = src_next_pos.x + src_entity.width - 20 * (src_entity.entity_type == PLAYER);

	float dst_bottom = dst_next_pos.y + dst_entity.height;
	float dst_right  = dst_next_pos.x + dst_entity.width;

	// Only detect collisions for objects near src.
	if (!(src_next_pos.x <= dst_right + EPS && src_right >= dst_next_pos.x - EPS
		&& src_next_pos.y <= dst_bottom + EPS && src_bottom >= dst_next_pos.y - EPS)) {
		return;
	}


	// Bottom
	if (
			(EPS > (src_bottom - dst_next_pos.y) - (src_entity.vel.y - dst_entity.vel.y))
				&& 
			(-EPS <= src_bottom - dst_next_pos.y)
		) [[likely]] {
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

	if (src_entity.entity_type == PLAYER) {
		if (!local_collisions[0] && !local_collisions[1] && !local_collisions[2] && !local_collisions[3]) {
			src_entity.health -= 2 * ENEMY_DPF;
		}
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
					src_entity.vel.y = -JUMP_SPEED + dst_entity.vel.y;
					if (dst_entity.health <= 0) {
						dst_entity.alive = false;
						memset(src_entity.collisions, false, sizeof(src_entity.collisions));
						return;
					}
				}
			}
			if (local_collisions[3]) {
				src_entity.pos.y = dst_entity.pos.y - src_entity.height;
				src_entity.vel.x = (dst_entity.vel.x == 0.0f) ? src_entity.vel.x : dst_entity.vel.x;
				src_entity.vel.y = dst_entity.vel.y + GRAVITY * (dst_entity.on_ground ? 0 : 1);
			}
			else if (local_collisions[0]) {
				src_entity.pos.x = dst_entity.pos.x + dst_entity.width + 1;
				src_entity.vel.y -= GRAVITY;
				src_entity.vel.x = dst_entity.vel.x;
			}
			else if (local_collisions[2]) {
				src_entity.pos.x = dst_entity.pos.x - src_entity.width - 1 + 20;
				src_entity.vel.y -= GRAVITY;
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

	src_entity.collisions[0] = local_collisions[0];
	src_entity.collisions[1] = local_collisions[1];
	src_entity.collisions[2] = local_collisions[2];
	src_entity.collisions[3] = local_collisions[3];
}

void handle_keyboard(
		SDL_Event& event, 
		Entities& entity_manager,
		SDL_Texture* texture
		) {
	// Get key state from event.
	
	if (event.type == SDL_KEYDOWN) {
		if (event.key.keysym.sym == SDLK_ESCAPE) {
			exit(0);
		}

		if (event.key.keysym.sym == SDLK_a) {
			entity_manager.player_entity.vel = Vector2f {
					-PLAYER_SPEED, 
					entity_manager.player_entity.vel.y
			};
			entity_manager.player_entity.facing_right = false;
		}
		if (event.key.keysym.sym == SDLK_d) {
			entity_manager.player_entity.vel = Vector2f {
					PLAYER_SPEED, 
					entity_manager.player_entity.vel.y
			};
			entity_manager.player_entity.facing_right = true;
		}
		if (event.key.keysym.sym == SDLK_w) {
			if (entity_manager.player_entity.on_ground) {
				entity_manager.player_entity.vel.y -= JUMP_SPEED;
			}
		}
		if (event.key.keysym.sym == SDLK_s) {
			;
		}

		if (event.key.keysym.sym == SDLK_SPACE) {
			// Fire bullet.
			entity_manager.projectile_entities.emplace_back(
					Vector2f {
						entity_manager.player_entity.pos.x + entity_manager.player_entity.width - 76,
						entity_manager.player_entity.pos.y + entity_manager.player_entity.height / 1.69f
					},
					Vector2f {
						64.0f * (entity_manager.player_entity.facing_right ? 1 : -1),
						0.0f
					},
					18,
					12,
					PROJECTILE,
					DYNAMIC,
					texture
			);
		}

		/*
		switch(event.key.keysym.sym) {
			case SDLK_ESCAPE:
				exit(0);
				break;

			case SDLK_a:
				entity_manager.player_entity.vel = Vector2f {
						-PLAYER_SPEED, 
						entity_manager.player_entity.vel.y
				};
				entity_manager.player_entity.facing_right = false;
				break;
			case SDLK_d:
				entity_manager.player_entity.vel = Vector2f {
						PLAYER_SPEED, 
						entity_manager.player_entity.vel.y
				};
				entity_manager.player_entity.facing_right = true;
				break;
			case SDLK_w:
				if (entity_manager.player_entity.on_ground) {
					entity_manager.player_entity.vel.y -= JUMP_SPEED;
				}
				break;
			case SDLK_k:
				break;
			case SDLK_SPACE:
				// Fire bullet.
				entity_manager.projectile_entities.emplace_back(
						Vector2f {
							entity_manager.player_entity.pos.x + entity_manager.player_entity.width - 76,
							entity_manager.player_entity.pos.y + 77
						},
						Vector2f {
							64.0f * (entity_manager.player_entity.facing_right ? 1 : -1),
							0.0f
						},
						18,
						12,
						PROJECTILE,
						DYNAMIC,
						texture
				);
				break;
 		}
		*/
	}

	else if (event.type == SDL_KEYUP) {
		switch(event.key.keysym.sym) {
			case SDLK_a:
				entity_manager.player_entity.vel = Vector2f {
						0.00f,
						entity_manager.player_entity.vel.y
				};
				break;
			case SDLK_d:
				entity_manager.player_entity.vel = Vector2f{
						0.00f,
						entity_manager.player_entity.vel.y
				};
				break;
		}
	}
}


void update(Entity& entity, int scroll_factor_x, int scroll_factor_y) {
	// Collisions -> left: 0, top: 1, right: 2, bottom: 3
	if (!entity.alive) return;
	
	switch(entity.entity_type) {
		case ENEMY_FLYING:
			if (entity.distance_from_player.x < GROUND_SIZE * 8 && entity.pos.y == WINDOW_HEIGHT) {
				entity.vel.y -= 1.5f * JUMP_SPEED;
			}

			entity.vel.y += GRAVITY;
			entity.pos.y += entity.vel.y;

			if (entity.health <= 0) {
				entity.alive = false;
			}

			// If below screen set at rest and y at WINDOW_HEIGHT.
			if (entity.pos.y > WINDOW_HEIGHT) {
				entity.pos.y = WINDOW_HEIGHT;
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
			entity.alive = entity.health > 0;

			if (!entity.collisions[3]) {
				entity.vel.y += GRAVITY;
				entity.on_ground = false;
			}
			else {
				entity.on_ground = true;
			}

			entity.pos.x += entity.vel.x;
			entity.pos.y += entity.vel.y;

			// Respawn if player falls off map.
			if (entity.pos.y + scroll_factor_y > WINDOW_HEIGHT || !entity.alive) {
				entity.alive = false;
			}

			// Keep player on screen.
			if (entity.pos.x < 0) {
				entity.pos.x = 0;
			}
			else if (entity.pos.x - scroll_factor_x + entity.width > WINDOW_WIDTH) {
				entity.pos.x = WINDOW_WIDTH + scroll_factor_x - entity.width + 20;
			}
			break;

		case PROJECTILE:
			entity.pos.x += entity.vel.x;
			break;
	}
}

void respawn(Entities& entities, RenderWindow& window, uint32_t level) {
	window.clear();
	window.center_message("LEVEL " + std::to_string(level));
	window.display();
	SDL_Delay(2000);
	window.scroll_factor_x = 0;
	window.scroll_factor_y = 0;

	entities.player_entity.alive  = true;
	entities.player_entity.health = 100;

	entities.player_entity.pos = entities.player_entity.respawn_pos;
	entities.player_entity.vel = entities.player_entity.respawn_vel;
	memset(
			entities.player_entity.collisions, 
			0, 
			sizeof(entities.player_entity.collisions)
			);
	
	for (Entity* entity: entities.get_non_player_entities()) {
		entity->alive  = true;
		entity->health = 100;

		entity->pos = entity->respawn_pos;
		entity->vel = entity->respawn_vel;
		memset(entity->collisions, 0, sizeof(entity->collisions));
	}
}
