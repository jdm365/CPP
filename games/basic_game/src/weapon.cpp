#include <cmath>
#include <vector>

#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/entity.hpp"
#include "../include/weapon.hpp"


void Weapon::fire(
		Entity& weapon_entity, 
		std::vector<Entity>& projectile_entities,
		int adjusted_mouse_x, 
		int adjusted_mouse_y,
		Textures& textures
		) {
	// adjusted_mouse_x = mouse_x + scroll_factor_x;
	// adjusted_mouse_y = mouse_y - scroll_factor_y;

	if (fps_cntr == 0 && ammo > 0) {
		Vector2f barrel_pos = {
			weapon_entity.pos.x + weapon_entity.width - 64.0f,
			weapon_entity.pos.y + (weapon_entity.height / 2.0f) - 8.0f
		};

		float mouse_distance_x = adjusted_mouse_x - barrel_pos.x;
		float mouse_distance_y = adjusted_mouse_y - barrel_pos.y;
		float mouse_angle = std::atan2(mouse_distance_y, mouse_distance_x);

		Vector2f vel = {
			32.0f * std::cos(mouse_angle),
			32.0f * std::sin(mouse_angle)
		};

		projectile_entities.emplace_back(
				barrel_pos,
				vel,
				12,
				12,
				PROJECTILE,
				DYNAMIC,
				textures.projectile_textures[projectile_type],
				mouse_angle
		);
		
		ammo--;
		fps_cntr = (int)(60 / rounds_per_second);
	}
	else if (fps_cntr > 0) {
		fps_cntr--;
	}
}
