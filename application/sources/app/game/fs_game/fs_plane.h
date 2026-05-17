#ifndef FS_PLANE_H
#define FS_PLANE_H

#include "app.h"

#include "fs_config.h"

typedef struct {
	bool visible;
	fs_game_coordinate_t coordinate;
} fs_plane_info_t;

extern fs_plane_info_t fs_plane;

#endif  //FS_PLANE_H