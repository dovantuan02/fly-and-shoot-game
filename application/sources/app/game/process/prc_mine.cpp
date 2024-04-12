#include <vector>

#include "app.h"
#include "timer.h"
#include "task_list.h"
#include "task_display.h"

#include "prc_bom.h"
#include "prc_tunnel_wall.h"
#include "prc_explosion.h"
#include "prc_mine.h"

#include "scr_bitmap.h"
#include "screen_infor.h"
#define MINE_MOVE_X (4)
#define MAX_MINE    (game_setting.game_mode + 3)

vector<mine_infor_t> v_mine;

static inline void mine_reset() {
    if (v_mine.size() != 0) {
        v_mine.clear();
        v_mine.shrink_to_fit();
    }
}
static inline void mine_push() 
{
    if(static_cast<int>(v_mine.size()) <= MAX_MINE)
    {
        uint8_t up = MAP_HEIGHT - v_limit_y[1][v_limit_y[1].size() - 1] - (10);
        uint8_t low =             v_limit_y[0][v_limit_y[0].size() - 1] + (10);

        mine_infor_t temp_mine;

        temp_mine.x = LCD_WIDTH;
        temp_mine.y = rand() % (up - low + 1) + low;;
        temp_mine.ver = rand() % 2;
        temp_mine.state = SHOW;
        v_mine.push_back(temp_mine);
        if (!v_bom_infor.empty()) {
            for (size_t i = 0; i < v_bom_infor.size(); i++) {
                if (temp_mine.x == v_bom_infor[i].x && abs(temp_mine.y - v_bom_infor[i].y) < 10) {
                    v_mine.pop_back();
                }
            }
        }
    }
    
}
static inline void mine_move()
{
    if (!v_mine.empty()) {
        for (size_t i = 0; i < v_mine.size(); i++) {
            if (v_mine[i].x > 0) {
                v_mine[i].x -= MINE_MOVE_X;
                continue;
            }
            v_mine.erase(v_mine.begin() + i);
        }
    }
}
void task_prc_mine_handle(ak_msg_t *msg)
{
    switch (msg->sig)
    {
        case SIG_MINE_RESET:
            mine_reset();
            break;
        
        case SIG_MINE_PUSH:
            mine_push();
            break;
        
        case SIG_MINE_MOVE:
            mine_move();
            break;
        
        case SIG_MINE_ON_TICK:
            task_post_pure_msg(AC_TASK_MINE_ID, SIG_MINE_MOVE);
            break;
        default:
            break;
    }
}