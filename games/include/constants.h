#ifndef CONSTANTS_H
#define CONSTANTS_H

// Textures
#define GRASS_FILEPATH   		"textures/grass_texture.png"
#define DIRT_FILEPATH   		"textures/dirt_texture.jpg"
#define SKY_FILEPATH  			"textures/sky_texture.jpg"
#define NAMEK_FILEPATH  		"textures/namek_texture.png"
#define PLAYER_FILEPATH_RIGHT  	"textures/knight_texture.png"
#define PLAYER_FILEPATH_LEFT  	"textures/knight_texture_left.png"

// Frame sizing
#define PLAYER_SIZE  			96
#define GROUND_SIZE  			32
#define WINDOW_WIDTH  			1280
#define WINDOW_HEIGHT  			720
#define PLATFORM_HEIGHT  		WINDOW_HEIGHT * 2 / 3
#define GROUND_HEIGHT  			int(PLATFORM_HEIGHT - 0.78 * PLAYER_SIZE)

// Animation factors
#define TIME_STEP  				0.04f
#define BUFFER_FACTOR    		int(80 * TIME_STEP)	
#define PLAYER_SPEED			16
#define JUMP_SPEED				24
#define GRAVITY					2	

#endif
