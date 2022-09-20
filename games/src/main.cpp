#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>

#include "render_window.hpp"
#include "entity.hpp"
#include "utils.hpp"
#include "event_handler.hpp"
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
	int grass_type  	= 1;
	int dirt_type   	= 2;
	int player_type 	= 3;

	Entity background_entity(
			Vector2f(0, 0),								// position 
			Vector2f(0, 0),								// velocity 
			WINDOW_WIDTH,								// width
			PLATFORM_HEIGHT,							// height 
			background_type,							// type
			background_texture							// texture	
			);

	int n_ground_textures = WINDOW_WIDTH / GROUND_SIZE;
	int n_ground_layers   = 1 + ((WINDOW_HEIGHT - PLATFORM_HEIGHT) / GROUND_SIZE);

	std::vector<Entity>  ground_entities;
	std::vector<std::vector<int>> collidable_entity_positions;

	for (int layer = 0; layer < n_ground_layers; layer++) {
		for (int idx = 0; idx < n_ground_textures; idx++) {
			if (layer == 0) {
				ground_entities.push_back(
						Entity(
							Vector2f(GROUND_SIZE * idx, PLATFORM_HEIGHT + layer * GROUND_SIZE), // position
							Vector2f(0, 0), 													// velocity
							GROUND_SIZE, 														// width
							GROUND_SIZE, 														// height
							grass_type,															// type
							grass_texture 														// texture
							)
						);
				}
			else {
				ground_entities.push_back(
						Entity(
							Vector2f(GROUND_SIZE * idx, PLATFORM_HEIGHT + layer * GROUND_SIZE), // position
							Vector2f(0, 0), 													// velocity
							GROUND_SIZE, 														// width
							GROUND_SIZE, 														// height
							dirt_type,															// type
							dirt_texture														// texture
							)
						);
			}
		}
	}
	for (int idx = 0; idx < ground_entities.size(); idx++) {
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
			Vector2f(100, GROUND_HEIGHT - 100), 		// position
			Vector2f(0, 0),								// velocity 
			PLAYER_SIZE,								// width
			PLAYER_SIZE,								// height 
			player_type,								// type
			player_texture_right						// texture
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
		player_entity = update(player_entity, collidable_entity_positions);
		SDL_Delay(1000 * TIME_STEP);

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
			window.render(entity);
		}
		window.render(player_entity, final_idx / BUFFER_FACTOR, player_texture_left);

		window.display();
}

	return 0;
}