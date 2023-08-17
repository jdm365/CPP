#ifndef CONSTANTS_H
#define CONSTANTS_H

// Textures
#define GRASS_FILEPATH   		"assets/grass_texture.png"
#define DIRT_FILEPATH   		"assets/dirt_texture.jpg"
#define SKY_FILEPATH  			"assets/sky_texture.jpg"
#define NAMEK_FILEPATH  		"assets/namek_texture.png"
#define ROCKET_FILEPATH 		"assets/rocket_texture.jpg"
#define PLAYER_FILEPATH_RIGHT  	"assets/knight_texture.png"
#define PLAYER_FILEPATH_LEFT  	"assets/knight_texture_left.png"
#define LEVEL_DESIGN_FILEPATH  	"assets/level_design.csv"
#define KRISTIN_FILEPATH		"assets/kristin_goggles.jpg"

// Frame sizing
#define PLAYER_WIDTH_SRC		32
#define PLAYER_HEIGHT_SRC		49
#define PLAYER_SIZE_FACTOR  	2.00f
#define GROUND_SIZE  			32
#define WINDOW_WIDTH  			1280
#define WINDOW_HEIGHT  			720
#define PLATFORM_HEIGHT  		(float)WINDOW_HEIGHT * 2.0f / 3.0f
#define PADDING  				(int)WINDOW_WIDTH * 0.1

// Animation factors
#define TIME_STEP  				1.0f / 60.0f
#define PLAYER_SPEED			8
#define JUMP_SPEED				16
#define GRAVITY					(float)0.60f

// Enemy behavior
#define ENEMY_DPF				2		// Damage per frame

#endif
