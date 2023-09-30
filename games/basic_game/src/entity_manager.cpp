#include <iostream>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.hpp"
#include "math.hpp"
#include "render_window.hpp"
#include "read_level.hpp"
#include "constants.h"
#include "entity_manager.hpp"


Entities::Entities(const std::string level_design_filepath) {
	// Define background entity
	background_entity = BackgroundEntity(background_texture);

	std::vector<std::vector<int8_t>> level_design = read_level_csv(level_design_filepath);
	level_width  = (int)level_design[0].size() * GROUND_SIZE;
	level_height = (int)level_design.size() * GROUND_SIZE;

	// Let origin be top left corner
	int x_offset = 0;
	int y_offset = 0;

	// Query texture dimensions for grass dirt enemies and ammo
	int grass_width, grass_height;
	int dirt_width, dirt_height;
	int walking_enemy_width, walking_enemy_height;
	int flying_enemy_width, flying_enemy_height;
	int ammo_width, ammo_height;

	SDL_QueryTexture(terrain_textures[GRASS], NULL, NULL, &grass_width, &grass_height);
	SDL_QueryTexture(terrain_textures[DIRT], NULL, NULL, &dirt_width, &dirt_height);
	SDL_QueryTexture(enemy_textures[WALKING], NULL, NULL, &walking_enemy_width, &walking_enemy_height);
	SDL_QueryTexture(enemy_textures[FLYING], NULL, NULL, &flying_enemy_width, &flying_enemy_height);
	SDL_QueryTexture(ammo_texture, NULL, NULL, &ammo_width, &ammo_height);


	// Top left to bottom right
	for (std::vector<int8_t>& row: level_design) {
		x_offset = 0;
		y_offset += GROUND_SIZE;
		for (const int8_t& tile_id: row) {
			x_offset += GROUND_SIZE;
			switch(tile_id) {
				case -1:
					break;
				case 0:
					// Define grass entity
					ground_entities.emplace_back(
							Vector2f {(float)x_offset, (float)y_offset},
							Vector2f {0, 0},
							GROUND_SIZE,
							GROUND_SIZE,
							GRASS,
							terrain_textures[GRASS]
							);
					break;
				case 1:
					// Define dirt entity
					ground_entities.emplace_back(
							Vector2f {(float)x_offset, (float)y_offset},
							Vector2f {0, 0},
							GROUND_SIZE,
							GROUND_SIZE,
							DIRT,
							terrain_textures[DIRT],
							0,
							7
							);
					break;
				case 2:
					// Define enemy_textures[WALKING] entity
					walking_enemy_entities.emplace_back(
							Vector2f {(float)x_offset, (float)y_offset - walking_enemy_height},
							Vector2f {0.25f * PLAYER_SPEED, 0},
							64,
							96,
							ENEMY_WALKING,
							enemy_textures[WALKING]
							);
					break;
				case 3:
					// Define ammo entity
					pickup_entities.emplace_back(
							Vector2f {(float)x_offset, (float)y_offset - ammo_height},
							Vector2f {0, 0},
							64,
							96,
							AMMO,
							ammo_texture
							);
					break;
				case 4:
					// Define enemy_textures[FLYING] entity
					flying_enemy_entities.emplace_back(
							Vector2f {(float)x_offset, (float)y_offset - flying_enemy_height},
							Vector2f {0, 0},
							96,
							128,
							ENEMY_FLYING,
							enemy_textures[FLYING]
							);
					break;
				case 5:
					pickup_entities.emplace_back(
							Vector2f {(float)x_offset, (float)y_offset - 64.0f},
							Vector2f {0, 0},
							64,
							96,
							DOOR,
							door_texture
							);
					break;
			}
		}
	}
	
	// Define player entity 
	float scale_height = PLAYER_HEIGHT_SRC / (float)PLAYER_WIDTH_SRC;
	int player_width = 110;
	int player_height = scale_height * player_width;

	Vector2f respawn_pos;
	// TODO: Don't hardcode.
	respawn_pos.x = 2 * GROUND_SIZE;
	respawn_pos.y = level_height - 6 * GROUND_SIZE - player_height;

	player_entity = PlayerEntity(
			respawn_pos,
			Vector2f {0, 0},
			player_width,
			player_height,
			player_texture
			);


	SDL_Point size;
	SDL_QueryTexture(
			weapon_textures[CHAINGUN],
			NULL, 
			NULL, 
			&size.x, 
			&size.y
			);

	// Define weapon entities
	int weapon_width = 150;
	int weapon_height = size.y / (float)size.x * weapon_width;

	weapon_entities.emplace(CHAINGUN, WeaponEntity(
					respawn_pos,
					Vector2f {0, 0},
					weapon_width,
					weapon_height,
					CHAINGUN,
					BULLET,
					"CHAINGUN",
					100,
					100,
					45,
					10,
					weapon_textures[CHAINGUN]
					));

	SDL_QueryTexture(
			weapon_textures[PISTOL],
			NULL, 
			NULL, 
			&size.x, 
			&size.y
			);

	// Define weapon entities
	weapon_width = 36;
	weapon_height = size.y / (float)size.x * weapon_width;
	weapon_entities.emplace(PISTOL, WeaponEntity(
			respawn_pos,
			Vector2f {0, 0},
			weapon_width,
			weapon_height,
			PISTOL,
			BULLET,
			"PISTOL",
			10000,
			10000,
			4,
			10,
			weapon_textures[PISTOL]
			));
}
