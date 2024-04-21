#ifndef FS_MISSLE_H
#define FS_MISSLE_H
#include "stdint.h"
#include <vector>

#include "fs_config.h"

using namespace std;

/*
* MAX MISSLE -> GET FROM GAME SETTING MISSLE (1 -> 5)
*/
#define FS_MAX_MISSLE (fs_game_setting.fs_setting_missle)

class fs_missile_info_t {
   private:
   public:
    bool visible;
    fs_game_coordinate_t coordinate;
    fs_missile_info_t(uint8_t x, uint8_t y, bool visible) {
        this->coordinate.x = x;
        this->coordinate.y = y;
        this->visible = visible;
    }
};

extern vector<fs_missile_info_t> fs_vec_missile;

#endif  //FS_MISSLE_H