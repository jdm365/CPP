#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <string.h>

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


void RenderWindow::render(
		Entity& entity, 
		int step_index, 
		SDL_Texture* left_texture, 
		int scroll_factor_x,
		int scroll_factor_y
		) {
	SDL_Point size;
	SDL_QueryTexture(entity.get_texture(), NULL, NULL, &size.x, &size.y);

	// Left sprite sheet is reflected about the y-axis.
	if (entity.vel.x < 0) {
		step_index = abs(step_index - 4);
	}
	
	// Used to choose correct sprite on sprite sheet.
	SDL_Rect src;

	// Size and location to display on screen.
	SDL_Rect dst;
	const char* ground = "ground";
	const char* player = "player";

	if (strcmp(entity.type, ground) == 0) {
		src.x = 0;
		src.y = 0;
		src.w = size.x;
		src.h = size.y;

		dst.x = entity.pos.x - scroll_factor_x;
		dst.y = entity.pos.y + scroll_factor_y;
		dst.w = entity.width;
		dst.h = entity.height;
	}
	else if (strcmp(entity.type, player) == 0) {
		// src.x = ((size.x / 5.0f) * (step_index % 5));
		// src.y = ((size.y / 2.0f) * float(step_index > 4));
		Vector2f _src;
		if (entity.vel.x == 0.0f) {
			_src = PLAYER_RIGHT_SPRITE_SHEET_POSITIONS[(step_index % 3) + 7];
		}
		else {
			_src = PLAYER_RIGHT_SPRITE_SHEET_POSITIONS[step_index % 5];
		}
		src = {(int)_src.x, (int)_src.y, PLAYER_WIDTH_SRC, PLAYER_HEIGHT_SRC - 1};

		src.w = PLAYER_WIDTH_SRC;
		src.h = PLAYER_HEIGHT_SRC - 1;

		dst.x = entity.pos.x - scroll_factor_x;
		dst.y = entity.pos.y + scroll_factor_y;
		dst.w = entity.width;
		dst.h = entity.height;
	}
	else {
		src.x = 0;
		src.y = 0;
		src.w = size.x;
		src.h = size.y;

		dst.x = entity.pos.x;
		dst.y = entity.pos.y;
		dst.w = entity.width;
		dst.h = entity.height;
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
