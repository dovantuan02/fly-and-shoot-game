#include <vector>

#include "app.h"
#include "task_display.h"
#include "task_list.h"

#include "fs_tunnel_wall.h"
#include "fs_plane.h"
#include "fs_config.h"

#include "fs_bitmap.h"
#include "scr_game_over.h"

using namespace std;

/***********************************************************
* VARIABLE PROCESS PLANE
***********************************************************/
#define FS_WALL_MOVE_X  (4)
#define FS_SPLIT_WALL_X (4)

/*
* fs_vec_wall : VECTOR WALL MANAGERMENT
*/
vector<fs_wall_info_t> fs_vec_wall;

/*
* fs_vec_limit_wall_y : VECTOR WALL TOP AND BOT LIMMIT MANAGERMENT
*/
vector<vector<uint8_t>> fs_vec_limit_wall_y = {
    {2, 2, 1, 3, 2, 2, 2, 4, 4, 4, 4, 4, 3, 1, 2, 2, 2, 3, 3, 3, 1, 3, 1,
     1, 1, 2, 3, 2, 2, 1, 3, 1, 1, 1, 3, 3, 2, 4, 1, 1, 1, 3, 3, 3, 2, 2,
     1, 1, 2, 2, 4, 3, 2, 3, 3, 4, 3, 1, 1, 3, 3, 1, 1, 1, 3, 3, 3},

    {1, 3, 3, 1, 1, 1, 1, 2, 3, 4, 2, 2, 2, 3, 4, 4, 2, 3, 1, 2, 2, 1, 2,
     2, 2, 1, 2, 4, 4, 3, 2, 3, 2, 2, 3, 2, 1, 1, 3, 1, 1, 1, 2, 2, 1, 2,
     4, 2, 1, 3, 2, 2, 1, 1, 2, 3, 2, 1, 1, 1, 3, 2, 2, 2, 1, 2, 1}
};

// clear all wall
static inline void fs_game_wall_reset() {
    if (!fs_vec_wall.empty()){
        fs_vec_wall.clear();
        fs_vec_wall.shrink_to_fit();
    }
}

// set default for all wall
static inline void fs_game_wall_setup() {
    for (uint8_t i = 0; i < NUM_WALL; i++) {
        fs_wall_info_t temp_wall;

        temp_wall.x = i * 128;
        temp_wall.y = 0;
        temp_wall.ver = FS_WALL_I;
        fs_vec_wall.push_back(temp_wall);
    }
}

// move wall to left
static inline void fs_game_wall_update() {
    if (!fs_vec_wall.empty()) {
        // move "x" map
        for (uint8_t i = 0; i < NUM_WALL; i++) {                         // scan all wall
            fs_vec_wall[i].x -= FS_WALL_MOVE_X;                             // move wall(x)
            if (fs_vec_wall[i].x < (-LCD_WIDTH)) {                       // check wall with -lcd_width(-124)
                fs_vec_wall[i].x = LCD_WIDTH + 1;                        // restore wall(x) -> 124
            }
        }
        // re-update limit axis y top
        fs_vec_limit_wall_y[0].push_back(fs_vec_limit_wall_y[0][0]);
        fs_vec_limit_wall_y[0].erase(fs_vec_limit_wall_y[0].begin());

        // re-update limit axis y bot
        fs_vec_limit_wall_y[1].push_back(fs_vec_limit_wall_y[1][0]);
        fs_vec_limit_wall_y[1].erase(fs_vec_limit_wall_y[1].begin());
    }
}

/***********************************************************
* PROCESS TUNNEL HANDLE
***********************************************************/

void task_fs_wall_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case FS_GAME_WALL_RESET_SIG: {
            APP_DBG_SIG("FS_GAME_WALL_RESET\n");
            fs_game_wall_reset();
            break;
        }
        
        case FS_GAME_WALL_SETUP_SIG: {
            APP_DBG_SIG("FS_GAME_WALL_SETUP\n");
            fs_game_wall_setup();
            break;
        }

        case FS_GAME_WALL_ON_TICK_SIG: {
            fs_game_wall_update();
            break;
        }
        default:
            break;
    }
}