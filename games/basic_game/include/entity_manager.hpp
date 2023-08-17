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
	std::vector<Entity*> all_entities;
	
	int level_width;

	// Get all entities
	std::vector<Entity*> get_all_entities() {
		std::vector<Entity*> all_entities;
		all_entities.push_back(&player_entity);
		for (int idx = 0; idx < (int)enemy_entities.size(); ++idx) {
			all_entities.push_back(&enemy_entities[idx]);
		}
		for (int idx = 0; idx < (int)ground_entities.size(); ++idx) {
			all_entities.push_back(&ground_entities[idx]);
		}
		return all_entities;
	}
};
