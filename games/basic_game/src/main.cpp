#include <random>
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

	int level_width  = entities.level_width;
	int level_height = entities.level_height;

	uint32_t level_number = 0;
	int 	 frame_idx 	  = 0;
	int 	 time_elapsed = 0;

	const uint8_t* keyboard_state = SDL_GetKeyboardState(NULL);
	uint8_t 	   last_keyboard_state[SDL_NUM_SCANCODES] = {0};
	bool 		   prev_left_click = false;

	respawn(entities, renderer, scroll_factors, level_number + 1);

	// Game Loop
	while (true) {
		// Play music if not done playing
		if (Mix_PlayingMusic() == 0) {
			Mix_PlayMusic(imps_song, -1);
			// Mix_PlayMusic(dark_halls, -1);
		}

		// Get keyboard state
		SDL_PumpEvents();
		handle_keyboard(
				last_keyboard_state,
				keyboard_state,
				prev_left_click,
				entities,
				scroll_factors
				);
		memcpy(last_keyboard_state, keyboard_state, SDL_NUM_SCANCODES);

		detect_collisions(
				entities,
				scroll_factors,
				frame_idx
				);

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
		// render_score(renderer, 0);
		uint8_t active_weapon = entities.player_entity.active_weapon_id;
		weapon_message(
				renderer,
				entities.weapon_entities[active_weapon].name,
				entities.weapon_entities[active_weapon].ammo
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
			respawn(entities, renderer, scroll_factors, level_number + 1);
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
			respawn(entities, renderer, scroll_factors, level_number + 1);
			continue;
		}

		display(renderer);
		tick(time_elapsed, frame_idx);
	}

	return 0;
}
