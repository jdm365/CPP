#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "entity.hpp"
#include "constants.h"

Entity handle(
		SDL_Event event, 
		Entity entity, 
		std::vector<std::vector<int>> collidable_entity_positions
		);
Entity update(Entity entity, std::vector<std::vector<int>>);
std::vector<bool> detect_collisions(
		Entity player_entity, 
		std::vector<std::vector<int>> collidable_entity_positions
		);
