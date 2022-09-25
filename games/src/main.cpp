#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>

#include "render_window.hpp"
#include "entity.hpp"
#include "entity_manager.hpp"
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

	struct Textures textures(&window);
	struct Entities entities(&textures);

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
			entities.player_entity = handle(
					event, 
					entities.player_entity, 
					entities.collidable_entity_positions
					);
		}
		entities.player_entity = update(entities.player_entity, entities.collidable_entity_positions);
		SDL_Delay(1000 * TIME_STEP);

		int scroll_factor_x = int(entities.player_entity.pos.x - (2 * WINDOW_WIDTH / 3));
		int scroll_factor_y = int((WINDOW_HEIGHT / 5) - entities.player_entity.pos.y);
		if (scroll_factor_x < 0) {
			scroll_factor_x = 0;
		}
		if (scroll_factor_y < 0) {
			scroll_factor_y = 0;
		}

		entities.rocket_entity.pos.x = entities.player_entity.pos.x + 12 - scroll_factor_x;
		entities.rocket_entity.pos.y = entities.player_entity.pos.y + 
									   	  entities.player_entity.height + scroll_factor_y;

		step_idx  = (step_idx + 1) % (6 * BUFFER_FACTOR);
		if (entities.player_entity.vel.x != 0) {
			final_idx = step_idx;
		}
		else {
			final_idx  = step_idx % (3 * BUFFER_FACTOR);
			final_idx += 7 * BUFFER_FACTOR;
		}

		window.clear();

		// Render all.
		window.render(entities.background_entity);
		for (Entity& entity: entities.ground_entities) {
			window.render(entity, 0, NULL, scroll_factor_x, scroll_factor_y);
		}
		window.render(
				entities.player_entity, 
				final_idx / BUFFER_FACTOR, 
				textures.player_texture_left,
				scroll_factor_x,
				scroll_factor_y
				);

		/*
		window.render(
				entities.rocket_entity, 
				0,
				NULL,
				scroll_factor_x,
				scroll_factor_y
				);
		*/
		window.display();
}

	return 0;
}
