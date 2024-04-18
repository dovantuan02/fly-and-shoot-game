#ifndef FS_EXPLOSION_H
#define FS_EXPLOSION_H

#include <vector>

#include "fs_config.h"
using namespace std;

enum ver_infor_t {
    VER_I = 0,
    VER_II
};

typedef struct 
{
    fs_game_coordinate_t coordinate;
    ver_infor_t ver;
} fs_explosion_infor_t;


extern vector<fs_explosion_infor_t> fs_vec_explosion;
extern fs_explosion_infor_t fs_explosion;

#endif  //FS_EXPLOSION_H