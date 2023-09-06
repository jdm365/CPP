#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "../include/entity.hpp"
#include "../include/render_window.hpp"
#include "../include/constants.h"


void detect_collisions(std::vector<Entity*> entities);
void _detect_collision(
		Entity& src_entity, 
		Entity& dst_entity
		);
void update(Entity& entity);
void handle_keyboard(SDL_Event& event, Entity& player_entity);
void respawn(Entity& player_entity);
