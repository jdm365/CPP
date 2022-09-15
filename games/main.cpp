#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>

#include "render_window.hpp"
#include "entity.hpp"
#include "utils.hpp"
#include "event_listener.hpp"

const char* GRASS_FILEPATH  = "textures/ground_grass_1.png";
const char* PLAYER_FILEPATH = "textures/hulking_knight.png";
const float TIME_STEP = 0.01f;
const int PLAYER_SIZE = 48;
const int GROUND_SIZE = 32;


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

	int n_ground_textures = 1280 / GROUND_SIZE;
	int platform_h        = 720 * 2 / 3;
	const int GROUND_HEIGHT = platform_h - GROUND_SIZE * 1.5;

	std::vector<Entity> ground_entities;
	for (int idx = 0; idx < n_ground_textures; idx++) {
		ground_entities.push_back(Entity(
					Vector2f(GROUND_SIZE * idx, platform_h), 
					Vector2f(0, 0), 
					GROUND_SIZE,
					GROUND_SIZE,
					grass_texture
					));
	}
	
	Entity player_entity(
			Vector2f(100, GROUND_HEIGHT), 
			Vector2f(0, 0), 
			PLAYER_SIZE,
			PLAYER_SIZE, 
			player_texture
			);

	bool game_running = true;

	SDL_Event event;

	while (game_running) {
		float current_time   = utils::hire_time_in_seconds();

		while (SDL_PollEvent(&event)) {
			player_entity = handle(event, player_entity, game_running);
			player_entity = update(player_entity, GROUND_HEIGHT);
			float new_time   = utils::hire_time_in_seconds();
			float frame_time = new_time - current_time;
			if (frame_time >= TIME_STEP) {
				break;
			}


		window.clear();

		for (Entity& entity: ground_entities) {
			window.render(entity);
		}

		window.render(player_entity);

		window.display();
	}
}

	window.clean_up();
	SDL_Quit();

	return 0;
}
