#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.hpp"


class RenderWindow {
	public:
		RenderWindow(const char* player_title, int player_width, int player_height);
		SDL_Texture* load_texture(const char* player_filepath);
		void clear();
		void render(Entity& entity, int step_index = 0, SDL_Texture* left_texture = NULL);
		void display();
		void quit();
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
};
