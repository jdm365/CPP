#ifndef CONSTANTS_H
#define CONSTANTS_H

// Textures
#define GRASS_FILEPATH   		"textures/grass_texture.png"
#define DIRT_FILEPATH   		"textures/dirt_texture.jpg"
#define SKY_FILEPATH  			"textures/sky_texture.jpg"
#define NAMEK_FILEPATH  		"textures/namek_texture.png"
#define ROCKET_FILEPATH 		"textures/rocket_texture.jpg"
#define PLAYER_FILEPATH_RIGHT  	"textures/knight_texture.png"
#define PLAYER_FILEPATH_LEFT  	"textures/knight_texture_left.png"
#define LEVEL_DESIGN_FILEPATH  	"textures/level_design.csv"

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
#define TIME_STEP  				0.04f
#define PLAYER_SPEED			16
#define JUMP_SPEED				30
#define GRAVITY					2.00f	
#define ROCKET_POWER			5.00f	

#endif
