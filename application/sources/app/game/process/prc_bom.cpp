#include "app.h"
#include "app_dbg.h"
#include "task_display.h"

#include "prc_bom.h"
#include "prc_tunnel_wall.h"

#include "scr_bitmap.h"

#define BOM_MOVE_X        (4)
#define MAX_BOM           (game_setting.game_mode)

vector<bom_infor_t> v_bom_infor;

static inline void bom_reset() {
    // APP_DBG("MAX BOM : %d\n", MAX_BOM);  
    if (!v_bom_infor.empty()){
        v_bom_infor.clear();
        v_bom_infor.shrink_to_fit();
    }
}

static inline void bom_push() {
    if (v_bom_infor.size() < MAX_BOM) {
        if (!v_limit_y.empty()) {
            uint8_t up = MAP_HEIGHT - v_limit_y[1][v_limit_y[1].size() - 1] - (10);
            uint8_t low =             v_limit_y[0][v_limit_y[0].size() - 1] + (10);

            bom_infor_t temp_bom;

            temp_bom.x = LCD_WIDTH;
            temp_bom.y = rand() % (up - low + 1) + low;
            v_bom_infor.push_back(temp_bom);
        }
    }
}

static inline void bom_move() {
    if (!v_bom_infor.empty()) {
        for (size_t i = 0; i < v_bom_infor.size(); i++) {
            v_bom_infor[i].x -= BOM_MOVE_X;
            if (v_bom_infor[i].x < 0) {
                v_bom_infor.erase(v_bom_infor.begin() - i);
                
            }
            
        }
    }
}

void task_prc_bom_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SIG_BOM_RESET:
            APP_DBG_SIG("SIG_BOM_RESET\n");
            bom_reset();
            break;

        case SIG_BOM_PUSH:
            APP_DBG_SIG("SIG_BOM_PUSH\n");
            bom_push();
            break;

        case SIG_BOM_MOVE:
            // APP_DBG_SIG("SIG_BOM_MOVE\n");
            bom_move();
            break;

        case SIG_BOM_ON_TICK:
            task_post_pure_msg(AC_TASK_BOM_ID, SIG_BOM_MOVE);
            break;
        default:
            break;
    }
}