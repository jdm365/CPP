#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "../include/entity.hpp"
#include "../include/entity_manager.hpp"
#include "../include/render_window.hpp"
#include "../include/constants.h"
#include "../include/weapon.hpp"
#include "../include/math.hpp"

void detect_collisions(
		Entities& entities,
		Vector2i& scroll_factors,
		const int frame_idx
		);
void _detect_collision(
		Entity& src_entity, 
		Entity& dst_entity
		);
void detect_collision_player_ground(
		PlayerEntity& player_entity,
		GroundEntity& ground_entity
		);
void detect_collision_player_enemy(
		PlayerEntity& player_entity,
		EnemyEntity& enemy_entity
		);
void detect_collision_player_pickup(
		PlayerEntity& player_entity,
		PickupEntity& pickup_entity,
		Entities& entities
		);
void detect_collision_enemy_enemy(
		EnemyEntity& enemy_entity_1,
		EnemyEntity& enemy_entity_2
		);
void detect_collision_enemy_ground(
		EnemyEntity& enemy_entity,
		GroundEntity& ground_entity
		);
void detect_collision_projectile_player(
		ProjectileEntity& projectile_entity,
		PlayerEntity& enemy_entity
		);
void detect_collision_projectile_enemy(
		ProjectileEntity& projectile_entity,
		EnemyEntity& enemy_entity
		);
void detect_collision_projectile_ground(
		ProjectileEntity& projectile_entity,
		GroundEntity& ground_entity
		);
void update_player(
		Entities& entity_manager,
		PlayerEntity& entity, 
		Vector2i& scroll_factors
		);
void update_enemy(
		Entities& entity_manager,
		EnemyEntity& entity, 
		Vector2i& scroll_factors,
		const int frame_idx
		);
void update_projectiles(
		std::vector<ProjectileEntity>& projectile_entities,
		Vector2i& scroll_factors
		);
void handle_keyboard(
		const uint8_t* last_keyboard_state,
		const uint8_t* keyboard_state,
		bool& prev_left_click,
		Entities& entity_manager,
		Vector2i& scroll_factors
		);
void respawn(
		Entities& entities, 
		SDL_Renderer* renderer,
		Vector2i& scroll_factors,
		uint32_t level
		);
