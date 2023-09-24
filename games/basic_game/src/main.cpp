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
	load_sounds();

	const std::string level_1 = "assets/levels/level_1.csv";
	const std::string level_2 = "assets/levels/level_2.csv";
	const std::string levels[2] = {level_1, level_2};

	Vector2i scroll_factors = {0, 0};

	Entities entities(level_1);

	int level_width = entities.level_width;
	int level_height = entities.level_height;

	uint32_t level_number = 0;
	int 	 frame_idx 	  = 0;
	int 	 time_elapsed = 0;

	const uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);

	// Define weapons
	Weapon chaingun(
			CHAINGUN,
			BULLET,
			"CHAINGUN",
			150,
			45,
			10,
			weapon_textures[CHAINGUN]
			);
	respawn(entities, renderer, chaingun, scroll_factors, level_number + 1);

	// Game Loop
	while (true) {
		// Play music if not done playing
		if (Mix_PlayingMusic() == 0) {
			int rand_val = rand() % 2;
			std::cout << rand_val << std::endl;
			if (rand_val == 0) {
				Mix_PlayMusic(imps_song, -1);
			} 
			else {
				Mix_PlayMusic(dark_halls, -1);
			}
		}

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
			chaingun.ammo = std::min(chaingun.ammo + 50, chaingun.max_ammo);

			// Play reload sound
			Mix_PlayChannel(-1, reload_sound, 0);
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

		if (entities.player_entity.pos.y > level_height) {
			entities.player_entity.alive = false;
		}

		if (!entities.player_entity.alive) {
			center_message(renderer, "YOU DIED");
			display(renderer);
			SDL_PumpEvents();
			// Pause music
			Mix_PauseMusic();
			SDL_Delay(2000);
			Mix_ResumeMusic();
			respawn(entities, renderer, chaingun, scroll_factors, level_number + 1);
			continue;
		}

		// Check if level complete
		if (entities.player_entity.pos.x - scroll_factors.x + entities.player_entity.width > WINDOW_WIDTH) {
			// Wait 3 seconds, then load next level.
			level_number++;
			center_message(renderer, "LEVEL COMPLETE");
			display(renderer);
			Mix_PauseMusic();
			SDL_Delay(3000);
			Mix_ResumeMusic();
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
