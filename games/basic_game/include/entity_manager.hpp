#pragma once

#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "../include/render_window.hpp"
#include "../include/entity.hpp"
#include "../include/constants.h"


enum WeaponType {
	CHAINGUN
};

enum ProjectileType {
	BULLET
};

enum EnemyType {
	WALKING,
	FLYING
};

enum TerrainType {
	GRASS,
	DIRT
};

struct Entities {
	Entity player_entity;

	std::vector<Entity> walking_enemy_entities;
	std::vector<Entity> flying_enemy_entities;
	std::vector<Entity> projectile_entities;
	std::vector<Entity> weapon_entities;

	std::vector<Entity> ammo_entities;
	std::vector<Entity> ground_entities;

	Entity background_entity;
	
	int level_width;

	Entities(const std::string level_design_filepath);

	std::vector<Entity*> get_non_player_entities() {
		std::vector<Entity*> all_entities;
		all_entities.reserve(
				walking_enemy_entities.size() + 
				flying_enemy_entities.size() + 
				ground_entities.size() + 
				projectile_entities.size()
				);
		for (Entity& entity: walking_enemy_entities) {
			all_entities.push_back(&entity);
		}
		for (Entity& entity: flying_enemy_entities) {
			all_entities.push_back(&entity);
		}
		for (Entity& entity: ground_entities) {
			all_entities.push_back(&entity);
		}
		for (Entity& entity: projectile_entities) {
			all_entities.push_back(&entity);
		}
		for (Entity& entity: ammo_entities) {
			all_entities.push_back(&entity);
		}
		return all_entities;
	}
};
