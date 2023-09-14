#include <iostream>
#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>
#include <string.h>

#include "../include/entity_manager.hpp"
#include "../include/entity.hpp"
#include "../include/constants.h"
#include "../include/render_window.hpp"

RenderWindow::RenderWindow(const char* title, int width, int height) {
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		std::cout << "SDL initialization failed." << SDL_GetError() << std::endl;
	}
	if (!(IMG_Init(IMG_INIT_PNG))) {
		std::cout << "IMG_Init failed" << SDL_GetError() << std::endl;
	}

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

	// Tint enemies color.
	if (strcmp(filepath, "assets/kristin_goggles.jpg") == 0) {
		SDL_SetTextureColorMod(texture, 205, 50, 50);
	}
	else if (strcmp(filepath, "assets/kristin_moustache.jpg") == 0) {
		SDL_SetTextureColorMod(texture, 50, 200, 255);
	}

	if (texture == NULL) {
		std::cout << "Failed to load a texture." << SDL_GetError() << std::endl;
	}

	return texture;
}


void RenderWindow::clear() {
	// Clear the window to black.
	SDL_RenderClear(renderer);
}

int RenderWindow::get_sprite_index(Vector2f& player_pos, Vector2f& player_vel) {
	scroll_factor_x = player_pos.x - (int)(WINDOW_WIDTH / 2);
	scroll_factor_y = -(player_pos.y - (int)(2 * WINDOW_HEIGHT / 5));

	scroll_factor_x = std::min(
			scroll_factor_x, 
			level_width - WINDOW_WIDTH
			);

	scroll_factor_x = std::max(scroll_factor_x, 0);
	scroll_factor_y = std::max(scroll_factor_y, -99 * GROUND_SIZE + WINDOW_HEIGHT);

	// 60 FPS
	// Complete 7 phase walking animation in 1 second.
	// 60 / 7 = 8.57 frames per walk cycle. Call it 8.
	int final_idx;

	step_idx++;
	if (player_vel.x != 0) {
		final_idx = int(step_idx / 8);
		final_idx = final_idx % 6;
	}
	else {
		final_idx = int(step_idx / 8) % 3;
		final_idx += 7;
	}
	return final_idx;
}


void RenderWindow::render_entity(Entity& entity) {
	if (!entity.alive) return;

	SDL_Point size;
	SDL_QueryTexture(entity.get_texture(), NULL, NULL, &size.x, &size.y);

	// Size and location of the sprite on the sprite sheet and dst.
	SDL_Rect src, dst;

	SDL_Texture* entity_texture = nullptr;

	if (entity.entity_type != BACKGROUND && entity.entity_type != PLAYER) {
		if (entity.pos.x > scroll_factor_x + WINDOW_WIDTH) return;
		if (entity.pos.x + entity.width < scroll_factor_x) return;
	}

	if (
			entity.entity_type == GROUND 
				|| 
			entity.entity_type == ENEMY_FLYING
				|| 
			entity.entity_type == BACKGROUND
				|| 
			entity.entity_type == PROJECTILE
			) {
		src.x = 0;
		src.y = 0;
		src.w = size.x;
		src.h = size.y;

		dst.x = entity.pos.x - scroll_factor_x * (entity.entity_type != BACKGROUND);
		dst.y = entity.pos.y + scroll_factor_y * (entity.entity_type != BACKGROUND);
		dst.w = entity.width;
		dst.h = entity.height;

		entity_texture = entity.get_texture();
	}
	else if (entity.entity_type == PLAYER) {
		int step_index = get_sprite_index(entity.pos, entity.vel);

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
		src = {(int)_src.x, (int)_src.y, PLAYER_WIDTH_SRC, PLAYER_HEIGHT_SRC - 1};

		src.w = PLAYER_WIDTH_SRC;
		src.h = PLAYER_HEIGHT_SRC - 1;

		dst.x = entity.pos.x - scroll_factor_x;
		dst.y = entity.pos.y + scroll_factor_y;
		dst.w = entity.width;
		dst.h = entity.height;
	}
	else if (entity.entity_type == ENEMY_WALKING) {
		src.x = 0;
		src.y = 0;
		src.w = size.x;
		src.h = size.y;

		dst.x = entity.pos.x - scroll_factor_x;
		dst.y = entity.pos.y + scroll_factor_y;
		dst.w = entity.width;
		dst.h = entity.height;

		entity.facing_right = entity.vel.x > 0.0f;
	}
	SDL_RenderCopyEx(
			renderer, 
			entity.get_texture(),
			&src, 
			&dst, 
			entity.angle_rad * 180 / M_PI,
			NULL, 
			entity.facing_right ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
			);
}

void RenderWindow::render_all(Entities& entities) {
	render_entity(entities.background_entity);
	for (Entity& entity: entities.walking_enemy_entities) {
		render_entity(entity);
	}

	for (Entity& entity: entities.flying_enemy_entities) {
		render_entity(entity);
	}

	for (Entity& entity: entities.ground_entities) {
		render_entity(entity);
	}

	for (Entity& entity: entities.projectile_entities) {
		render_entity(entity);
	}

	render_entity(entities.player_entity);

	for (Entity& entity: entities.weapon_entities) {
		render_entity(entity);
	}
}

void RenderWindow::display() {
	SDL_RenderPresent(renderer);
}

void RenderWindow::quit() {
	SDL_DestroyWindow(window);
	SDL_Quit();
}

void RenderWindow::tick() {
	time_elapsed = (int)SDL_GetTicks() - start_time;
	delay_time   = (int)1000 * TIME_STEP - time_elapsed;

	// If delay time is less than zero then code is updating too slowly 
	// (and is surely terrible).
	if (delay_time < 0) {
		std::cout << "Delay Time: " << delay_time << std::endl;
		std::cout << "Delay time is negative. Code is updating too slowly." << std::endl;
	}

	if (delay_time > 0) {
		SDL_Delay(delay_time);
	}
	start_time = (int)SDL_GetTicks();
}

void RenderWindow::render_health_bar(float percent, int x, int y, int w, int h, SDL_Color fg_color, SDL_Color bg_color) {
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

void RenderWindow::render_score(int score) {
   TTF_Font* font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf", 48);
   if (font == NULL) {
	  std::cout << "Failed to load font." << std::endl;
	  // std::exit(1);
   }

   SDL_Color color = { 255, 255, 255 };

   // Display Text "Score"
   SDL_Surface* surface_message = TTF_RenderText_Solid(font, "Score", color);
   SDL_Texture* message = SDL_CreateTextureFromSurface(renderer, surface_message);
   SDL_Rect message_rect = { 
	   (WINDOW_WIDTH / 2) - 25, 
	   10,
	   50,
	   20
   };
   SDL_RenderCopy(renderer, message, NULL, &message_rect);
   TTF_CloseFont(font);

   // Display Score
   int num_chars = 4;

   font = TTF_OpenFont("/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf", 36);
   std::string score_string = std::to_string(score);
   surface_message = TTF_RenderText_Solid(font, score_string.c_str(), color);
   message = SDL_CreateTextureFromSurface(renderer, surface_message);
   message_rect = { 
	   (WINDOW_WIDTH / 2) - 4 * num_chars, 
	   30, 
	   8 * num_chars, 
	   16
   };
   SDL_RenderCopy(renderer, message, NULL, &message_rect);

   SDL_FreeSurface(surface_message);
   SDL_DestroyTexture(message);
   TTF_CloseFont(font);
}


void RenderWindow::center_message(std::string text) {

   SDL_Color color = { 255, 255, 255 };

   // Display Score
   int num_chars = text.length();

   TTF_Font* font = TTF_OpenFont(
		   "/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf", 
		   64
		   );
   if (font == NULL) {
	  std::cout << "Failed to load font." << std::endl;
	  // std::exit(1);
   }

   SDL_Surface* surface_message = TTF_RenderText_Solid(font, text.c_str(), color);
   SDL_Texture* message 		= SDL_CreateTextureFromSurface(renderer, surface_message);

   // Display In Center
   const int message_width 	= 16 * num_chars;
   const int message_height = 64;
   const int message_x 		= (WINDOW_WIDTH / 2) - (message_width / 2);
   const int message_y 		= (WINDOW_HEIGHT / 2) - (message_height / 2);
   SDL_Rect  message_rect 	= { 
	   message_x, 
	   message_y, 
	   message_width, 
	   message_height
   };
   SDL_RenderCopy(renderer, message, NULL, &message_rect);

   SDL_FreeSurface(surface_message);
   SDL_DestroyTexture(message);
   TTF_CloseFont(font);
}
