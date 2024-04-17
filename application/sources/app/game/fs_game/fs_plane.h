#ifndef PRC_PLANE_H
#define PRC_PLANE_H
#include "fs_config.h"
#include "app.h"

typedef struct 
{
    int8_t x;
    int8_t y;
    fs_game_icon_state_t state;
} fs_game_plane_infor_t;

extern fs_game_plane_infor_t fs_plane;
#endif