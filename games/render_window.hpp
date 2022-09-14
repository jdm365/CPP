#pragma once

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "entity.hpp"


class RenderWindow {
	public:
		RenderWindow(const char* player_title, int player_width, int player_height);
		SDL_Texture* load_texture(const char* player_filepath);
		int get_refresh_rate();
		void clean_up();
		void clear();
		void render(Entity& player_entity);
		void display();
	private:
		SDL_Window* window;
		SDL_Renderer* renderer;
};
