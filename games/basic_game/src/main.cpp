#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/render_window.hpp"
#include "../include/entity_manager.hpp"
#include "../include/event_handler.hpp"
#include "../include/weapon.hpp"


int main(int argc, char* args[]) {
	RenderWindow window("KRISTIN DESTROY", WINDOW_WIDTH, WINDOW_HEIGHT);

	const std::string level_1 = "assets/levels/level_1.csv";
	const std::string level_2 = "assets/levels/level_2.csv";
	const std::string levels[2] = {level_1, level_2};

	Textures textures(&window);
	Entities entities(&textures, level_1);

	window.level_width = entities.level_width;

	bool done = false;

	uint32_t level_number = 0;

	respawn(entities, window, level_number + 1);

	const uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);

	// Define weapons
	Weapon chaingun(
			CHAINGUN,
			100000,
			20,
			10,
			textures.weapon_textures[CHAINGUN]
			);

	// Game Loop
	while (!done) {
		// Get keyboard state
		SDL_PumpEvents();
		handle_keyboard(
				keyboard_state,
				entities,
				window.scroll_factor_x,
				window.scroll_factor_y,
				textures,
				chaingun
				);


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
			SDL_PumpEvents();
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
