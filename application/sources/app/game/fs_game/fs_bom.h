#ifndef FS_BOM_H
#define FS_BOM_H

#include <vector>
#include <stdint.h>

#include "fs_config.h"

using namespace std;

typedef struct {
    fs_game_coordinate_t coordinate;
} fs_bom_info_t;

extern vector<fs_bom_info_t> fs_vec_bom;

#endif  //FS_BOM_H