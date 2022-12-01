#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "entity.hpp"
#include "constants.h"

void handle(
		SDL_Event& event, 
		Entity& entity,
		std::vector<bool>& collisions
		);
void update(
		Entity& entity,
		std::vector<bool>& collisions
		);
void detect_collisions(
		std::vector<bool>& collisions,
		Entity& player_entity, 
		std::vector<Entity>& collidable_entities,
		int scroll_factor_x
		);
