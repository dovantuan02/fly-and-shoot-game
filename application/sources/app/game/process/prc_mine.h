#ifndef PRC_MINE_H
#define PRC_MINE_H

#include <stdint.h>
#include <vector>

#include "screen_infor.h"
using namespace std;

typedef struct 
{
    int8_t x;
    int8_t y;
    uint8_t ver;
    icon_state_t state;
}mine_infor_t;

extern vector<mine_infor_t> v_mine;
#endif