#include <vector>

#include "app.h"
#include "task_display.h"
#include "task_list.h"

#include "prc_tunnel_wall.h"
#include "prc_plane.h"

#include "scr_bitmap.h"
#include "scr_game_over.h"
#include "screen_infor.h"

using namespace std;

/***********************************************************
* VARIABLE PROCESS PLANE
***********************************************************/
#define WALL_MOVE_X (4)
#define SPLIT_WALL_X (4)

/*
* v_wall : VECTOR WALL MANAGERMENT
*/
vector<wall_infor_t> v_wall;

/*
* v_limit_y : VECTOR WALL TOP AND BOT LIMMIT MANAGERMENT
*/
vector<vector<uint8_t>> v_limit_y = {
    {2, 2, 1, 3, 2, 2, 2, 4, 4, 4, 4, 4, 3, 1, 2, 2, 2, 3, 3, 3, 1, 3, 1,
     1, 1, 2, 3, 2, 2, 1, 3, 1, 1, 1, 3, 3, 2, 4, 1, 1, 1, 3, 3, 3, 2, 2,
     1, 1, 2, 2, 4, 3, 2, 3, 3, 4, 3, 1, 1, 3, 3, 1, 1, 1, 3, 3, 3},

    {1, 3, 3, 1, 1, 1, 1, 2, 3, 4, 2, 2, 2, 3, 4, 4, 2, 3, 1, 2, 2, 1, 2,
     2, 2, 1, 2, 4, 4, 3, 2, 3, 2, 2, 3, 2, 1, 1, 3, 1, 1, 1, 2, 2, 1, 2,
     4, 2, 1, 3, 2, 2, 1, 1, 2, 3, 2, 1, 1, 1, 3, 2, 2, 2, 1, 2, 1}
};

// CLEAR ALL WALL
static inline void wall_reset() {
    if (!v_wall.empty()){
        v_wall.clear();
        v_wall.shrink_to_fit();
    }
}

// SET DEFAULT FOR ALL WALL
static inline void wall_setup() {
    for (uint8_t i = 0; i < NUM_WALL; i++) {

        wall_infor_t temp_wall;

        temp_wall.x = i * 128;
        temp_wall.y = 0;
        temp_wall.ver = WALL_I;
        v_wall.push_back(temp_wall);
    }
}

// MOVE WALL TO LEFT
static inline void wall_update() {
    if (!v_wall.empty()) {
        // MOVE "X" MAP
        for (uint8_t i = 0; i < NUM_WALL; i++) {                    // SCAN ALL WALL
            v_wall[i].x -= WALL_MOVE_X;                             // MOVE WALL(X)
            if (v_wall[i].x < (-LCD_WIDTH)) {                       // CHECK WALL WITH -LCD_WIDTH(-124)
                v_wall[i].x = LCD_WIDTH + 1;                        // RESTORE WALL(X) -> 124
            }
        }
        // RE-UPDATE LIMIT AXIS Y TOP
        v_limit_y[0].push_back(v_limit_y[0][0]);
        v_limit_y[0].erase(v_limit_y[0].begin());

        // RE-UPDATE LIMIT AXIS Y BOT
        v_limit_y[1].push_back(v_limit_y[1][0]);
        v_limit_y[1].erase(v_limit_y[1].begin());
    }
}

void task_prc_wall_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SIG_WALL_RESET: {
            APP_DBG_SIG("SIG_WALL_RESET\n");
            wall_reset();
            break;
        }
        case SIG_WALL_SETUP: {
            APP_DBG_SIG("SIG_WALL_SETUP\n");
            wall_setup();
            break;
        }

        case SIG_WALL_UPDATE: {
            // APP_DBG_SIG("SIG_WALL_UPDATE\n");
            wall_update();
            break;
        }
        case SIG_WALL_ON_TICK: {
            task_post_pure_msg(AC_TASK_WALL_ID, SIG_WALL_UPDATE);
            break;
        }
        default:
            break;
    }
}