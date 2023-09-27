#pragma once

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

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
Mix_Chunk* load_wav(std::string filepath);
Mix_Music* load_mp3(std::string filepath);
void clear_window(SDL_Renderer* renderer);
int get_sprite_index(bool is_standing_still, const int frame_idx);
void update_scroll_factors(
		Vector2i& scroll_factors, 
		Vector2f& player_pos, 
		int level_width
		);
void render_background(
		SDL_Renderer* renderer, 
		BackgroundEntity& background_entity,
		const Vector2i& scroll_factors
		);
void render_player(
		SDL_Renderer* renderer, 
		PlayerEntity& player_entity,
		const Vector2i& scroll_factors,
		const int frame_idx
		);
void render_enemy(
		SDL_Renderer* renderer, 
		EnemyEntity& enemy_entity,
		const Vector2i& scroll_factors
		);
void render_projectile(
		SDL_Renderer* renderer, 
		ProjectileEntity& projectile_entity,
		const Vector2i& scroll_factors
		);
void render_pickup(
		SDL_Renderer* renderer, 
		PickupEntity& pickup_entity,
		const Vector2i& scroll_factors
		);
void render_terrain(
		SDL_Renderer* renderer, 
		GroundEntity& ground_entity,
		const Vector2i& scroll_factors
		);
void render_weapon(
		SDL_Renderer* renderer, 
		WeaponEntity& weapon_entity,
		const Vector2i& scroll_factors
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

// Statically load all textures
// Use load_texture() to load textures
extern SDL_Texture* background_texture;
extern SDL_Texture* player_texture;
extern SDL_Texture* terrain_textures[2];
extern SDL_Texture* enemy_textures[2];
extern SDL_Texture* weapon_textures[2];
extern SDL_Texture* projectile_textures[1];
extern SDL_Texture* ammo_texture;

void load_textures(SDL_Renderer* renderer);

// Statically load all sound effects
extern Mix_Chunk* gunshot_sound;
extern Mix_Chunk* reload_sound;
extern Mix_Chunk* empty_chamber_sound;
extern Mix_Chunk* boing_sound;
extern Mix_Chunk* dying_sound;

extern Mix_Music* dark_halls;
extern Mix_Music* imps_song;

void load_sounds();

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
