#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
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
	TTF_Init();

	renderer = SDL_CreateRenderer(window, -1, SDL_RENDERER_ACCELERATED);
}


SDL_Texture* RenderWindow::load_texture(const char* filepath) {
	SDL_Texture* texture = NULL;
	texture = IMG_LoadTexture(renderer, filepath);

	// Tint red
	if (strcmp(filepath, "assets/kristin_goggles.jpg") == 0) {
		SDL_SetTextureColorMod(texture, 255, 0, 0);
	}

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

	// Used to choose correct sprite on sprite sheet.
	SDL_Rect src;

	// Size and location to display on screen.
	SDL_Rect dst;
	const char* ground = "ground";
	const char* player = "player";
	const char* enemy  = "enemy";

	SDL_Texture* entity_texture;

	if (strcmp(entity.type, ground) == 0) {
		src.x = 0;
		src.y = 0;
		src.w = size.x;
		src.h = size.y;

		dst.x = entity.pos.x - scroll_factor_x;
		dst.y = entity.pos.y + scroll_factor_y;
		dst.w = entity.width;
		dst.h = entity.height;

		entity_texture = entity.get_texture();
	}
	else if (strcmp(entity.type, player) == 0) {
		// Left sprite sheet is reflected about the y-axis.
		if (!entity.facing_right) {
			step_index = abs(step_index - 4);
		}

		Vector2f _src;
		if (entity.vel.x == 0.0f) {
			_src = PLAYER_RIGHT_SPRITE_SHEET_POSITIONS[(step_index % 3) + 7];
		}
		else {
			_src = PLAYER_RIGHT_SPRITE_SHEET_POSITIONS[step_index % 5];
		}
		if (!entity.facing_right) _src.x = size.x - _src.x - PLAYER_WIDTH_SRC;
		src = {(int)_src.x, (int)_src.y, PLAYER_WIDTH_SRC, PLAYER_HEIGHT_SRC - 1};

		src.w = PLAYER_WIDTH_SRC;
		src.h = PLAYER_HEIGHT_SRC - 1;

		dst.x = entity.pos.x - scroll_factor_x;
		dst.y = entity.pos.y + scroll_factor_y;
		dst.w = entity.width;
		dst.h = entity.height;

		entity_texture = entity.facing_right ? entity.get_texture() : left_texture;
	}
	else if (strcmp(entity.type, enemy) == 0) {
		src.x = 0;
		src.y = 0;
		src.w = size.x;
		src.h = size.y;

		dst.x = entity.pos.x - scroll_factor_x;
		dst.y = entity.pos.y + scroll_factor_y;
		dst.w = entity.width;
		dst.h = entity.height;

		entity_texture = entity.get_texture();
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

		entity_texture = entity.get_texture();
	}

	SDL_RenderCopy(renderer, entity_texture, &src, &dst);
}

void RenderWindow::display() {
	SDL_RenderPresent(renderer);
}

void RenderWindow::quit() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void RenderWindow::render_health_bar(int x, int y, int w, int h, float percent, SDL_Color fg_color, SDL_Color bg_color) {
   SDL_Color old;
   int pw = (int)w * percent;

   SDL_GetRenderDrawColor(renderer, &old.r, &old.g, &old.g, &old.a);
   SDL_Rect bgrect = { x, y, w, h };

   SDL_SetRenderDrawColor(renderer, bg_color.r, bg_color.g, bg_color.b, bg_color.a);
   SDL_RenderFillRect(renderer, &bgrect);
   SDL_SetRenderDrawColor(renderer, fg_color.r, fg_color.g, fg_color.b, fg_color.a);
   SDL_Rect fgrect = { x, y, pw, h };

   SDL_RenderFillRect(renderer, &fgrect);
   SDL_SetRenderDrawColor(renderer, old.r, old.g, old.b, old.a);

   TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf", 20);
   if (font == NULL) {
	  std::cout << "Failed to load font." << std::endl;
	  // std::exit(1);
   }

   SDL_Color color = { 255, 255, 255 };

   // Display Text "Health" above bar
   SDL_Surface* surface_message = TTF_RenderText_Solid(font, "Health", color);
   SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surface_message);
   SDL_Rect message_rect = { x + 10, y - 20, 80, 20};
   SDL_RenderCopy(renderer, message, NULL, &message_rect);
   TTF_CloseFont(font);

   // Display Health Percentage in bar
   int num_chars = 1 == (int)percent ? 3 : 2;
   num_chars = percent < 0.1f ? 1 : num_chars;

   font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf", 12);
   std::string percent_string = std::to_string((int)(percent * 100));
   surface_message = TTF_RenderText_Solid(font, percent_string.c_str(), color);
   message = SDL_CreateTextureFromSurface(renderer, surface_message);
   message_rect = { 
	   x + (w / 2) - 4 * num_chars, 
	   y + 2, 
	   8 * num_chars, 
	   16
   };
   SDL_RenderCopy(renderer, message, NULL, &message_rect);

   SDL_FreeSurface(surface_message);
   SDL_DestroyTexture(message);
   TTF_CloseFont(font);
}
