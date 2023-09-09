#include <SDL2/SDL.h>
#include <SDL2/SDL_image.h>

#include "../include/render_window.hpp"
#include "../include/entity_manager.hpp"
#include "../include/event_handler.hpp"


int main(int argc, char* args[]) {
	RenderWindow window("KRISTIN DESTROY", WINDOW_WIDTH, WINDOW_HEIGHT);

	Textures textures(&window);
	Entities entities(&textures);

	window.level_width = entities.level_width;

	SDL_Event event;
	bool done = false;

	// Game Loop
	while (!done) {
		while (SDL_PollEvent(&event)) {
			if (event.type == SDL_QUIT) {
				window.quit();
				done = true;
			}
			handle_keyboard(
					event, 
					entities,
					textures.energy_projectile_texture
					);
		}

		detect_collisions(entities.get_all_entities());

		window.clear();

		if (!entities.player_entity.alive) {
			respawn(entities.get_all_entities());
		}

		// Render all.
		window.render_entity(entities.background_entity);
		for (Entity* entity: entities.get_all_entities()) {
			window.render_entity(*entity);
		}
		window.render_health_bar(entities.player_entity.health * 0.01f);
		window.render_score(entities.player_entity.score);

		window.display();
		window.tick();
	}

	return 0;
}
