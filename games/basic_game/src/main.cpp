#include <iostream>
#include <vector>
#include <assert.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/render_window.hpp"
#include "../include/entity.hpp"
#include "../include/entity_manager.hpp"
#include "../include/utils.hpp"
#include "../include/event_handler.hpp"
#include "../include/constants.h"


int main(int argc, char* args[]) {
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		std::cout << "SDL initialization failed." << SDL_GetError() << std::endl;
	}
	if (!(IMG_Init(IMG_INIT_PNG))) {
		std::cout << "IMG_Init failed" << SDL_GetError() << std::endl;
	}

	RenderWindow window("Game v1.0", WINDOW_WIDTH, WINDOW_HEIGHT);

	Textures textures(&window);
	Entities entities(&textures);

	SDL_Event event;
	bool done = false;
	int  step_idx = 0;
	int  final_idx;

	int  start_time;
	int  time_elapsed;
	int  delay_time;

	int x_scroll_start_pos = WINDOW_WIDTH / 2;
	int y_scroll_start_pos = WINDOW_HEIGHT / 5;

	int scroll_factor_x = 0;
	int scroll_factor_y = 0;

	int screen_pos;
	std::vector<bool> collisions = {false, false, false, false};

	int last_player_health = 0;

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
		collisions = {false, false, false, false};

		for (int idx = 0; idx < (int)entities.all_entities.size(); ++idx) {
			if (entities.all_entities[idx]->static_entity) {
				continue;
			}
			for (int jdx = idx + 1; jdx < (int)entities.all_entities.size(); ++jdx) {
				detect_collision(
						collisions, 
						*entities.all_entities[idx],
						*entities.all_entities[jdx]
						);
			}
		}

		entities.player_entity.alive = (entities.player_entity.health > 0);
		if (!entities.player_entity.alive) {
			respawn(entities.player_entity);
		}
		update(entities.player_entity, entities.enemy_entities, collisions);

		scroll_factor_x = (int)entities.player_entity.pos.x - x_scroll_start_pos;
		scroll_factor_y = (int)y_scroll_start_pos - entities.player_entity.pos.y;

		scroll_factor_x = std::min(
				scroll_factor_x, 
				entities.level_width - WINDOW_WIDTH
				);

		if (scroll_factor_x < 0) {
			scroll_factor_x = 0;
		}
		if (scroll_factor_y < 0) {
			scroll_factor_y = 0;
		}

		// 60 FPS
		// Complete 7 phase walking animation in 1 second.
		// 60 / 7 = 8.57 frames per walk cycle. Call it 8.
		step_idx++;
		if (entities.player_entity.vel.x != 0) {
			final_idx = int(step_idx / 8);
			final_idx = final_idx % 6;
		}
		else {
			final_idx = int(step_idx / 8) % 3;
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
		for (Entity& entity: entities.enemy_entities) {
			screen_pos = entity.pos.x - scroll_factor_x;
			window.render(entity, 0, NULL, scroll_factor_x, scroll_factor_y);
		}
		window.render(
				entities.player_entity, 
				final_idx, 
				textures.player_texture_left,
				scroll_factor_x,
				scroll_factor_y
				);
		window.render_health_bar(
				WINDOW_WIDTH - 120,
				50,
				100,
				20,
				entities.player_entity.health * 0.01f
				);

		window.display();

		time_elapsed = int(SDL_GetTicks() - start_time);
		delay_time   = int(1000 * TIME_STEP - time_elapsed);

		// If delay time is less than zero then code is updating too slowly 
		// (and is surely terrible).
		assert(delay_time > 0);

		if (entities.player_entity.health != last_player_health) {
			last_player_health = entities.player_entity.health;
		}
		if (delay_time > 0) {
			SDL_Delay(delay_time);
		}
}

	return 0;
	}
