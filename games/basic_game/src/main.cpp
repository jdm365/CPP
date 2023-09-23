#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "render_window.hpp"
#include "entity_manager.hpp"
#include "event_handler.hpp"
#include "weapon.hpp"


int main(int argc, char* args[]) {
	SDL_Window*   window   = NULL;
	SDL_Renderer* renderer = NULL;
	init_window("KRISTIN DESTROY", &window, &renderer);
	load_textures(renderer);

	const std::string level_1 = "assets/levels/level_1.csv";
	const std::string level_2 = "assets/levels/level_2.csv";
	const std::string levels[2] = {level_1, level_2};

	Vector2i scroll_factors = {0, 0};

	Entities entities(level_1);

	int level_width = entities.level_width;

	uint32_t level_number = 0;
	int 	 frame_idx 	  = 0;
	int 	 time_elapsed = 0;

	const uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);

	// Define weapons
	Weapon chaingun(
			CHAINGUN,
			BULLET,
			"CHAINGUN",
			100,
			45,
			10,
			weapon_textures[CHAINGUN]
			);
	respawn(entities, renderer, chaingun, scroll_factors, level_number + 1);

	// Game Loop
	while (true) {
		// Get keyboard state
		SDL_PumpEvents();
		handle_keyboard(
				keyboard_state,
				entities,
				scroll_factors,
				chaingun
				);


		detect_collisions(
				entities,
				scroll_factors
				);
		if (entities.player_entity.reload) {
			entities.player_entity.reload = false;
			chaingun.ammo = chaingun.max_ammo;
		}

		clear_window(renderer);

		update_scroll_factors(
				scroll_factors,
				entities.player_entity.pos,
				level_width
				);

		render_all(renderer, entities, scroll_factors, frame_idx);
		render_health_bar(
				renderer,
				entities.player_entity.health * 0.01f
				);
		render_score(
				renderer,
				entities.player_entity.score
				);
		weapon_message(
				renderer,
				chaingun.name, 
				chaingun.ammo
				);

		if (!entities.player_entity.alive) {
			center_message(renderer, "YOU DIED");
			display(renderer);
			SDL_PumpEvents();
			SDL_Delay(2000);
			respawn(entities, renderer, chaingun, scroll_factors, level_number + 1);
			continue;
		}

		// Check if level complete
		if (entities.player_entity.pos.x - scroll_factors.x + entities.player_entity.width > WINDOW_WIDTH) {
			// Wait 3 seconds, then load next level.
			level_number++;
			center_message(renderer, "LEVEL COMPLETE");
			display(renderer);
			SDL_Delay(3000);
			entities = Entities(levels[level_number]);
			level_width = entities.level_width;
			respawn(entities, renderer, chaingun, scroll_factors, level_number + 1);
			continue;
		}

		display(renderer);
		tick(time_elapsed, frame_idx);
	}

	return 0;
}
