#ifndef PRC_MINE_H
#define PRC_MINE_H

#include <stdint.h>
#include <vector>

#include "fs_config.h"

using namespace std;

typedef struct 
{
    int8_t x;
    int8_t y;
    uint8_t ver;
    fs_game_icon_state_t state;
}fs_game_mine_infor_t;

extern vector<fs_game_mine_infor_t> fs_vec_mine;
#endif