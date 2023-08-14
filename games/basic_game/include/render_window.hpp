#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.hpp"


class RenderWindow {
	public:
		RenderWindow(const char* player_title, int player_width, int player_height);
		SDL_Texture* load_texture(const char* player_filepath);
		void clear();
		void render(
				Entity& entity, 
				int step_index = 0, 
				SDL_Texture* left_texture = NULL, 
				int scroll_factor_x = 0,
				int scroll_factor_y = 0
				);
		void display();
		void quit();
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
};

const Vector2f PLAYER_RIGHT_SPRITE_SHEET_POSITIONS[10] = {
	Vector2f(18, 2),
	Vector2f(82, 2),
	Vector2f(145, 2),
	Vector2f(208, 2),
	Vector2f(273, 2),
	Vector2f(18, 61),
	Vector2f(82, 61),
	Vector2f(145, 66),
	Vector2f(210, 66),
	Vector2f(274, 66)
};

// 1920x1080 - 320x128
// ratio = 6:1 - 8.44:1
//
//
// Left boundaries - 105, 490, 870, 1250, 1640 -> 18, 82, 145, 208, 273
// Top boundaries - 15, 520 -> 2, 61
// Width - 200 - 33
// Height - 380 - 45
