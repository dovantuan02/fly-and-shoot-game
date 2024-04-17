#ifndef FS_BOM_H
#define FS_BOM_H

#include <vector>
#include <stdint.h>

#include "fs_config.h"

using namespace std;

typedef struct
{
    int8_t x;
    int8_t y;
}fs_game_bom_infor_t;

extern vector<fs_game_bom_infor_t> fs_vec_bom;
#endif