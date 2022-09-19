#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.hpp"
#include "constants.h"

Entity handle(SDL_Event event, Entity entity);
Entity update(Entity entity);

