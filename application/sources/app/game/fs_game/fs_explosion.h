#ifndef FS_EXPLOSION_H
#define FS_EXPLOSION_H

#include <vector>

#include "fs_config.h"
using namespace std;

typedef enum  {
    FS_EXPLOSION_VER_I = 0,
    FS_EXPLOSION_VER_II
} fs_ver_info_t;

typedef struct {
    fs_game_coordinate_t coordinate;
    fs_ver_info_t ver;
} fs_explosion_info_t;

extern vector<fs_explosion_info_t> fs_vec_explosion;
extern fs_explosion_info_t fs_explosion;

#endif  //FS_EXPLOSION_H