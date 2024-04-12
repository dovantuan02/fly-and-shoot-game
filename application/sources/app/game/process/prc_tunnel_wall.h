#ifndef PRC_MAP_H
#define PRC_MAP_H
#include <vector>
#include "stdint.h"

#include "screen_infor.h"

using namespace std;

#define NUM_WALL         (2)

enum ver_wall_t
{
    WALL_I = 0,
    WALL_II
};

typedef struct 
{
    int16_t x;
    int8_t y;
    enum ver_wall_t ver;
}wall_infor_t;

extern vector<vector<uint8_t>> v_limit_y;
extern vector<wall_infor_t> v_wall;

#endif
