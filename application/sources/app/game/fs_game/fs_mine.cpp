#include <vector>

#include "app.h"
#include "timer.h"
#include "task_list.h"
#include "task_display.h"

#include "fs_bom.h"
#include "fs_tunnel_wall.h"
#include "fs_explosion.h"
#include "fs_mine.h"
#include "fs_config.h"

#include "fs_bitmap.h"
/***********************************************************
* VARIABLE PROCESS EXPLOSION
***********************************************************/
#define FS_MINE_MOVE_X  (4)

/*
* max bom -> get from game setting mode (easy = 2, normal = 3, hard = 4)
*/
#define FS_MAX_MINE    (fs_game_setting.fs_setting_game_mode + 3)

/*
* fs_vec_mine : VECTOR MINE MANAGERMENT
*/
vector<fs_mine_infor_t> fs_vec_mine;

// clear all mine
static inline void fs_game_mine_reset() {
    if (fs_vec_mine.size() != 0) {
        fs_vec_mine.clear();
        fs_vec_mine.shrink_to_fit();
    }
}

// add mine to mine managerment
static inline void fs_game_mine_push() {
    if (static_cast<int>(fs_vec_mine.size()) <= FS_MAX_MINE) {
        // get bot y limmit 
        uint8_t bot_limmit = MAP_HEIGHT - fs_vec_limit_wall_y[1][fs_vec_limit_wall_y[1].size() - 1] - (10);
        // get top y limmit
        uint8_t top_limmit =              fs_vec_limit_wall_y[0][fs_vec_limit_wall_y[0].size() - 1] + (10);

        fs_mine_infor_t temp_mine;

        temp_mine.coordinate.x = LCD_WIDTH;
        // set mine y range (top y limmit - bot y limmit)
        temp_mine.coordinate.y = rand() % (bot_limmit - top_limmit + 1) + top_limmit; 
        temp_mine.state = FS_SHOW;
        temp_mine.ver = rand() % 2;                   // random mine 
        fs_vec_mine.push_back(temp_mine);             // add mine to mine managerment

        if (!fs_vec_bom.empty()) {             
            for (size_t i = 0; i < fs_vec_bom.size(); i++) {           // scan all bom
                /* 
                *   check all bom with mine
                *   if bom(x,y) equal mine(x,y) ->? erase 
                */
                if (temp_mine.coordinate.x == fs_vec_bom[i].coordinate.x && abs(temp_mine.coordinate.y - fs_vec_bom[i].coordinate.y) < 10) {    
                    fs_vec_mine.pop_back();
                }
            }
        }
    }
}

//  move all mine
static inline void fs_game_mine_move() {
    if (!fs_vec_mine.empty()) {
        for (size_t i = 0; i < fs_vec_mine.size(); i++) {            // scan all mine
            if (fs_vec_mine[i].coordinate.x > 0) {                              // mine(x)  bigger min lcd(x) 
                fs_vec_mine[i].coordinate.x -= FS_MINE_MOVE_X;                     // move mine x
                continue;
            }
            fs_vec_mine.erase(fs_vec_mine.begin() + i);             // erase mine
        }
    }
}

/***********************************************************
* PROCESS MINE HANDLE
***********************************************************/

void task_fs_mine_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case FS_GAME_MINE_RESET_SIG: {
            fs_game_mine_reset();
            break;
        }

        case FS_GAME_MINE_PUSH_SIG: {
            fs_game_mine_push();
            break;
        }

        case FS_GAME_MINE_MOVE_SIG: {
            fs_game_mine_move();
            break;
        }
        case FS_GAME_MINE_ON_TICK_SIG: {
            task_post_pure_msg(FS_GAME_TASK_MINE_ID, FS_GAME_MINE_MOVE_SIG);
            break;
        }
        default:
            break;
    }
}