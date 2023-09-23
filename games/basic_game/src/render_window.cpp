#include <iostream>
#include <string.h>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_ttf.h>

#include "render_window.hpp"
#include "entity.hpp"
#include "constants.h"
#include "entity_manager.hpp"
#include "weapon.hpp"

SDL_Texture* background_texture;
SDL_Texture* player_texture;
SDL_Texture* terrain_textures[2];
SDL_Texture* enemy_textures[2];
SDL_Texture* weapon_textures[1];
SDL_Texture* projectile_textures[1];
SDL_Texture* ammo_texture;

void load_textures(SDL_Renderer* renderer) {
	background_texture	  	  	= load_texture(renderer, NAMEK_FILEPATH);
	player_texture		  	  	= load_texture(renderer, PLAYER_FILEPATH);
	terrain_textures[GRASS]   	= load_texture(renderer, GRASS_FILEPATH);
	terrain_textures[DIRT]    	= load_texture(renderer, DIRT_FILEPATH);
	enemy_textures[WALKING]   	= load_texture(renderer, KRISTIN_MOUSTACHE_FILEPATH);
	enemy_textures[FLYING]    	= load_texture(renderer, KRISTIN_JUMP_FILEPATH);
	weapon_textures[CHAINGUN]   = load_texture(renderer, CHAINGUN_FILEPATH);
	projectile_textures[BULLET] = load_texture(renderer, BULLET_FILEPATH);
	ammo_texture                = load_texture(renderer, AMMO_FILEPATH);
}

// RenderWindow::RenderWindow(const char* title, int width, int height) {
void init_window(std::string title, SDL_Window** window, SDL_Renderer** renderer) {
	if (SDL_Init(SDL_INIT_VIDEO) > 0) {
		std::cout << "SDL initialization failed." << SDL_GetError() << std::endl;
	}
	if (!(IMG_Init(IMG_INIT_PNG))) {
		std::cout << "IMG_Init failed" << SDL_GetError() << std::endl;
	}

	*window = SDL_CreateWindow(
			title.c_str(),
			SDL_WINDOWPOS_UNDEFINED, 
			SDL_WINDOWPOS_UNDEFINED,
			WINDOW_WIDTH,
			WINDOW_HEIGHT,
			SDL_WINDOW_SHOWN
			);

	if (window == NULL) {
		std::cout << "Window failed to initialize. Error: ";
		std::cout << SDL_GetError() << std::endl;
	}
	TTF_Init();

	*renderer = SDL_CreateRenderer(*window, -1, SDL_RENDERER_ACCELERATED);

	if (renderer == NULL) {
		std::cout << "Renderer failed to initialize. Error: ";
		std::cout << SDL_GetError() << std::endl;
	}
}


// SDL_Texture* RenderWindow::load_texture(const char* filepath) {
SDL_Texture* load_texture(SDL_Renderer* renderer, std::string filepath) {
	SDL_Texture* texture = NULL;
	texture = IMG_LoadTexture(renderer, filepath.c_str());

	if (texture == NULL) {
		std::cout << "Failed to load a texture." << SDL_GetError() << std::endl;
	}

	// Tint enemies color.
	if (strcmp(filepath.c_str(), "assets/kristin_goggles.jpg") == 0) {
		SDL_SetTextureColorMod(texture, 205, 50, 50);
	}
	else if (strcmp(filepath.c_str(), "assets/kristin_moustache.jpg") == 0) {
		SDL_SetTextureColorMod(texture, 50, 200, 255);
	}
	return texture;
}


// void RenderWindow::clear() {
void clear_window(SDL_Renderer* renderer) {
	SDL_RenderClear(renderer);
	SDL_SetRenderDrawColor(renderer, 0, 0, 0, 255);
}

// int RenderWindow::get_sprite_index(Vector2f& player_pos, Vector2f& player_vel) {
int get_sprite_index(bool is_standing_still, const int frame_idx) {
	// 60 FPS
	// Complete 7 phase walking animation in 1 second.
	// 60 / 7 = 8.57 frames per walk cycle. Call it 8.
	int sprite_idx;

	if (!is_standing_still) {
		sprite_idx = (int)(frame_idx / 8) % 6;
	}
	else {
		// Offset to the standing still sprite.
		sprite_idx = 7 + ((int)(frame_idx / 8) % 3);
	}
	return sprite_idx;
};

void update_scroll_factors(
		Vector2i& scroll_factors,
		Vector2f& player_pos, 
		int level_width
		) {
	scroll_factors = {
		(int)player_pos.x - (int)(WINDOW_WIDTH * 0.5f),
		(int)(0.4f * WINDOW_HEIGHT) - (int)player_pos.y
	};

	// Clamp scroll factors.
	scroll_factors.x = std::min(
			scroll_factors.x,
			level_width - WINDOW_WIDTH
			);

	// Don't scroll past the left edge.
	scroll_factors.x = std::max(scroll_factors.x, 0);
	scroll_factors.y = std::max(scroll_factors.y, -99 * GROUND_SIZE + WINDOW_HEIGHT);
}


// void RenderWindow::render_entity(Entity& entity) {
void render_entity(
		SDL_Renderer* renderer, 
		Entity& entity, 
		const Vector2i& scroll_factors,
		const int frame_idx
		) {
	if (!entity.alive) return;

	SDL_Point size;
	SDL_QueryTexture(entity.texture, NULL, NULL, &size.x, &size.y);

	// Size and location of the sprite on the sprite sheet and dst.
	SDL_Rect src, dst;

	if (entity.entity_type != BACKGROUND && entity.entity_type != PLAYER) {
		if (entity.pos.x > scroll_factors.x + WINDOW_WIDTH) return;
		if (entity.pos.x + entity.width < scroll_factors.x) return;
	}

	if (
			entity.entity_type == GROUND 
				|| 
			entity.entity_type == ENEMY_FLYING
				|| 
			entity.entity_type == BACKGROUND
				|| 
			entity.entity_type == PROJECTILE
				||
			entity.entity_type == WEAPON 
				||
			entity.entity_type == AMMO 
			) {
		src.x = 0;
		src.y = 0;
		src.w = size.x;
		src.h = size.y;

		dst.x = entity.pos.x - scroll_factors.x * (entity.entity_type != BACKGROUND);
		dst.y = entity.pos.y + scroll_factors.y * (entity.entity_type != BACKGROUND);
		dst.w = entity.width;
		dst.h = entity.height;
	}
	else if (entity.entity_type == PLAYER) {
		int step_index = get_sprite_index((bool)(entity.vel.x == 0), frame_idx);

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

		dst.x = entity.pos.x - scroll_factors.x;
		dst.y = entity.pos.y + scroll_factors.y;
		dst.w = entity.width;
		dst.h = entity.height;
	}
	else if (entity.entity_type == ENEMY_WALKING) {
		src.x = 0;
		src.y = 0;
		src.w = size.x;
		src.h = size.y;

		dst.x = entity.pos.x - scroll_factors.x;
		dst.y = entity.pos.y + scroll_factors.y;
		dst.w = entity.width;
		dst.h = entity.height;

		entity.facing_right = entity.vel.x > 0.0f;
	}
	SDL_RenderCopyEx(
			renderer, 
			entity.texture,
			&src, 
			&dst, 
			entity.angle_rad * 180 / M_PI,
			NULL, 
			entity.facing_right ? SDL_FLIP_NONE : SDL_FLIP_HORIZONTAL
			);
}

// void RenderWindow::render_all(Entities& entities) {
void render_all(
		SDL_Renderer* renderer, 
		Entities& entities, 
		const Vector2i& scroll_factors, 
		const int frame_idx
		) {
	render_entity(
			renderer,
			entities.background_entity,
			scroll_factors,
			frame_idx
			);
	for (Entity& entity: entities.walking_enemy_entities) {
		render_entity(
				renderer,
				entity,
				scroll_factors,
				frame_idx
				);
	}

	for (Entity& entity: entities.flying_enemy_entities) {
		render_entity(
				renderer,
				entity,
				scroll_factors,
				frame_idx
				);
	}

	for (Entity& entity: entities.ground_entities) {
		render_entity(
				renderer,
				entity,
				scroll_factors,
				frame_idx
				);
	}

	for (Entity& entity: entities.projectile_entities) {
		render_entity(
				renderer,
				entity,
				scroll_factors,
				frame_idx
				);
	}

	render_entity(
			renderer,
			entities.player_entity,
			scroll_factors,
			frame_idx
			);

	for (Entity& entity: entities.weapon_entities) {
		render_entity(
				renderer,
				entity,
				scroll_factors,
				frame_idx
				);
	}

	for (Entity& entity: entities.ammo_entities) {
		render_entity(
				renderer,
				entity,
				scroll_factors,
				frame_idx
				);
	}
}

// void RenderWindow::display() {
void display(SDL_Renderer* renderer) {
	SDL_RenderPresent(renderer);
}

// void RenderWindow::quit() {
void quit(SDL_Window* window) {
	SDL_DestroyWindow(window);
	SDL_Quit();
}

// void RenderWindow::tick() {
void tick(int& last_time, int& frame_idx) {
	int time_elapsed = (int)SDL_GetTicks() - last_time;
	int delay_time   = (int)1000 / FPS - time_elapsed;
	// std::cout << "Possible FPS: " << 1000 / time_elapsed << std::endl;

	if (delay_time < 0) {
		std::cout << "FPS: " << 1000 / time_elapsed << std::endl;
		std::cout << "Delay time is negative. Code is updating too slowly." << std::endl;
	}

	if (delay_time > 0) {
		SDL_Delay(delay_time);
	}
	last_time = (int)SDL_GetTicks();
	frame_idx = (frame_idx + 1) % FPS;
}

// void RenderWindow::render_health_bar(float percent, int x, int y, int w, int h, SDL_Color fg_color, SDL_Color bg_color) {
void render_health_bar(
		SDL_Renderer* renderer,
		float percent, 
		int x, 
		int y, 
		int w, 
		int h, 
		SDL_Color fg_color, 
		SDL_Color bg_color
		) {
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

// void RenderWindow::render_score(int score) {
void render_score(SDL_Renderer* renderer, int score) {
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


// void RenderWindow::center_message(std::string text) {
void center_message(SDL_Renderer* renderer, std::string text) {

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

// void RenderWindow::weapon_message(std::string& weapon_name, int ammo) {
void weapon_message(SDL_Renderer* renderer, std::string& weapon_name, int ammo) {

   SDL_Color color = { 255, 255, 255 };

   // Display Score
   std::string weapon_name_text = "Weapon: " + weapon_name;
   std::string ammo_text 		= "Ammo: " + std::to_string(ammo);
   int num_chars_0 = weapon_name_text.length();
   int num_chars_1 = ammo_text.length();

   TTF_Font* font = TTF_OpenFont(
		   "/usr/share/fonts/truetype/freefont/FreeMonoBold.ttf", 
		   24
		   );
   if (font == NULL) {
	  std::cout << "Failed to load font." << std::endl;
	  // std::exit(1);
   }

   SDL_Surface* surface_message_0 = TTF_RenderText_Solid(font, weapon_name_text.c_str(), color);
   SDL_Surface* surface_message_1 = TTF_RenderText_Solid(font, ammo_text.c_str(), color);
   SDL_Texture* message_0 		= SDL_CreateTextureFromSurface(renderer, surface_message_0);
   SDL_Texture* message_1 		= SDL_CreateTextureFromSurface(renderer, surface_message_1);

   // Display In Top Left 
   const int message_width_0	= 16 * num_chars_0;
   const int message_width_1	= 16 * num_chars_1;
   const int message_height 	= 24;
   const int message_x 			= 10;
   const int message_y_0 		= 10;
   const int message_y_1 		= 10 + message_height;

   SDL_Rect  message_rect_0	= { 
	   message_x, 
	   message_y_0, 
	   message_width_0, 
	   message_height
   };
   SDL_Rect  message_rect_1	= { 
	   message_x, 
	   message_y_1, 
	   message_width_1, 
	   message_height
   };
   SDL_RenderCopy(renderer, message_0, NULL, &message_rect_0);
   SDL_RenderCopy(renderer, message_1, NULL, &message_rect_1);

   SDL_FreeSurface(surface_message_0);
   SDL_FreeSurface(surface_message_1);
   SDL_DestroyTexture(message_0);
   SDL_DestroyTexture(message_1);
   TTF_CloseFont(font);
}
