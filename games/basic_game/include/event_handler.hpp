#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "../include/entity.hpp"
#include "../include/entity_manager.hpp"
#include "../include/render_window.hpp"
#include "../include/constants.h"


void detect_collisions(
		Entities& entities,
		int scroll_factor_x,
		int scroll_factor_y
		);
void _detect_collision(
		Entity& src_entity, 
		Entity& dst_entity
		);
void update(
		Entities& entity_manager,
		Entity& entity, 
		int scroll_factor_x, 
		int scroll_factor_y
		);
void handle_keyboard(
		// SDL_Event& event, 
		const uint8_t* keyboard_state,
		Entities& entity_manager,
		int& scroll_factor_x,
		int& scroll_factor_y,
		SDL_Texture* texture
		);
void respawn(Entities& entities, RenderWindow& window, uint32_t level);
