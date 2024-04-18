#ifndef FS_TUNNEL_WALL_H
#define FS_TUNNEL_WALL_H
#include <vector>
#include "stdint.h"

#include "fs_config.h"

using namespace std;

#define NUM_WALL         (2)

enum ver_wall_t
{
    FS_WALL_I = 0,
    FS_WALL_II
};

typedef struct 
{
    int16_t x;
    int8_t y;
    enum ver_wall_t ver;
}fs_wall_infor_t;

extern vector<vector<uint8_t>> fs_vec_limit_wall_y;
extern vector<fs_wall_infor_t> fs_vec_wall;

#endif  //FS_TUNNEL_WALL_H
