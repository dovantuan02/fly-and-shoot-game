#ifndef FS_PLANE_H
#define FS_PLANE_H

#include "app.h"

#include "fs_config.h"

typedef struct 
{
    fs_game_coordinate_t coordinate;
    fs_game_icon_state_t state;
} fs_plane_infor_t;

extern fs_plane_infor_t fs_plane;

#endif  //FS_PLANE_H