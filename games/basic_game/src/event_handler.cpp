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
		Vector2i& scroll_factors,
		const int frame_idx
		) {
	// PLAYER
	#pragma omp parallel for schedule(static)
	for (GroundEntity& entity: entities.ground_entities) {
		detect_collision_player_ground(entities.player_entity, entity); 
	}

	#pragma omp parallel for schedule(static)
	for (EnemyEntity& entity: entities.walking_enemy_entities) {
		if (!entity.alive) {
			continue;
		}
		detect_collision_player_enemy(entities.player_entity, entity); 
	}
	#pragma omp parallel for schedule(static)
	for (EnemyEntity& entity: entities.flying_enemy_entities) {
		if (!entity.alive) {
			continue;
		}
		detect_collision_player_enemy(entities.player_entity, entity); 
	}

	#pragma omp parallel for schedule(static)
	for (PickupEntity& entity: entities.pickup_entities) {
		if (!entity.alive) {
			continue;
		}
		detect_collision_player_pickup(entities.player_entity, entity, entities);
	}

	// PROJECTILES
	#pragma omp parallel for schedule(static)
	for (ProjectileEntity& entity: entities.projectile_entities) {
		if (!entity.alive) {
			continue;
		}
		for (EnemyEntity& dst_entity: entities.walking_enemy_entities) {
			if (!dst_entity.alive) {
				continue;
			}
			detect_collision_projectile_enemy(entity, dst_entity);
		}
		for (EnemyEntity& dst_entity: entities.flying_enemy_entities) {
			if (!dst_entity.alive) {
				continue;
			}
			detect_collision_projectile_enemy(entity, dst_entity);
		}
		for (GroundEntity& dst_entity: entities.ground_entities) {
			if (!dst_entity.alive) {
				continue;
			}
			detect_collision_projectile_ground(entity, dst_entity);
		}
		detect_collision_projectile_player(entity, entities.player_entity);
	}
	update_player(entities, entities.player_entity, scroll_factors);
	update_projectiles(entities.projectile_entities, scroll_factors);

	// ENEMIES
	#pragma omp parallel for schedule(static)
	for (EnemyEntity& src_entity: entities.walking_enemy_entities) {
		if (!src_entity.alive) {
			continue;
		}
		for (EnemyEntity& dst_entity: entities.walking_enemy_entities) {
			if (!dst_entity.alive) {
				continue;
			}
			detect_collision_enemy_enemy(src_entity, dst_entity);
		}
		for (EnemyEntity& dst_entity: entities.flying_enemy_entities) {
			if (!dst_entity.alive) {
				continue;
			}
			detect_collision_enemy_enemy(src_entity, dst_entity);
		}
	}

	#pragma omp parallel for schedule(static)
	for (EnemyEntity& src_entity: entities.flying_enemy_entities) {
		if (!src_entity.alive) {
			continue;
		}
		for (GroundEntity& dst_entity: entities.ground_entities) {
			if (!dst_entity.alive) {
				continue;
			}
			detect_collision_enemy_ground(src_entity, dst_entity);
		}
		update_enemy(entities, src_entity, scroll_factors, frame_idx);
	}

	#pragma omp parallel for schedule(static)
	for (EnemyEntity& src_entity: entities.walking_enemy_entities) {
		if (!src_entity.alive) {
			continue;
		}
		for (GroundEntity& dst_entity: entities.ground_entities) {
			if (!dst_entity.alive) {
				continue;
			}
			detect_collision_enemy_ground(src_entity, dst_entity);
		}
		update_enemy(entities, src_entity, scroll_factors, frame_idx);
	}
}


void detect_collision_player_enemy(
		PlayerEntity& src_entity,
		EnemyEntity&  dst_entity
		) {
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

	dst_entity.distance_from_player = Vector2f {
		dst_entity.pos.x - src_entity.pos.x + (dst_entity.width * 0.5f),
		dst_entity.pos.y - src_entity.pos.y - (dst_entity.height * 0.5f)
	};

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
	if (local_collisions[0] || local_collisions[2]) {
		src_entity.health -= ENEMY_DPF;

		// Play groan sound
		Mix_PlayChannel(-1, groan_sound, 0);
	}
	else if (local_collisions[1] || local_collisions[3]) {
		dst_entity.health -= 100;
		src_entity.vel.y = dst_entity.vel.y - JUMP_SPEED;
		dst_entity.alive = false;

		// Play boing sound
		Mix_PlayChannel(-1, boing_sound, 0);

		// Play dying sound
		Mix_PlayChannel(-1, dying_sound, 0);
		return;
	}

	if (local_collisions[0]) {
		src_entity.pos.x = dst_entity.pos.x + dst_entity.width + 1;
		src_entity.vel.x = dst_entity.vel.x;
	}
	else if (local_collisions[2]) {
		src_entity.pos.x = dst_entity.pos.x - src_entity.width - 1;
		src_entity.vel.x = dst_entity.vel.x;
	}

	src_entity.collisions[0] |= local_collisions[0];
	src_entity.collisions[1] |= local_collisions[1];
	src_entity.collisions[2] |= local_collisions[2];
	src_entity.collisions[3] |= local_collisions[3];
}

void detect_collision_player_ground(
		PlayerEntity& src_entity,
		GroundEntity& dst_entity
		) {
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
			(-EPS < dst_entity.pos.y - src_bottom_last)
				&& 
			(-EPS < src_bottom - dst_next_pos.y)
				&&
			(dst_entity.collision_edges & 0b1000)
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

	if (local_collisions[3] && !src_entity.collisions[3]) {
		src_entity.pos.y = dst_entity.pos.y - src_entity.height;
	}
	else if (local_collisions[1]) {
		src_entity.pos.y = dst_entity.pos.y + dst_entity.height;
	}
	else if (local_collisions[0]) {
		src_entity.pos.x = dst_entity.pos.x + dst_entity.width + 1;
		src_entity.vel.x = dst_entity.vel.x;
	}
	else if (local_collisions[2]) {
		src_entity.pos.x = dst_entity.pos.x - src_entity.width - 1;
		src_entity.vel.x = dst_entity.vel.x;
	}

	src_entity.collisions[0] |= local_collisions[0];
	src_entity.collisions[1] |= local_collisions[1];
	src_entity.collisions[2] |= local_collisions[2];
	src_entity.collisions[3] |= local_collisions[3];
}

void detect_collision_player_pickup(
		PlayerEntity& src_entity,
		PickupEntity& dst_entity,
		Entities& entities
		) {
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


	if (dst_entity.pickup_id == AMMO) {
		dst_entity.alive = false;
		uint8_t active_weapon = entities.player_entity.active_weapon_id;
		entities.weapon_entities[active_weapon].ammo = std::min(
				entities.weapon_entities[active_weapon].max_ammo,
				entities.weapon_entities[active_weapon].ammo + 50
				);
	}
	else if (dst_entity.pickup_id == DOOR) {
		entities.player_entity.level_complete = true;
	}

	// Play reload_sound
	Mix_PlayChannel(-1, reload_sound, 0);
}

void detect_collision_enemy_enemy(
		EnemyEntity& src_entity,
		EnemyEntity& dst_entity
		) {
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

	Vector2f src_vel = src_entity.vel;
	src_entity.vel = dst_entity.vel;
	dst_entity.vel = src_vel;

	src_entity.collisions[0] |= local_collisions[0];
	src_entity.collisions[1] |= local_collisions[1];
	src_entity.collisions[2] |= local_collisions[2];
	src_entity.collisions[3] |= local_collisions[3];
}

void detect_collision_enemy_ground(
		EnemyEntity& src_entity,
		GroundEntity& dst_entity
		) {
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

	src_entity.collisions[0] |= local_collisions[0];
	src_entity.collisions[1] |= local_collisions[1];
	src_entity.collisions[2] |= local_collisions[2];
	src_entity.collisions[3] |= local_collisions[3];
}

void detect_collision_projectile_enemy(
		ProjectileEntity& src_entity,
		EnemyEntity& dst_entity
		) {
	if (src_entity.projectile_id == FIREBALL) {
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

	dst_entity.health -= src_entity.damage;
	dst_entity.vel.x -= dst_entity.vel.x;

	// Stop rendering projectile by making it not alive.
	src_entity.alive = false;
}

void detect_collision_projectile_player(
		ProjectileEntity& src_entity,
		PlayerEntity& dst_entity
		) {
	if (src_entity.projectile_id == BULLET) {
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

	dst_entity.health -= src_entity.damage;
	// dst_entity.vel.x  -= dst_entity.vel.x;

	// Stop rendering projectile by making it not alive.
	src_entity.alive = false;

	// Play groan sound
	Mix_PlayChannel(-1, groan_sound, 0);
}

void detect_collision_projectile_ground(
		ProjectileEntity& src_entity,
		GroundEntity& dst_entity
		) {
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

	src_entity.alive = false;
}

void handle_keyboard(
		const uint8_t* last_keyboard_state,
		const uint8_t* keyboard_state,
		bool& prev_left_click,
		Entities& entity_manager,
		Vector2i& scroll_factors
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


	if (keyboard_state[SDL_SCANCODE_Q] && last_keyboard_state[SDL_SCANCODE_Q] == 0) {
		entity_manager.player_entity.active_weapon_id = (entity_manager.player_entity.active_weapon_id + 1) % entity_manager.weapon_entities.size();
	}
	if (keyboard_state[SDL_SCANCODE_E] && last_keyboard_state[SDL_SCANCODE_E] == 0) {
		entity_manager.player_entity.active_weapon_id = (entity_manager.player_entity.active_weapon_id - 1) % entity_manager.weapon_entities.size();
		if (entity_manager.player_entity.active_weapon_id < 0) {
			entity_manager.player_entity.active_weapon_id += entity_manager.weapon_entities.size();
		}
	}
	
	int mouse_x, mouse_y;
	uint32_t mouse_state = SDL_GetMouseState(&mouse_x, &mouse_y);

	if (mouse_state & SDL_BUTTON(SDL_BUTTON_LEFT)) {
		if (!prev_left_click) {
			entity_manager.weapon_entities[entity_manager.player_entity.active_weapon_id].fps_cntr = 0;
		}
		prev_left_click = true;

		// Fire bullet.
		mouse_x += scroll_factors.x;
		mouse_y -= scroll_factors.y;

		uint8_t active_weapon = entity_manager.player_entity.active_weapon_id;

		ProjectileEntity projectile_entity = entity_manager.weapon_entities[active_weapon].fire(
				mouse_x, 
				mouse_y
				);
		if (projectile_entity.texture != NULL) {
			entity_manager.projectile_entities.push_back(projectile_entity);
		}
	}
	else {
		prev_left_click = false;
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

				// Play dying sound
				Mix_PlayChannel(-1, dying_sound, 0);

				/*
				if (dying_sound == NULL) {
					printf("Mix_PlayChannel: %s\n", Mix_GetError());
				}
				*/
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

				Mix_PlayChannel(-1, dying_sound, 0);

				if (dying_sound == NULL) {
					printf("Mix_PlayChannel: %s\n", Mix_GetError());
				}
			}
			break;

		case PLAYER:
			entity.alive = (entity.health > 0);

			if (entity.collisions[3]) {
				entity.vel.y = 0.0f;
				entity.on_ground = true;
			}
			else if (entity.collisions[1]) {
				entity.vel.y = GRAVITY;
			}
			else {
				entity.vel.y += GRAVITY;
				entity.on_ground = false;
			}

			entity.pos.x += entity.vel.x;
			entity.pos.y += entity.vel.y;

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

				/*
				entity_manager.weapon_entities[0].pos.x = barrel_pos.x - entity_manager.weapon_entities[0].width / 3.0f;
				entity_manager.weapon_entities[0].pos.y = barrel_pos.y - entity_manager.weapon_entities[0].height / 2.0f;
				entity_manager.weapon_entities[0].angle_rad = mouse_angle;
				*/
				uint8_t active_weapon = entity_manager.player_entity.active_weapon_id;
				entity_manager.weapon_entities[active_weapon].pos.x = barrel_pos.x - entity_manager.weapon_entities[active_weapon].width / 3.0f;
				entity_manager.weapon_entities[active_weapon].pos.y = barrel_pos.y - entity_manager.weapon_entities[active_weapon].height / 2.0f;
				entity_manager.weapon_entities[active_weapon].angle_rad = mouse_angle;
			}
			break;

		case PROJECTILE:
			entity.pos.x += entity.vel.x;
			entity.pos.y += entity.vel.y;
			break;
	}

	memset(entity.collisions, false, sizeof(entity.collisions));
}

void update_player(
		Entities& entity_manager,
		PlayerEntity& player_entity,
		Vector2i& scroll_factors
		) {
	player_entity.alive = (player_entity.health > 0);

	if (player_entity.collisions[3]) {
		player_entity.vel.y = 0.0f;
		player_entity.on_ground = true;
	}
	else if (player_entity.collisions[1]) {
		player_entity.vel.y = GRAVITY;
		player_entity.on_ground = false;
	}
	else {
		player_entity.vel.y += GRAVITY;
		player_entity.on_ground = false;
	}

	player_entity.pos.x += player_entity.vel.x;
	player_entity.pos.y += player_entity.vel.y;

	// Keep player on screen.
	if (player_entity.pos.x < 0) {
		player_entity.pos.x = 0;
	}
	else if (player_entity.pos.x - scroll_factors.x + player_entity.width > WINDOW_WIDTH) {
		player_entity.pos.x = WINDOW_WIDTH + scroll_factors.x - player_entity.width;
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

		/*
		entity_manager.weapon_entities[0].pos.x = barrel_pos.x - entity_manager.weapon_entities[0].width / 3.0f;
		entity_manager.weapon_entities[0].pos.y = barrel_pos.y - entity_manager.weapon_entities[0].height / 2.0f;
		entity_manager.weapon_entities[0].angle_rad = mouse_angle;
		*/
		uint8_t active_weapon_id = entity_manager.player_entity.active_weapon_id;
		WeaponEntity& active_weapon = entity_manager.weapon_entities[active_weapon_id];
		if (active_weapon_id == CHAINGUN) {
			active_weapon.pos.x = barrel_pos.x - active_weapon.width * 0.33f;
			active_weapon.pos.y = barrel_pos.y - active_weapon.height * 0.5f;
		}
		else if (active_weapon_id == PISTOL) {
			active_weapon.pos.x = barrel_pos.x;
			active_weapon.pos.y = barrel_pos.y - active_weapon.height * 0.75f;
		}
		active_weapon.angle_rad = mouse_angle;
	}
	memset(player_entity.collisions, false, sizeof(player_entity.collisions));
}

void update_enemy(
		Entities& entity_manager,
		EnemyEntity& enemy_entity,
		Vector2i& scroll_factors,
		const int frame_idx
		) {
	if (!enemy_entity.alive) {
		// Play dying sound
		Mix_PlayChannel(-1, dying_sound, 0);
		return;
	}
	
	switch(enemy_entity.enemy_id) {
		case ENEMY_FLYING:
			if (enemy_entity.pos.y == enemy_entity.respawn_pos.y) {
				enemy_entity.vel.y -= 0.5f * JUMP_SPEED;
			}

			if ((l2_norm(enemy_entity.distance_from_player) < 1000.0f) && (frame_idx % 15 == 0)) { 
				float firing_angle = std::atan2(
						-enemy_entity.distance_from_player.y, 
						-enemy_entity.distance_from_player.x
						);
				firing_angle += (rand() % 100) / 100.0f - 0.5f;

				Vector2f projectile_vel = {
					4.0f * std::cos(firing_angle),
					4.0f * std::sin(firing_angle)
				};

				// Fire projectile
				ProjectileEntity fireball(
						Vector2f { enemy_entity.pos.x, enemy_entity.pos.y },
						projectile_vel,
						32,
						32,
						FIREBALL,
						10,
						projectile_textures[FIREBALL],
						firing_angle
						);
				entity_manager.projectile_entities.push_back(fireball);

				Mix_PlayChannel(-1, fireball_sound, 0);
			}

			enemy_entity.vel.y += GRAVITY * 0.167f;
			enemy_entity.pos.y += enemy_entity.vel.y;

			if (enemy_entity.health <= 0) {
				enemy_entity.alive = false;

				// Play dying sound
				Mix_PlayChannel(-1, dying_sound, 0);
			}

			// If at starting y, jump.
			if (enemy_entity.pos.y > enemy_entity.respawn_pos.y) {
				enemy_entity.pos.y = enemy_entity.respawn_pos.y;
				enemy_entity.vel.y = 0.00f;
			}
			break;

		case ENEMY_WALKING:
			if (!enemy_entity.collisions[3]) {
				enemy_entity.vel.y += GRAVITY;
				enemy_entity.on_ground = false;
			}
			else {
				enemy_entity.collisions[3] = true;
				if (enemy_entity.overhang) {
					enemy_entity.vel.x = -enemy_entity.vel.x;
				}
			}

			enemy_entity.pos.x += enemy_entity.vel.x;
			enemy_entity.pos.y += enemy_entity.vel.y;

			if (enemy_entity.health <= 0) {
				enemy_entity.alive = false;

				// Play dying sound
				Mix_PlayChannel(-1, dying_sound, 0);
			}
			break;
	}
	memset(enemy_entity.collisions, false, sizeof(enemy_entity.collisions));
	enemy_entity.overhang = true;
}

void update_projectiles(
		std::vector<ProjectileEntity>& projectile_entities,
		Vector2i& scroll_factors
		) {
	for (int idx = 0; idx < (int)projectile_entities.size(); idx++) {
		if (
				!projectile_entities[idx].alive
					||
				projectile_entities[idx].pos.x - scroll_factors.x < 0
					||
				projectile_entities[idx].pos.x - scroll_factors.x > WINDOW_WIDTH
					||
				projectile_entities[idx].pos.y + scroll_factors.y < 0
					||
				projectile_entities[idx].pos.y + scroll_factors.y > WINDOW_HEIGHT
			) {
			// Remove projectile from vector.
			if ((int)projectile_entities.size() > 0) {
				projectile_entities.erase(projectile_entities.begin() + idx);
			}
			continue;
		}
		projectile_entities[idx].pos.x += projectile_entities[idx].vel.x;
		projectile_entities[idx].pos.y += projectile_entities[idx].vel.y;
	}
}

void respawn(
		Entities& entities, 
		SDL_Renderer* renderer,
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
	
	uint8_t active_weapon = entities.player_entity.active_weapon_id;
	entities.weapon_entities[active_weapon].ammo = entities.weapon_entities[active_weapon].max_ammo;
	entities.player_entity.alive  = true;
	entities.player_entity.health = 100;

	entities.player_entity.pos = entities.player_entity.respawn_pos;
	entities.player_entity.vel = entities.player_entity.respawn_vel;

	for (EnemyEntity& entity: entities.walking_enemy_entities) {
		entity.alive  = true;
		entity.health = 100;

		entity.pos = entity.respawn_pos;
		entity.vel = entity.respawn_vel;
		memset(entity.collisions, 0, sizeof(entity.collisions));
	}

	for (EnemyEntity& entity: entities.flying_enemy_entities) {
		entity.alive  = true;
		entity.health = 100;

		entity.pos = entity.respawn_pos;
		entity.vel = entity.respawn_vel;
		memset(entity.collisions, 0, sizeof(entity.collisions));
	}

	for (PickupEntity& entity: entities.pickup_entities) {
		entity.alive = true;
	}
	entities.player_entity.level_complete = false;
}
