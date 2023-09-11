#pragma once

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/entity.hpp"
#include "../include/constants.h"
#include "../include/entity_manager.hpp"

struct Entities;

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
		void render_all(Entities& entities);
		void render_score(int score);
		void center_message(std::string text);
		int  get_sprite_index(Vector2f& player_pos, Vector2f& player_vel);
		void display();
		void quit();
		void tick();
		int level_width = 0;
		int scroll_factor_x = 0;
		int scroll_factor_y = 0;
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
		int step_idx = 0;
		int time_elapsed = 0;
		int delay_time = 0;
		int start_time = 0;
};


const Vector2f PLAYER_RIGHT_SPRITE_SHEET_POSITIONS[10] = {
	Vector2f {19, 11},
	Vector2f {83, 11},
	Vector2f {147, 11},
	Vector2f {212, 11},
	Vector2f {276, 11},
	Vector2f {19, 75},
	Vector2f {83, 75},
	Vector2f {146, 75},
	Vector2f {210, 75},
	Vector2f {273, 75}
};
