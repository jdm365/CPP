#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "render_window.hpp"
#include "entity.hpp"

RenderWindow::RenderWindow(
		const char* player_title, 
		int player_width, 
		int player_height
		): window(NULL), renderer(NULL) {

	window = SDL_CreateWindow(
			player_title, 
			SDL_WINDOWPOS_UNDEFINED, 
			SDL_WINDOWPOS_UNDEFINED, 
			player_width, 
			player_height, 
			SDL_WINDOW_SHOWN
			);
	if (window == NULL) {
		std::cout << "Window failed to initialize. Error: ";
		std::cout << SDL_GetError() << std::endl;
	}

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}

SDL_Texture* RenderWindow::load_texture(const char* filepath) {
	SDL_Texture* texture = NULL;
	texture = IMG_LoadTexture(renderer, filepath);

	if (texture == NULL) {
		std::cout << "Failed to load a texture." << SDL_GetError() << std::endl;
	}
	return texture;
}

int RenderWindow::get_refresh_rate() {
	int display_index = SDL_GetWindowDisplayIndex(window);

	SDL_DisplayMode mode;

	SDL_GetDisplayMode(display_index, 0, &mode);

	return mode.refresh_rate;
}

void RenderWindow::clear() {
	SDL_RenderClear(renderer);
}

void RenderWindow::render(Entity& player_entity) {
	SDL_Rect src;
	src.x = player_entity.get_current_frame().x;
	src.y = player_entity.get_current_frame().y;
	src.w = player_entity.get_current_frame().w;
	src.h = player_entity.get_current_frame().h;

	SDL_Rect dst;
	dst.x = player_entity.get_pos().x;
	dst.y = player_entity.get_pos().y;
	dst.w = player_entity.get_current_frame().w;
	dst.h = player_entity.get_current_frame().h;

	SDL_RenderCopy(renderer, player_entity.get_texture(), &src, &dst);
}

void RenderWindow::display() {
	SDL_RenderPresent(renderer);
}

void RenderWindow::clean_up() {
	SDL_DestroyWindow(window);
}
