#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/render_window.hpp"
#include "../include/entity_manager.hpp"
#include "../include/event_handler.hpp"


int main(int argc, char* args[]) {
	RenderWindow window("KRISTIN DESTROY", WINDOW_WIDTH, WINDOW_HEIGHT);

	const std::string level_1 = "assets/levels/level_1.csv";
	const std::string level_2 = "assets/levels/level_2.csv";
	const std::string levels[2] = {level_1, level_2};

	Textures textures(&window);
	Entities entities(&textures, level_1);

	window.level_width = entities.level_width;

	SDL_Event event;
	bool done = false;

	// uint32_t level_number = 0;
	uint32_t level_number = 1;

	respawn(entities, window, level_number + 1);

	// Game Loop
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				window.quit();
				done = true;
			}
			handle_keyboard(
					event, 
					entities,
					textures.energy_projectile_texture
					);
		}

		detect_collisions(
				entities,
				window.scroll_factor_x,
				window.scroll_factor_y
				);

		window.clear();

		window.render_all(entities);
		window.render_health_bar(entities.player_entity.health * 0.01f);
		window.render_score(entities.player_entity.score);

		if (!entities.player_entity.alive) {
			window.center_message("YOU DIED");
			window.display();
			SDL_Delay(2000);
			respawn(entities, window, level_number + 1);
			continue;
		}

		// Check if level complete
		if (entities.player_entity.pos.x - window.scroll_factor_x + entities.player_entity.width > WINDOW_WIDTH) {
			// Wait 3 seconds, then load next level.
			level_number++;
			window.center_message("LEVEL COMPLETE");
			window.display();
			SDL_Delay(3000);
			entities = Entities(&textures, levels[level_number]);
			window.level_width = entities.level_width;
			window.scroll_factor_x = 0;
			window.scroll_factor_y = 0;
			respawn(entities, window, level_number + 1);
			continue;
		}

		window.display();
		window.tick();
	}

	return 0;
}
