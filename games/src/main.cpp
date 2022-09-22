#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>

#include "render_window.hpp"
#include "entity.hpp"
#include "utils.hpp"
#include "event_handler.hpp"
#include "read_level.hpp"
#include "constants.h"


int main(int argc, char* args[]) {
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		std::cout << "SDL initialization failed." << SDL_GetError() << std::endl;
	}
	if (!(IMG_Init(IMG_INIT_PNG))) {
		std::cout << "IMG_Init failed" << SDL_GetError() << std::endl;
	}

	RenderWindow window("Game v1.0", WINDOW_WIDTH, WINDOW_HEIGHT);

	//SDL_Texture* background_texture	  = window.load_texture(SKY_FILEPATH);
	SDL_Texture* background_texture	  = window.load_texture(NAMEK_FILEPATH);
	SDL_Texture* grass_texture        = window.load_texture(GRASS_FILEPATH);
	SDL_Texture* dirt_texture         = window.load_texture(DIRT_FILEPATH);
	SDL_Texture* player_texture_right = window.load_texture(PLAYER_FILEPATH_RIGHT);
	SDL_Texture* player_texture_left  = window.load_texture(PLAYER_FILEPATH_LEFT);


	int background_type = 0;
	int ground_type  	= 1;
	int player_type 	= 3;

	Entity background_entity(
			Vector2f(0, 0),								// position 
			Vector2f(0, 0),								// velocity 
			WINDOW_WIDTH,								// width
			WINDOW_HEIGHT,								// height 
			background_type,							// type
			background_texture							// texture	
			);

	std::vector<Entity> ground_entities;
	std::vector<int> level_design = read_level_csv(LEVEL_DESIGN_FILEPATH);
	std::vector<std::vector<int>> collidable_entity_positions;
	int max_width = int(level_design.size()) * GROUND_SIZE;

	for (int idx = 0; idx < int(level_design.size()); idx++) {
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
					grass_texture 							// texture
					)
				);
		for (int idx_2 = 1; idx_2 < n_dirt_layers + 1; idx_2++) {
			ground_entities.push_back(
					Entity(
						Vector2f(x_pos, height + idx_2 * GROUND_SIZE), 				// position
						Vector2f(0, 0), 											// velocity
						GROUND_SIZE, 												// width
						GROUND_SIZE, 												// height
						ground_type,												// type
						dirt_texture												// texture
						)
					);
			}
		}

	for (int idx = 0; idx < int(ground_entities.size()); idx++) {
		if (ground_entities[idx].type == 1) {
			std::vector<int> tmp;
			tmp.push_back(ground_entities[idx].pos.x);
			tmp.push_back(ground_entities[idx].pos.y);
			tmp.push_back(ground_entities[idx].pos.x + ground_entities[idx].width);
			tmp.push_back(ground_entities[idx].pos.y + ground_entities[idx].height);
			collidable_entity_positions.push_back(tmp);
		}
	}
	
	Entity player_entity(
			Vector2f(100, PLATFORM_HEIGHT - (250)), 						// position
			Vector2f(0, 0),													// velocity 
			int(PLAYER_SIZE_FACTOR * PLAYER_WIDTH_SRC),						// width
			int(PLAYER_SIZE_FACTOR * PLAYER_HEIGHT_SRC),					// height 
			player_type,													// type
			player_texture_right											// texture
			);

	SDL_Event event;
	bool done = false;
	int step_idx = 0;
	int final_idx;

	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				window.quit();
				done = true;
			}
			player_entity = handle(event, player_entity, collidable_entity_positions);
		}
		player_entity = update(player_entity, collidable_entity_positions, max_width);
		SDL_Delay(1000 * TIME_STEP);

		int scroll_factor_x = int(player_entity.pos.x - (2 * WINDOW_WIDTH / 3));
		int scroll_factor_y = int((WINDOW_HEIGHT / 5) - player_entity.pos.y);
		if (scroll_factor_x < 0) {
			scroll_factor_x = 0;
		}
		if (scroll_factor_y < 0) {
			scroll_factor_y = 0;
		}

		step_idx  = (step_idx + 1) % (6 * BUFFER_FACTOR);
		if (player_entity.vel.x != 0) {
			final_idx = step_idx;
		}
		else {
			final_idx  = step_idx % (3 * BUFFER_FACTOR);
			final_idx += 7 * BUFFER_FACTOR;
		}

		window.clear();

		// Render all.
		window.render(background_entity);
		for (Entity& entity: ground_entities) {
			window.render(entity, 0, NULL, scroll_factor_x, scroll_factor_y);
		}
		window.render(
				player_entity, 
				final_idx / BUFFER_FACTOR, 
				player_texture_left,
				scroll_factor_x,
				scroll_factor_y
				);

		window.display();
}

	return 0;
}
