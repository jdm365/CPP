#include <iostream>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/entity_manager.hpp"
#include "../include/entity.hpp"
#include "../include/math.hpp"
#include "../include/render_window.hpp"
#include "../include/read_level.hpp"
#include "../include/constants.h"


Textures::Textures(RenderWindow* window) {
	background_texture	  	  	= window->load_texture(NAMEK_FILEPATH);
	player_texture		  	  	= window->load_texture(PLAYER_FILEPATH);
	terrain_textures[GRASS]   	= window->load_texture(GRASS_FILEPATH);
	terrain_textures[DIRT]    	= window->load_texture(DIRT_FILEPATH);
	enemy_textures[WALKING]   	= window->load_texture(KRISTIN_MOUSTACHE_FILEPATH);
	enemy_textures[FLYING]    	= window->load_texture(KRISTIN_JUMP_FILEPATH);
	weapon_textures[CHAINGUN]   = window->load_texture(CHAINGUN_FILEPATH);
	projectile_textures[BULLET] = window->load_texture(BULLET_FILEPATH);
}

Entities::Entities(Textures* textures, const std::string level_design_filepath) {
	// Define background entity
	background_entity = Entity(
			Vector2f {0, 0},							// position 
			Vector2f {0, 0},							// velocity 
			WINDOW_WIDTH,								// width
			WINDOW_HEIGHT,								// height 
			BACKGROUND,									// entity type
			TRANSPARENT,								// collision type
			textures->background_texture				// texture	
			);

	std::vector<int> level_design = read_level_csv(level_design_filepath);
	level_width = (int)level_design.size() * GROUND_SIZE;

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
					textures->enemy_textures[FLYING],
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
					textures->enemy_textures[FLYING]								// texture	
					);
			continue;
		}
		else if (level_design[idx] == -25) {
			SDL_Point size;
			SDL_QueryTexture(
					textures->enemy_textures[WALKING],
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
					textures->enemy_textures[WALKING]								// texture	
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
					textures->terrain_textures[GRASS]		// texture
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
						textures->terrain_textures[DIRT]								// texture
					);
			}
		}
	
	// Define player entity 
	float scale_height = PLAYER_HEIGHT_SRC / (float)PLAYER_WIDTH_SRC;
	int player_width = 110;
	int player_height = scale_height * player_width;

	Vector2f respawn_pos;
	respawn_pos.x = 4 * GROUND_SIZE;
	respawn_pos.y = WINDOW_HEIGHT - level_design[4] * GROUND_SIZE - player_height;

	player_entity = Entity(
			respawn_pos,																// position
			Vector2f {0, 0},															// velocity 
			player_width,																// width
			player_height,																// height 
			PLAYER,																		// entity type
			DYNAMIC,																	// collision type
			textures->player_texture													// texture
			);

	SDL_Point size;
	SDL_QueryTexture(
			textures->weapon_textures[CHAINGUN],
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
			textures->weapon_textures[CHAINGUN]													// texture
			);
}
