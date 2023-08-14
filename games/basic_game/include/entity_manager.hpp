#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "render_window.hpp"
#include "entity.hpp"
#include "constants.h"


struct Textures {
	Textures(RenderWindow* window);
	SDL_Texture* background_texture;
	SDL_Texture* grass_texture;
	SDL_Texture* dirt_texture;
	SDL_Texture* player_texture_right;
	SDL_Texture* player_texture_left;
	// SDL_Texture* rocket_texture;
};


struct Entities {
	Entities(Textures* textures);
	Entity background_entity;
	std::vector<Entity> ground_entities;
	Entity player_entity;
	// Entity rocket_entity;
	
	int level_width;
};
