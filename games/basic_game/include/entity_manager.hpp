#pragma once

#include <iostream>
#include <vector>
#include <unordered_map>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/render_window.hpp"
#include "../include/entity.hpp"
#include "../include/constants.h"


struct Entities {
	// Entity background_entity;
	BackgroundEntity background_entity;

	// std::vector<Entity> walking_enemy_entities;
	std::vector<EnemyEntity> walking_enemy_entities;

	// std::vector<Entity> flying_enemy_entities;
	std::vector<EnemyEntity> flying_enemy_entities;

	// std::vector<Entity> projectile_entities;
	std::vector<ProjectileEntity> projectile_entities;

	std::unordered_map<uint8_t, WeaponEntity> weapon_entities;

	std::vector<PickupEntity> pickup_entities;

	// std::vector<Entity> ground_entities;
	std::vector<GroundEntity> ground_entities;

	// Entity player_entity;
	PlayerEntity player_entity;
	
	int level_width;
	int level_height;

	Entities(const std::string level_design_filepath);
};
