#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>
#include <SDL2/SDL_mixer.h>

#include "entity.hpp"
#include "render_window.hpp"
#include "constants.h"


Entity::Entity(
		Vector2f pos, 
		Vector2f vel, 
		int width,
		int height,
		uint8_t entity_type,
		uint8_t collision_type,
		SDL_Texture* texture,
		float angle_rad
		) : pos(pos), 
			vel(vel), 
			width(width), 
			height(height), 
			entity_type(entity_type),
			collision_type(collision_type),
			angle_rad(angle_rad),
			texture(texture) {

	respawn_pos = pos;
	respawn_vel = vel;
}



ProjectileEntity WeaponEntity::fire(
		int adjusted_mouse_x,
		int adjusted_mouse_y
		) {
	// adjusted_mouse_x = mouse_x + scroll_factor_x;
	// adjusted_mouse_y = mouse_y - scroll_factor_y;

	if (fps_cntr == 0 && ammo > 0) {
		Vector2f barrel_pos {
			pos.x + width - 32.0f,
			pos.y + (height / 2.0f) - 8.0f
		};

		float mouse_distance_x = adjusted_mouse_x - barrel_pos.x;
		float mouse_distance_y = adjusted_mouse_y - barrel_pos.y;
		float mouse_angle = std::atan2(mouse_distance_y, mouse_distance_x);

		Vector2f _vel = {
			32.0f * std::cos(mouse_angle),
			32.0f * std::sin(mouse_angle)
		};

		int projectile_width, projectile_height;
	
		SDL_QueryTexture(
				projectile_textures[projectile_id], 
				NULL, 
				NULL, 
				&projectile_width, 
				&projectile_height
				);

		float ratio = (float)projectile_height / (float)projectile_width;

		ProjectileEntity projectile_entity(
				barrel_pos,
				_vel,
				10,
				(int)(10 * ratio),
				projectile_id,
				damage_per_round,
				projectile_textures[projectile_id],
				mouse_angle
				);
		
		if (weapon_id != PISTOL) {
			ammo--;
		}
		fps_cntr = (int)(FPS / rounds_per_second);

		Mix_Volume(-1, MIX_MAX_VOLUME / 16);
		Mix_PlayChannel(-1, gunshot_sound, 0);

		return projectile_entity;
	}
	else if (fps_cntr > 0) {
		fps_cntr--;
		ProjectileEntity projectile_entity;
		return projectile_entity;
	}
	else if (ammo == 0) {
		fps_cntr += rounds_per_second;
		Mix_Volume(-1, MIX_MAX_VOLUME / 16);
		Mix_PlayChannel(-1, empty_chamber_sound, 0);

		ProjectileEntity projectile_entity;
		return projectile_entity;
	}
	else {
		ProjectileEntity projectile_entity;
		return projectile_entity;
	}
}
