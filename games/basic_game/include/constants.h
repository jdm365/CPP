#ifndef CONSTANTS_H
#define CONSTANTS_H

// Textures
#define GRASS_FILEPATH   			"assets/grass_texture.png"
#define DIRT_FILEPATH   			"assets/dirt_texture.jpg"
#define SKY_FILEPATH  				"assets/sky_texture.jpg"
#define NAMEK_FILEPATH  			"assets/namek_texture.png"
#define ROCKET_FILEPATH 			"assets/rocket_texture.jpg"
// #define PLAYER_FILEPATH		  		"assets/knight_texture.png"
#define PLAYER_FILEPATH		  		"assets/pixil-frame-0.png"
#define LEVEL_DESIGN_FILEPATH  		"assets/level_design.csv"
#define KRISTIN_JUMP_FILEPATH		"assets/kristin_goggles.jpg"
#define KRISTIN_MOUSTACHE_FILEPATH	"assets/kristin_moustache.jpg"

// Frame sizing
#define PLAYER_WIDTH_SRC		42
#define PLAYER_HEIGHT_SRC		42
#define GROUND_SIZE  			32
#define WINDOW_WIDTH  			1280
#define WINDOW_HEIGHT  			720
#define PADDING  				(int)WINDOW_WIDTH * 0.1

// Animation factors
#define TIME_STEP  				1.0f / 60.0f
#define PLAYER_SPEED			8
#define JUMP_SPEED				16
#define GRAVITY					(float)0.60f

// Enemy behavior
#define ENEMY_DPF				2		// Damage per frame

#endif
