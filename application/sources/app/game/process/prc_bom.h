#ifndef PRC_BOM_H
#define PRC_BOM_H

#include <vector>
#include "stdint.h"

#include "screen_infor.h"

using namespace std;

typedef struct
{
    int8_t x;
    int8_t y;
}bom_infor_t;

extern vector<bom_infor_t> v_bom_infor;
#endif