#ifndef PRC_MISSLE_H
#define PRC_MISSLE_H
#include "stdint.h"
#include <vector>

#include "screen_infor.h"

using namespace std;

/*
* MAX MISSLE -> GET FROM GAME SETTING MISSLE (1 -> 5)
*/
#define MAX_MISSLE (game_setting.max_missle)

class  missile
{
    private:        
    public:
        int8_t x, y;
        icon_state_t state;
        missile(uint8_t x, uint8_t y, icon_state_t state)
        {   
            this->x = x;
            this->y = y;
            this->state = state;
        }
};

extern vector<missile> v_missile;
#endif