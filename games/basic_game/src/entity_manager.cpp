#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <vector>

#include "entity_manager.hpp"
#include "entity.hpp"
#include "math.hpp"
#include "render_window.hpp"
#include "read_level.hpp"
#include "constants.h"


Textures::Textures(RenderWindow* window) {
	background_texture	  = (*window).load_texture(NAMEK_FILEPATH);
	grass_texture         = (*window).load_texture(GRASS_FILEPATH);
	dirt_texture          = (*window).load_texture(DIRT_FILEPATH);
	player_texture_right  = (*window).load_texture(PLAYER_FILEPATH_RIGHT);
	player_texture_left   = (*window).load_texture(PLAYER_FILEPATH_LEFT);
}

Entities::Entities(Textures* textures) {
	const char* background_type = "background";
	const char* ground_type  	= "ground";
	const char* player_type 	= "player";

	// Define background entity
	background_entity = Entity(
			Vector2f(0, 0),								// position 
			Vector2f(0, 0),								// velocity 
			WINDOW_WIDTH,								// width
			WINDOW_HEIGHT,								// height 
			background_type,							// type
			(*textures).background_texture,				// texture	
			false										// collidable
			);

	std::vector<int> level_design = read_level_csv(LEVEL_DESIGN_FILEPATH);
	level_width = (int)level_design.size() * GROUND_SIZE;

	// Define ground entities
	for (int idx = 0; idx < (int)level_design.size(); ++idx) {
		if (level_design[idx] == -99) {
			continue;
		}

		int x_pos  		  = GROUND_SIZE * idx;
		int height		  = PLATFORM_HEIGHT - level_design[idx] * GROUND_SIZE;
		int n_dirt_layers = (WINDOW_HEIGHT - height) / GROUND_SIZE;
		
		ground_entities.push_back(
				Entity(
					Vector2f(x_pos, height),				// position
					Vector2f(0, 0), 						// velocity
					GROUND_SIZE, 							// width
					GROUND_SIZE, 							// height
					ground_type,							// type
					(*textures).grass_texture, 				// texture
					true									// collidable
					)
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
					(jdx < (level_design[idx] - level_design[idx - 1]))
					) {
				collidable = true;
			}
			ground_entities.push_back(
					Entity(
						Vector2f(x_pos, height + jdx * GROUND_SIZE), 				// position
						Vector2f(0, 0), 											// velocity
						GROUND_SIZE, 												// width
						GROUND_SIZE, 												// height
						ground_type,												// type
						(*textures).dirt_texture,									// texture
						collidable													// collidable
						)
					);
			}
		}
	
	// Define player entity 
	player_entity = Entity(
			Vector2f(100, PLATFORM_HEIGHT - (250)), 						// position
			Vector2f(0, 0),													// velocity 
			int(PLAYER_SIZE_FACTOR * PLAYER_WIDTH_SRC),						// width
			int(PLAYER_SIZE_FACTOR * PLAYER_HEIGHT_SRC),					// height 
			player_type,													// type
			(*textures).player_texture_right,								// texture
			true															// collidable
			);
}
