#include "app.h"
#include "app_dbg.h"
#include "task_display.h"

#include "fs_bom.h"
#include "fs_tunnel_wall.h"

#include "scr_bitmap.h"

/***********************************************************
* VARIABLE PROCESS BOM
***********************************************************/

/*
* max bom -> get from game setting mode (easy = 2, normal = 3, hard = 4)
*/
#define MAX_BOM           (fs_game_setting.fs_setting_game_mode)

#define BOM_MOVE_X        (4)

vector<fs_game_bom_infor_t> fs_vec_bom;        // variable bom managerment

// clear all bom available
static inline void fs_game_bom_reset() {
    // APP_DBG("MAX BOM : %d\n", MAX_BOM);
    if (!fs_vec_bom.empty()) {
        fs_vec_bom.clear();
        fs_vec_bom.shrink_to_fit();
    }
}

// add bom with top and bot limmit
static inline void fs_game_bom_push() {
    if (fs_vec_bom.size() < MAX_BOM) {
        if (!fs_vec_limit_wall_y.empty()) {
            uint8_t bot_limmit = MAP_HEIGHT - fs_vec_limit_wall_y[1][fs_vec_limit_wall_y[1].size() - 1] - (10);  // get bot limmit
            uint8_t top_limmit =              fs_vec_limit_wall_y[0][fs_vec_limit_wall_y[0].size() - 1] + (10);  // get top limmit

            fs_game_bom_infor_t temp_bom;

            temp_bom.x = LCD_WIDTH;
            // set coordinate for bom
            temp_bom.y = rand() % (bot_limmit - top_limmit + 1) + top_limmit;
            // add bom to array bom managerment
            fs_vec_bom.push_back(temp_bom);
        }
    }
}

// move all bom to left screen
static inline void fs_game_bom_move() {
    if (!fs_vec_bom.empty()) {
        for (size_t i = 0; i < fs_vec_bom.size(); i++) {  // scan all bom
            fs_vec_bom[i].x -= BOM_MOVE_X;  // move coordinate x bom
            if (fs_vec_bom[i].x < 0) {      // check bom x < 0 ?
                fs_vec_bom.erase(fs_vec_bom.begin() - i);  // erase bom
            }
        }
    }
}

/***********************************************************
* PROCESS BOM HANDLE
***********************************************************/

void task_fs_bom_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SIG_BOM_RESET: {
            APP_DBG_SIG("SIG_BOM_RESET\n");
            fs_game_bom_reset();
            break;
        }
        case SIG_BOM_PUSH: {
            APP_DBG_SIG("SIG_BOM_PUSH\n");
            fs_game_bom_push();
            break;
        }
        case SIG_BOM_MOVE: {
            // APP_DBG_SIG("SIG_BOM_MOVE\n");
            fs_game_bom_move();
            break;
        }
        case SIG_BOM_ON_TICK: {
            task_post_pure_msg(AC_TASK_BOM_ID, SIG_BOM_MOVE);
            break;
        }
        default:
            break;
    }
}