#pragma once

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

struct Entities;

#include "render_window.hpp"
#include "entity.hpp"
#include "constants.h"
#include "entity_manager.hpp"
#include "math.hpp"


void init_window(
		std::string title,
		SDL_Window** window, 
		SDL_Renderer** renderer
		);
SDL_Texture* load_texture(SDL_Renderer* renderer, std::string filepath);
void clear_window(SDL_Renderer* renderer);
int get_sprite_index(bool is_standing_still, const int frame_idx);
void update_scroll_factors(
		Vector2i& scroll_factors, 
		Vector2f& player_pos, 
		int level_width
		);
void render_entity(
		SDL_Renderer* renderer, 
		Entity& entity,
		const Vector2i& scroll_factors,
		const int frame_idx
		);
void render_all(
		SDL_Renderer* renderer, 
		Entities& entities,
		const Vector2i& scroll_factors,
		const int frame_idx
		);
void display(SDL_Renderer* renderer);
void quit(SDL_Window* window);
void tick(int& last_time, int& frame_idx); 
void render_health_bar(
		SDL_Renderer* renderer,
		float percent, 
		int x = WINDOW_WIDTH - 120,
		int y = 50, 
		int w = 100, 
		int h = 20, 
		SDL_Color fg_color = {255, 0, 0, 255},
		SDL_Color bg_color = {0, 0, 0, 255}
		);
void render_score(SDL_Renderer* renderer, int score);
void center_message(SDL_Renderer* renderer, std::string text);
void weapon_message(
		SDL_Renderer* renderer, 
		std::string& weapon_name, 
		int ammo
		);

// Statically load all textures into array
// Use load_texture() to load textures
extern SDL_Texture* background_texture;
extern SDL_Texture* player_texture;
extern SDL_Texture* terrain_textures[2];
extern SDL_Texture* enemy_textures[2];
extern SDL_Texture* weapon_textures[1];
extern SDL_Texture* projectile_textures[1];
extern SDL_Texture* ammo_texture;

void load_textures(SDL_Renderer* renderer);


static const Vector2f PLAYER_RIGHT_SPRITE_SHEET_POSITIONS[10] = {
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
