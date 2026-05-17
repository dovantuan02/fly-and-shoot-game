#ifndef FS_OBSTACLE_H
#define FS_OBSTACLE_H

#include <vector>
#include "stdint.h"

#include "fs_config.h"

#define FS_OBSTACLE_WIDTH	(5)
#define FS_OBSTACLE_HEIGHT	(5)

enum {
	FS_BOM_ID,
	FS_MINE_I_ID,
	FS_MINE_II_ID
};

typedef struct 
{
	uint8_t id;
	fs_game_coordinate_t coordinate;
	const unsigned char *bitmap;
	uint8_t score;
}fs_obstacle_info_t;

extern vector<fs_obstacle_info_t> fs_vec_obstacle;

#endif // FS_OBSTACLE_H