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
	background_texture	  	  = (*window).load_texture(NAMEK_FILEPATH);
	grass_texture         	  = (*window).load_texture(GRASS_FILEPATH);
	dirt_texture          	  = (*window).load_texture(DIRT_FILEPATH);
	player_texture		  	  = (*window).load_texture(PLAYER_FILEPATH);
	kristin_jump_texture  	  = (*window).load_texture(KRISTIN_JUMP_FILEPATH);
	kristin_moustache_texture = (*window).load_texture(KRISTIN_MOUSTACHE_FILEPATH);
	energy_projectile_texture = (*window).load_texture(ENERGY_PROJECTILE_FILEPATH);
}

Entities::Entities(Textures* textures) {
	// Define background entity
	background_entity = Entity(
			Vector2f {0, 0},								// position 
			Vector2f {0, 0},								// velocity 
			WINDOW_WIDTH,								// width
			WINDOW_HEIGHT,								// height 
			BACKGROUND,									// type
			(*textures).background_texture,				// texture	
			false,										// collidable
			true										// static_entity
			);

	std::vector<int> level_design = read_level_csv(LEVEL_DESIGN_FILEPATH);
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
					(*textures).kristin_jump_texture,
					NULL, 
					NULL, 
					&size.x, 
					&size.y
					);
			float scale_height = size.y / (float)size.x;
			float texture_width  = gap_width - 2 * GROUND_SIZE;
			float texture_height = scale_height * texture_width;

			float enemy_x_pos = (idx + 1.5f) * GROUND_SIZE - gap_width * 0.5f;
			float enemy_y_pos = WINDOW_HEIGHT + 2 * player_entity.height;

			enemy_entities.emplace_back(
					Vector2f {enemy_x_pos, enemy_y_pos}, 							// position
					Vector2f {0, 0},												// velocity 
					texture_width,													// width
					texture_height,													// height
					ENEMY,															// type
					(*textures).kristin_jump_texture,								// texture
					true,															// collidable
					false															// static_entity
					);
			continue;
		}
		else if (level_design[idx] == -25) {
			SDL_Point size;
			SDL_QueryTexture(
					(*textures).kristin_moustache_texture,
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

			enemy_entities.emplace_back(
					Vector2f {enemy_x_pos, enemy_y_pos}, 							// position
					Vector2f {0.25f * PLAYER_SPEED, 0},								// velocity 
					texture_width,													// width
					texture_height,													// height
					ENEMY,															// type
					(*textures).kristin_moustache_texture,							// texture
					true,															// collidable
					false															// static_entity
					);
			// continue;
		}
		width_counter = 0;

		int x_pos  		  = GROUND_SIZE * idx;
		int diff          = (level_design[idx] == -25) ? level_design[idx - 1] : level_design[idx];
		int height		  = WINDOW_HEIGHT - diff * GROUND_SIZE;
		int n_dirt_layers = (WINDOW_HEIGHT - height) / GROUND_SIZE;

		Vector2f position;
		position.x = x_pos;
		position.y = height;
		
		ground_entities.emplace_back(
					position, 								// position
					Vector2f {0, 0}, 						// velocity
					GROUND_SIZE, 							// width
					GROUND_SIZE, 							// height
					GROUND,									// type
					(*textures).grass_texture, 				// texture
					true,									// collidable
					true									// static_entity
				);

		for (int jdx = 1; jdx < n_dirt_layers + 1; ++jdx) {
			bool collidable = false;
			if (idx == (int)level_design.size() - 1) {
				collidable = true;
			} 
			else if ((level_design[idx - 1] == -99) || (level_design[idx + 1] == -99)) {
				collidable = true;
			}
			else if (
					(level_design[idx - 1] < level_design[idx] || level_design[idx + 1] < level_design[idx]) 
						&& 
					((jdx < (level_design[idx] - level_design[idx - 1])) || (jdx < (level_design[idx] - level_design[idx + 1])))
					) {
				collidable = true;
			}
			Vector2f position;
			position.x = x_pos;
			position.y = height + jdx * GROUND_SIZE;
			ground_entities.emplace_back(
						position, 													// position
						Vector2f {0, 0}, 											// velocity
						GROUND_SIZE, 												// width
						GROUND_SIZE, 												// height
						GROUND,														// type
						(*textures).dirt_texture,									// texture
						collidable,													// collidable
						true														// static_entity
					);
			}
		}
	
	// Define player entity 
	float scale_height = PLAYER_HEIGHT_SRC / (float)PLAYER_WIDTH_SRC;
	int player_width = 120;
	int player_height = scale_height * player_width;

	Vector2f respawn_pos;
	respawn_pos.x = 4 * GROUND_SIZE;
	respawn_pos.y = WINDOW_HEIGHT - level_design[4] * GROUND_SIZE - player_height;

	player_entity = Entity(
			respawn_pos,																// position
			Vector2f {0, 0},															// velocity 
			player_width,																// width
			player_height,																// height 
			PLAYER,																		// type
			(*textures).player_texture,													// texture
			true,																		// collidable
			false																		// static_entity
			);
}
