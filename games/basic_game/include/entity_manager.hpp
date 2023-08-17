#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "../include/render_window.hpp"
#include "../include/entity.hpp"
#include "../include/constants.h"


struct Textures {
	Textures(RenderWindow* window);
	SDL_Texture* background_texture;
	SDL_Texture* grass_texture;
	SDL_Texture* dirt_texture;
	SDL_Texture* player_texture_right;
	SDL_Texture* player_texture_left;
	SDL_Texture* kristin_texture;
};


struct Entities {
	Entities(Textures* textures);
	Entity background_entity;
	Entity player_entity;
	std::vector<Entity> enemy_entities;
	std::vector<Entity> ground_entities;
	
	int level_width;
};
