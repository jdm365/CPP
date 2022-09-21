#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "render_window.hpp"
#include "entity.hpp"
#include "constants.h"

RenderWindow::RenderWindow(const char* title, int width, int height) {
	// Initialize with NULL to catch errors.
	window	 = NULL;
	renderer = NULL;

	window = SDL_CreateWindow(
			title, 
			SDL_WINDOWPOS_UNDEFINED, 
			SDL_WINDOWPOS_UNDEFINED, 
			width, 
			height, 
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


void RenderWindow::clear() {
	SDL_RenderClear(renderer);
}


void RenderWindow::render(Entity& entity, int step_index, SDL_Texture* left_texture) {
	SDL_Point size;
	SDL_QueryTexture(entity.get_texture(), NULL, NULL, &size.x, &size.y);

	// Left sprite sheet is reflected about the y-axis.
	if (entity.vel.x < 0) {
		step_index = abs(step_index - 4);
	}
	
	// Used to choose correct sprite on sprite sheet.
	SDL_Rect src;
	src.x = (size.x / 5) * (step_index % 5);
	src.y = (size.y / 2) * float(step_index > 4);

	// Size and location to display on screen.
	SDL_Rect dst;
	dst.x = entity.pos.x;
	dst.y = entity.pos.y;

	switch(entity.type) {
		// player
		case 3:
			src.w = (size.x / 5) - ((size.x / 5) * PLAYER_FRAME_FACTOR_X);
			src.h = (size.y / 2) - ((size.y / 2) * PLAYER_FRAME_FACTOR_Y);
			dst.w = entity.width;
			dst.h = entity.height;
			break;
		default:
			src.w = size.x;
			src.h = size.y;
			dst.w = entity.width;
			dst.h = entity.height;
			break;
	}


	if (entity.vel.x < 0) {
		SDL_RenderCopy(renderer, left_texture, &src, &dst);
	}
	else {
		SDL_RenderCopy(renderer, entity.get_texture(), &src, &dst);
	}
}

void RenderWindow::display() {
	SDL_RenderPresent(renderer);
}

void RenderWindow::quit() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}
