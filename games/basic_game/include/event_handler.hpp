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
		Entity& player_entity,
		std::vector<Entity>& enemy_entities,
		std::vector<bool>& collisions
		);
void detect_collision(
		std::vector<bool>& collisions,
		Entity& player_entity, 
		Entity& other_entity
		);
