#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.hpp"

Entity handle(SDL_Event event, Entity player_entity, bool game_running);
Entity update(Entity player_entity, int GROUND_HEIGHT);

