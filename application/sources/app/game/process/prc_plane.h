#ifndef PRC_PLANE_H
#define PRC_PLANE_H
#include "screen_infor.h"
#include "app.h"

typedef struct 
{
    int8_t x;
    int8_t y;
    icon_state_t state;
} plane_infor_t;

extern plane_infor_t plane;
#endif