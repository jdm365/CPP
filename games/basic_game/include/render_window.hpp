#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/entity.hpp"
#include "../include/constants.h"


class RenderWindow {
	public:
		RenderWindow(const char* player_title, int player_width, int player_height);
		SDL_Texture* load_texture(const char* player_filepath);
		void clear();
		void render_entity(Entity& entity);
		void render_health_bar(
				float percent, 
				int x = WINDOW_WIDTH - 120,
				int y = 50, 
				int w = 100, 
				int h = 20, 
				SDL_Color fg_color = {255, 0, 0, 255},
				SDL_Color bg_color = {0, 0, 0, 255}
				);
		void render_score(int score);
		int  get_sprite_index(Vector2f& player_pos, Vector2f& player_vel);
		void display();
		void quit();
		void tick();
		int level_width = 0;
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		int step_idx = 0;
		int scroll_factor_x = 0;
		int scroll_factor_y = 0;
		int time_elapsed = 0;
		int delay_time = 0;
		int start_time = 0;
};


const Vector2f PLAYER_RIGHT_SPRITE_SHEET_POSITIONS[10] = {
	Vector2f {22, 11},
	Vector2f {83, 11},
	Vector2f {142, 11},
	Vector2f {206, 11},
	Vector2f {272, 11},
	Vector2f {22, 74},
	Vector2f {82, 74},
	Vector2f {140, 74},
	Vector2f {204, 74},
	Vector2f {270, 74}
};

// 1920x1080 - 320x128
// ratio = 6:1 - 8.44:1
//
//
// Left boundaries - 105, 490, 870, 1250, 1640 -> 18, 82, 145, 208, 273
// Top boundaries - 15, 520 -> 2, 61
// Width - 200 - 33
// Height - 380 - 45
