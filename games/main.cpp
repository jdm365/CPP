#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>

#include "render_window.hpp"
#include "entity.hpp"
#include "utils.hpp"
#include "event_listener.hpp"
#include "constants.h"


int main(int argc, char* args[]) {
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		std::cout << "SDL initialization failed." << SDL_GetError() << std::endl;
	}
	if (!(IMG_Init(IMG_INIT_PNG))) {
		std::cout << "IMG_Init failed" << SDL_GetError() << std::endl;
	}

	RenderWindow window("Game v1.0", WINDOW_WIDTH, WINDOW_HEIGHT);

	SDL_Texture* grass_texture  = window.load_texture(GRASS_FILEPATH);
	SDL_Texture* player_texture = window.load_texture(PLAYER_FILEPATH);

	int n_ground_textures = WINDOW_WIDTH / GROUND_SIZE;

	std::vector<Entity> ground_entities;
	for (int idx = 0; idx < n_ground_textures; idx++) {
		ground_entities.push_back(Entity(
					Vector2f(GROUND_SIZE * idx, PLATFORM_HEIGHT), 
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
		while (SDL_PollEvent(&event)) {
			player_entity = handle(event, player_entity, game_running);
		}
		player_entity = update(player_entity);
		SDL_Delay(1000 * TIME_STEP);


		window.clear();

		for (Entity& entity: ground_entities) {
			window.render(entity);
		}

		window.render(player_entity);

		window.display();
}

	window.clean_up();
	SDL_Quit();

	return 0;
}
