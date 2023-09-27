#pragma once

#include <string>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "math.hpp"

enum WeaponType {
	CHAINGUN,
	PISTOL
};

enum ProjectileType {
	BULLET
};

enum EnemyType {
	WALKING,
	FLYING
};

enum TerrainType {
	GRASS,
	DIRT
};

enum EntityType {
	BACKGROUND,
	PLAYER,
	ENEMY_WALKING,
	ENEMY_FLYING,
	GROUND,
	WEAPON,
	PROJECTILE,
	AMMO
};

enum CollisionType {
	TRANSPARENT,
	STATIC,
	DYNAMIC
};


class Entity {
	public:
		Entity() {}
		Entity(
				Vector2f pos, 
				Vector2f vel, 
				int width, 
				int height, 
				uint8_t entity_type,
				uint8_t collision_type,
				SDL_Texture* texture,
				float angle_rad = 0.0f
				);
		Vector2f pos;
		Vector2f vel;
		int width;
		int height;
		uint8_t entity_type;
		uint8_t collision_type;

		bool on_ground = true;
		bool facing_right = true;
		bool alive = true;
		int  health = 100;
		Vector2f respawn_pos;
		Vector2f respawn_vel;
		bool overhang = true;
		int  score = 0;			// TODO: Move somewhere else.
		bool collisions[4] = {false, false, false, false};
		Vector2f distance_from_player = Vector2f {0.0f, 0.0f};
		float angle_rad = 0.0f;
		SDL_Texture* texture;
		bool reload = false;

		bool is_collidable() {
			return this->collision_type != TRANSPARENT;
		}
}; 

struct PlayerEntity {
	PlayerEntity() {}
	PlayerEntity(
			Vector2f pos, 
			Vector2f vel, 
			int width, 
			int height, 
			SDL_Texture* texture,
			float angle_rad = 0.0f
			) :
		pos(pos),
		vel(vel),
		width(width),
		height(height),
		texture(texture),
		angle_rad(angle_rad) {
			respawn_pos = pos;
			respawn_vel = vel;
		};
	Vector2f pos;
	Vector2f vel;
	int width;
	int height;
	uint8_t collision_type = DYNAMIC;
	SDL_Texture* texture;
	bool alive = true;

	bool collisions[4] = {false, false, false, false};
	bool on_ground = true;
	bool facing_right = true;
	int  health = 100;
	float angle_rad = 0.0f;
	Vector2f respawn_pos;
	Vector2f respawn_vel;
	bool is_collidable = true;
	uint8_t active_weapon_id = PISTOL;
};


struct EnemyEntity {
	EnemyEntity(
			Vector2f pos, 
			Vector2f vel, 
			int width, 
			int height, 
			uint8_t enemy_id,
			SDL_Texture* texture,
			float angle_rad = 0.0f
			) :
		pos(pos),
		vel(vel),
		width(width),
		height(height),
		enemy_id(enemy_id),
		texture(texture),
		angle_rad(angle_rad) {
			respawn_pos = pos;
			respawn_vel = vel;
		};
	Vector2f pos;
	Vector2f vel;
	int width;
	int height;
	uint8_t enemy_id;
	SDL_Texture* texture;
	bool alive = true;

	bool collisions[4] = {false, false, false, false};
	bool on_ground = true;
	bool facing_right = true;
	int  health = 100;
	float angle_rad = 0.0f;
	Vector2f respawn_pos;
	Vector2f respawn_vel;
	Vector2f distance_from_player = Vector2f {0.0f, 0.0f};
	bool overhang = false;
	bool is_collidable = true;
	uint8_t collision_type = DYNAMIC;
};

struct BackgroundEntity {
	BackgroundEntity() {};
	BackgroundEntity(SDL_Texture* texture) : texture(texture) {};
	SDL_Texture* texture;
};

struct GroundEntity {
	GroundEntity(
			Vector2f pos, 
			Vector2f vel, 
			int width, 
			int height, 
			uint8_t ground_id,
			SDL_Texture* texture,
			float angle_rad = 0.0f,
			bool is_collidable = true
			) :
		pos(pos),
		vel(vel),
		width(width),
		height(height),
		ground_id(ground_id),
		texture(texture),
		angle_rad(angle_rad),
		is_collidable(is_collidable) {};
	Vector2f pos;
	Vector2f vel;
	int width;
	int height;
	uint8_t ground_id;
	uint8_t collision_type = STATIC;
	SDL_Texture* texture;
	float angle_rad = 0.0f;

	bool alive = true;
	bool is_collidable = true;
};


struct ProjectileEntity {
	ProjectileEntity() : texture(NULL) {};
	ProjectileEntity(
			Vector2f pos, 
			Vector2f vel, 
			int width, 
			int height, 
			uint8_t projectile_id,
			float damage,
			SDL_Texture* texture,
			float angle_rad = 0.0f
			) :
		pos(pos),
		vel(vel),
		width(width),
		height(height),
		projectile_id(projectile_id),
		damage(damage),
		texture(texture),
		angle_rad(angle_rad) {};
	Vector2f pos;
	Vector2f vel;
	int width;
	int height;
	uint8_t projectile_id;
	float damage;
	SDL_Texture* texture;
	float angle_rad    = 0.0f;
	bool alive 	   	   = true;
	bool is_collidable = true;
};

struct WeaponEntity {
	WeaponEntity() {};
	WeaponEntity(
			Vector2f pos, 
			Vector2f vel, 
			int width, 
			int height, 
			uint8_t weapon_id,
			uint8_t projectile_id,
			std::string name,
			unsigned int ammo,
			unsigned int max_ammo,
			unsigned int rounds_per_second,
			float damage_per_round,
			SDL_Texture* texture,
			float angle_rad = 0.0f
			) :
		pos(pos),
		vel(vel),
		width(width),
		height(height),
		weapon_id(weapon_id),
		projectile_id(projectile_id),
		name(name),
		ammo(ammo),
		max_ammo(max_ammo),
		rounds_per_second(rounds_per_second),
		damage_per_round(damage_per_round),
		texture(texture),
		angle_rad(angle_rad) {
			fps_cntr = 0;
		};
	Vector2f pos;
	Vector2f vel;
	int width;
	int height;
	uint8_t weapon_id;
	uint8_t projectile_id;
	std::string name;
	unsigned int ammo;
	unsigned int max_ammo;
	unsigned int rounds_per_second;
	float damage_per_round;
	SDL_Texture* texture;
	float angle_rad = 0.0f;
	int fps_cntr 	= 0;

	bool is_collidable = true;
	bool alive 	       = true;


	ProjectileEntity fire(int adjusted_mouse_x, int adjusted_mouse_y);
};


struct PickupEntity {
	PickupEntity(
			Vector2f pos, 
			Vector2f vel, 
			int width, 
			int height, 
			uint8_t pickup_id,
			SDL_Texture* texture,
			float angle_rad = 0.0f
			) :
		pos(pos),
		vel(vel),
		width(width),
		height(height),
		pickup_id(pickup_id),
		texture(texture),
		angle_rad(angle_rad) {};
	Vector2f pos;
	Vector2f vel;
	int width;
	int height;
	uint8_t pickup_id;
	SDL_Texture* texture;
	float angle_rad    = 0.0f;
	bool alive 	   	   = true;
	bool is_collidable = true;
	uint8_t collision_type = STATIC;
};
