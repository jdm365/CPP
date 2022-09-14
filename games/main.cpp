#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>

#include "render_window.hpp"
#include "entity.hpp"
#include "utils.hpp"


const char* GRASS_FILEPATH  = "textures/ground_grass_1.png";
const char* PLAYER_FILEPATH = "textures/hulking_knight.png";
const float TIME_STEP = 0.01f;


int main(int argc, char* args[]) {
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		std::cout << "SDL initialization failed." << SDL_GetError() << std::endl;
	}
	if (!(IMG_Init(IMG_INIT_PNG))) {
		std::cout << "IMG_Init failed" << SDL_GetError() << std::endl;
	}

	RenderWindow window("Game v1.0", 1280, 720);

	SDL_Texture* grass_texture  = window.load_texture(GRASS_FILEPATH);
	SDL_Texture* player_texture = window.load_texture(PLAYER_FILEPATH);

	int n_ground_textures = 1280 / 32;
	int platform_h        = 720 * 2 / 3;

	std::vector<Entity> ground_entities;
	for (int idx = 0; idx < n_ground_textures; idx++) {
		ground_entities.push_back(Entity(
					Vector2f(32 * idx, platform_h), 
					32,
					32,
					grass_texture
					));
	}
	
	Entity player_entity(
			Vector2f(100, platform_h - 32 * 1.5), 
			player_entity.PLAYER_SIZE,
			player_entity.PLAYER_SIZE, 
			player_texture
			);

	bool game_running = true;

	SDL_Event event;

	while (game_running) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				game_running = false;
			}
			if (event.type == SDL_KEYDOWN) {
				switch(event.key.keysym.sym) {
					case SDLK_LEFT:
						player_entity.pos = Vector2f (player_entity.get_pos().x - 10, player_entity.get_pos().y);
						break;
				}
				switch(event.key.keysym.sym) {
					case SDLK_RIGHT:
						player_entity.pos = Vector2f (player_entity.get_pos().x + 10, player_entity.get_pos().y);
						break;
				}
				switch(event.key.keysym.sym) {
					case SDLK_DOWN:
						if (player_entity.get_pos().y >= platform_h - 32 * 1.5) {
							break;
						}
						player_entity.pos = Vector2f (player_entity.get_pos().x, player_entity.get_pos().y + 10);
						break;
				}
				switch(event.key.keysym.sym) {
					case SDLK_UP:
						player_entity.pos = Vector2f (player_entity.get_pos().x, player_entity.get_pos().y - 10);
						break;
				}
			}

			window.clear();

			float current_time   = utils::hire_time_in_seconds();
			for (Entity& entity: ground_entities) {
				window.render(entity);
			}
			window.render(player_entity);
			window.display();

			float new_time   = utils::hire_time_in_seconds();
			float frame_time = new_time - current_time;

			if (frame_time < TIME_STEP) {
				SDL_Delay(1000 * (TIME_STEP - frame_time));
			}
		}

	}

	window.clean_up();
	SDL_Quit();

	return 0;
}