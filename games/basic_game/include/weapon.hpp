#pragma once

#include <string>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/event_handler.hpp"
#include "../include/math.hpp"
#include "../include/entity.hpp"


struct Weapon {
	WeaponType type = CHAINGUN;
	ProjectileType projectile_type = BULLET;
	std::string name;
	unsigned int ammo;
	unsigned int max_ammo;
	int rounds_per_second;
	float damage_per_round;
	int fps_cntr = 0;

	Weapon() {};
	Weapon(
			WeaponType type,
			ProjectileType projectile_type,
			std::string name,
			unsigned int ammo,
			int rounds_per_second,
			float damage_per_round,
			SDL_Texture* projectile_texture
			) : type(type),
				projectile_type(projectile_type),
				name(name),
				ammo(ammo),
				max_ammo(ammo),
				rounds_per_second(rounds_per_second),
				damage_per_round(damage_per_round) {};

	void fire(
			Entity& weapon_entity, 
			std::vector<Entity>& projectile_entities,
			int adjusted_mouse_x, 
			int adjusted_mouse_y,
			Textures& textures
			);
};