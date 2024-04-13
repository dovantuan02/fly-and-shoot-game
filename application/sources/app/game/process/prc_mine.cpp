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

/***********************************************************
* VARIABLE PROCESS EXPLOSION
***********************************************************/
#define MINE_MOVE_X (4)

/*
* MAX BOM -> GET FROM GAME SETTING MODE (EASY = 2, NORMAL = 3, HARD = 4)
*/
#define MAX_MINE    (game_setting.game_mode + 3)

/*
* v_mine : VECTOR MINE MANAGERMENT
*/
vector<mine_infor_t> v_mine;

// CLEAR ALL MINE
static inline void mine_reset() {
    if (v_mine.size() != 0) {
        v_mine.clear();
        v_mine.shrink_to_fit();
    }
}

// ADD MINE TO MINE MANAGERMENT
static inline void mine_push() 
{
    if (static_cast<int>(v_mine.size()) <= MAX_MINE) {
        // GET BOT Y LIMMIT 
        uint8_t bot_limmit = MAP_HEIGHT - v_limit_y[1][v_limit_y[1].size() - 1] - (10);
        // GET TOP Y LIMMIT
        uint8_t top_limmit =              v_limit_y[0][v_limit_y[0].size() - 1] + (10);

        mine_infor_t temp_mine;

        temp_mine.x = LCD_WIDTH;
        // SET MINE Y RANGE (TOP Y LIMMIT - BOT Y LIMMIT)
        temp_mine.y = rand() % (bot_limmit - top_limmit + 1) + top_limmit; 
        temp_mine.state = SHOW;
        temp_mine.ver = rand() % 2;              // RANDOM MINE 
        v_mine.push_back(temp_mine);            // ADD MINE TO MINE MANAGERMENT
        if (!v_bom_infor.empty()) {             
            for (size_t i = 0; i < v_bom_infor.size(); i++) {           // SCAN ALL BOM
                /* 
                *   CHECK ALL BOM WITH MINE
                *   IF BOM(X,Y) EQUAL MINE(X,Y) ->? ERASE 
                */
                if (temp_mine.x == v_bom_infor[i].x && abs(temp_mine.y - v_bom_infor[i].y) < 10) {    
                    v_mine.pop_back();
                }
            }
        }
    }
    
}

//  MOVE ALL MINE
static inline void mine_move()
{
    if (!v_mine.empty()) {
        for (size_t i = 0; i < v_mine.size(); i++) {            // SCAN ALL MINE
            if (v_mine[i].x > 0) {                              // MINE(X)  BIGGER MIN LCD(X) 
                v_mine[i].x -= MINE_MOVE_X;                     // MOVE MINE X
                continue;
            }
            v_mine.erase(v_mine.begin() + i);                   // ERASE MINE
        }
    }
}

/***********************************************************
* PROCESS MINE HANDLE
***********************************************************/
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