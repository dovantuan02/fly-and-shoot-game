#ifndef PRC_EXPLOSION_H
#define PRC_EXPLOSION_H

#include <vector>

using namespace std;

enum ver_infor_t {
    VER_I = 0,
    VER_II
};

typedef struct 
{
    int8_t x;
    int8_t y;
    ver_infor_t ver;
} explosion_infor_t;


extern vector<explosion_infor_t> v_explosion;
extern explosion_infor_t explosion;
#endif