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

struct Weapon;

void detect_collisions(
		Entities& entities,
		Vector2i& scroll_factors
		);
void _detect_collision(
		Entity& src_entity, 
		Entity& dst_entity
		);
void update(
		Entities& entity_manager,
		Entity& entity, 
		Vector2i& scroll_factors
		);
void handle_keyboard(
		const uint8_t* keyboard_state,
		Entities& entity_manager,
		Vector2i& scroll_factors,
		Weapon& weapon
		);
void respawn(
		Entities& entities, 
		SDL_Renderer* renderer,
		Weapon& weapon,
		Vector2i& scroll_factors,
		uint32_t level
		);
