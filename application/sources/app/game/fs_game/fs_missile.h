#ifndef PRC_MISSLE_H
#define PRC_MISSLE_H
#include "stdint.h"
#include <vector>

#include "fs_config.h"

using namespace std;

/*
* MAX MISSLE -> GET FROM GAME SETTING MISSLE (1 -> 5)
*/
#define FS_MAX_MISSLE (fs_game_setting.fs_setting_missle)

class  fs_game_missile_infor_t
{
    private:        
    public:
        int8_t x, y;
        fs_game_icon_state_t state;
        fs_game_missile_infor_t(uint8_t x, uint8_t y, fs_game_icon_state_t state)
        {   
            this->x = x;
            this->y = y;
            this->state = state;
        }
};

extern vector<fs_game_missile_infor_t> fs_vec_missile;
#endif