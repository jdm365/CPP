#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "../include/entity.hpp"
#include "../include/constants.h"


void detect_collision(
		std::vector<bool>& collisions,
		Entity& player_entity, 
		Entity& other_entity
		);
void update(
		Entity& player_entity,
		Vector2f& player_pos,
		std::vector<bool>& collisions
		);
void handle(
		SDL_Event& event, 
		Entity& entity
		);
void respawn(Entity& player_entity);
