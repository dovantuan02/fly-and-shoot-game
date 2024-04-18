#ifndef FS_MINE_H
#define FS_MINE_H

#include <stdint.h>
#include <vector>

#include "fs_config.h"

using namespace std;

typedef struct 
{
    fs_game_coordinate_t coordinate;
    uint8_t ver;
    fs_game_icon_state_t state;
}fs_mine_infor_t;

extern vector<fs_mine_infor_t> fs_vec_mine;

#endif  //FS_MINE_H