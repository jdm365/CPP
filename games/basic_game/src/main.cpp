#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <iostream>
#include <vector>
#include <assert.h>

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
	int  step_idx = 0;
	int  final_idx;

	int  start_time;
	int  time_elapsed;
	int  delay_time;

	int x_scroll_start_pos = 2 * WINDOW_WIDTH / 3;
	int y_scroll_start_pos = WINDOW_HEIGHT / 5;

	int scroll_factor_x = 0;
	int scroll_factor_y = 0;

	int screen_pos;
	std::vector<bool> collisions = {false, false};

	// Game Loop
	while (!done) {
		start_time = int(SDL_GetTicks());

		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				window.quit();
				done = true;
			}
			handle(event, entities.player_entity, collisions);
		}
		detect_collisions(
				collisions, 
				entities.player_entity, 
				entities.ground_entities, 
				scroll_factor_x
				);
		update(entities.player_entity, collisions);

		scroll_factor_x = int(entities.player_entity.pos.x - x_scroll_start_pos);
		scroll_factor_y = int(y_scroll_start_pos - entities.player_entity.pos.y);

		if (scroll_factor_x < 0) {
			scroll_factor_x = 0;
		}
		if (scroll_factor_y < 0) {
			scroll_factor_y = 0;
		}

		step_idx++;
		if (entities.player_entity.vel.x != 0) {
			final_idx = int(step_idx / 3);
			final_idx = final_idx % 6;
		}
		else {
			final_idx = int(step_idx / 3) % 3;
			final_idx += 7;
		}

		window.clear();

		// Render all.
		window.render(entities.background_entity);

		for (Entity& entity: entities.ground_entities) {
			screen_pos = entity.pos.x - scroll_factor_x;
			if ((screen_pos + PADDING > 0) && (screen_pos < WINDOW_WIDTH)) {
				window.render(entity, 0, NULL, scroll_factor_x, scroll_factor_y);
			}
		}
		window.render(
				entities.player_entity, 
				final_idx, 
				textures.player_texture_left,
				scroll_factor_x,
				scroll_factor_y
				);

		window.display();

		time_elapsed = int(SDL_GetTicks() - start_time);
		delay_time   = int(1000 * TIME_STEP - time_elapsed);

		// If delay time is less than zero then code is updating too slowly 
		// (and surely is terrible).
		// assert(delay_time > 0);

		if (delay_time > 0) {
			SDL_Delay(delay_time);
		}
}

	return 0;
}
