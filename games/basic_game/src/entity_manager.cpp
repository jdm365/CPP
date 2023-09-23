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
	background_entity = Entity(
			Vector2f {0, 0},							// position 
			Vector2f {0, 0},							// velocity 
			WINDOW_WIDTH,								// width
			WINDOW_HEIGHT,								// height 
			BACKGROUND,									// entity type
			TRANSPARENT,								// collision type
			background_texture							// texture	
			);

	std::vector<std::vector<int8_t>> level_design = read_level_csv(level_design_filepath);
	level_width = (int)level_design[0].size() * GROUND_SIZE;
	int level_height = (int)level_design.size() * GROUND_SIZE;

	// Let origin be top left corner
	int x_offset = 0;
	int y_offset = 0;

	int idx = 0;


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
		for (const int8_t& tile_id: row) {
			idx++;
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
							GROUND,
							STATIC,
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
							GROUND,
							STATIC,
							terrain_textures[DIRT]
							);
					break;
				case 2:
					// Define enemy_textures[WALKING] entity
					walking_enemy_entities.emplace_back(
							Vector2f {(float)x_offset, (float)y_offset - walking_enemy_height},
							Vector2f {0, 0},
							64,
							96,
							ENEMY_WALKING,
							DYNAMIC,
							enemy_textures[WALKING]
							);
					break;
				case 3:
					// Define ammo entity
					ammo_entities.emplace_back(
							Vector2f {(float)x_offset, (float)y_offset - ammo_height},
							Vector2f {0, 0},
							64,
							96,
							AMMO,
							STATIC,
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
							DYNAMIC,
							enemy_textures[FLYING]
							);
					break;
			}
		}
		y_offset += GROUND_SIZE;
	}
	std::cout << "Number of tiles: " << idx << std::endl;


	/*
	int width_counter = 0;

	// Define ground entities
	for (int idx = 0; idx < (int)level_design.size(); ++idx) {
		if (level_design[idx] == -99) {
			width_counter++;
			continue;
		}
		else if (level_design[idx] == -50) {
			int gap_width = GROUND_SIZE * (2 * width_counter + 1);

			SDL_Point size;
			SDL_QueryTexture(
					enemy_textures[FLYING],
					NULL, 
					NULL, 
					&size.x, 
					&size.y
					);
			float scale_height = size.y / (float)size.x;
			float texture_width  = gap_width - 2 * GROUND_SIZE;
			float texture_height = scale_height * texture_width;

			float enemy_x_pos = (idx + 1.5f) * GROUND_SIZE - gap_width * 0.5f;
			float enemy_y_pos = WINDOW_HEIGHT + 2 * (int)texture_height;

			flying_enemy_entities.emplace_back(
					Vector2f {enemy_x_pos, enemy_y_pos}, 							// position
					Vector2f {0, 0},												// velocity 
					texture_width,													// width
					texture_height,													// height
					ENEMY_FLYING,													// entity type
					DYNAMIC,														// collision type
					enemy_textures[FLYING]											// texture	
					);
			continue;
		}
		else if (level_design[idx] == -25) {
			SDL_Point size;
			SDL_QueryTexture(
					enemy_textures[WALKING],
					NULL, 
					NULL, 
					&size.x, 
					&size.y
					);
			float scale_height = size.y / (float)size.x;
			float texture_width  = 3 * GROUND_SIZE;
			float texture_height = scale_height * texture_width;

			float enemy_x_pos = (idx - 1) * GROUND_SIZE;
			float enemy_y_pos = WINDOW_HEIGHT - level_design[idx - 1] * GROUND_SIZE - (int)texture_height;

			walking_enemy_entities.emplace_back(
					Vector2f {enemy_x_pos, enemy_y_pos}, 							// position
					Vector2f {0.25f * PLAYER_SPEED, 0},								// velocity 
					texture_width,													// width
					texture_height,													// height
					ENEMY_WALKING,													// type
					DYNAMIC,														// collision type
					enemy_textures[WALKING]											// texture	
					);
		}
		width_counter = 0;

		int x_pos  		  = GROUND_SIZE * idx;
		int diff          = (level_design[idx] == -25) ? level_design[idx - 1] : level_design[idx];
		int height		  = WINDOW_HEIGHT - diff * GROUND_SIZE;
		int n_dirt_layers = (GROUND_SIZE * 99 - height) / GROUND_SIZE;

		Vector2f position;
		position.x = x_pos;
		position.y = height;
		
		ground_entities.emplace_back(
					position, 								// position
					Vector2f {0, 0}, 						// velocity
					GROUND_SIZE, 							// width
					GROUND_SIZE, 							// height
					GROUND,									// entity type
					STATIC,									// collision type
					terrain_textures[GRASS]					// texture
				);

		for (int jdx = 1; jdx < n_dirt_layers + 1; ++jdx) {
			Vector2f position;
			position.x = x_pos;
			position.y = height + jdx * GROUND_SIZE;
			ground_entities.emplace_back(
						position, 													// position
						Vector2f {0, 0}, 											// velocity
						GROUND_SIZE, 												// width
						GROUND_SIZE, 												// height
						GROUND,														// entity type
						STATIC,														// collision type
						terrain_textures[DIRT]										// texture
					);
			}
		}
	*/
	
	// Define player entity 
	float scale_height = PLAYER_HEIGHT_SRC / (float)PLAYER_WIDTH_SRC;
	int player_width = 110;
	int player_height = scale_height * player_width;

	Vector2f respawn_pos;
	// TODO: Don't hardcode.
	respawn_pos.x = 4 * GROUND_SIZE;
	respawn_pos.y = level_height - 6 * GROUND_SIZE - player_height;
	// respawn_pos.y = WINDOW_HEIGHT - 6 * GROUND_SIZE - player_height;

	player_entity = Entity(
			respawn_pos,																// position
			Vector2f {0, 0},															// velocity 
			player_width,																// width
			player_height,																// height 
			PLAYER,																		// entity type
			DYNAMIC,																	// collision type
			player_texture																// texture
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
	weapon_entities.emplace_back(
			respawn_pos,																// position
			Vector2f {0, 0},															// velocity 
			weapon_width,																// width
			weapon_height,																// height 
			WEAPON,																		// entity type
			TRANSPARENT,																// collision type
			weapon_textures[CHAINGUN]													// texture
			);
}
