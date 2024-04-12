#ifndef PRC_MISSLE_H
#define PRC_MISSLE_H
#include "stdint.h"
#include <vector>

#include "screen_infor.h"

using namespace std;

#define MAX_MISSLE (game_setting.max_missle)

class  missle
{
    private:        
    public:
        int8_t x, y;
        icon_state_t state;
        missle(uint8_t x, uint8_t y, icon_state_t state)
        {   
            this->x = x;
            this->y = y;
            this->state = state;
        }
};

extern vector<missle> v_missle;
#endif