#include "app.h"
#include "app_dbg.h"
#include "task_display.h"
#include "screen_manager.h"
#include "buzzer.h"

#include "prc_bom.h"
#include "prc_plane.h"
#include "prc_tunnel_wall.h"
#include "prc_mine.h"
#include "prc_explosion.h"
#include "buzzer_def.h"

#include "scr_bitmap.h"
#include "scr_game_over.h"

/***********************************************************
* VARIABLE PROCESS PLANE
***********************************************************/
#define PLANE_Y_DOWN (1)
#define PLANE_Y_UP   (6)

/*
*   plane : VARIABLE CONTROL PLANE
*/
plane_infor_t plane;

// PLAY SOUND WHEN PLANE CRASH
#define SOUND_GAME_OVER()         \
    if (game_setting.state_sound) \
        BUZZER_PlayTones(tones_game_over);
 
// PLANE SET DEFAULT WHEN GAME ON
static inline void plane_setup() {
    plane.x = 5;
    plane.y = 15;
    plane.state = SHOW;
}

// PLANE DOWN EVERY 100ms
static inline void plane_down() {
    plane.y += PLANE_Y_DOWN;
    if (plane.y > MAP_HEIGHT) {
        plane.state = HIDE;
        plane.y = MAX_LCD_HEIGHT;
    }
}

// PLAYER PRESS BUTTON -> PLANE UP
static inline void plane_up() {
    plane.y -= PLANE_Y_UP;
}

// PLANE TOUCH WALL TOP OR BOT
static inline void wall_crash() {
    // CHECK PLANE(Y) WITH WALL LIMMIT(X,Y)
    if ((plane.y + PLANE_ICON_HEIGHT) > (MAP_HEIGHT - v_limit_y[1][0]) || plane.y < v_limit_y[0][0]) {     
        if (plane.state == SHOW) {
            plane.state = HIDE;
            // SET COORDINATE EXPLOSION
            explosion.x = plane.x;
            explosion.y = plane.y;
            explosion.ver = VER_I;

            // SOUND BUZZER
            SOUND_GAME_OVER();

            // TURN OFF TIMER MOVE PLANE DOWN
            timer_remove_attr(AC_TASK_PLANE_ID, SIG_PLANE_ON_TICK);         
            // POST TO "SIG_EXPLOSION_PUSH" WITH DATA(explosion)
            task_post_dynamic_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_PUSH, (uint8_t *)&explosion, sizeof(explosion));
            // SET TIMER TRAN TO SCREEN GAME OVER
            timer_set(AC_TASK_DISPLAY_GAME_OVER_ID, SIG_DISPLAY_GAME_OVER_ON_TICK, AC_GAME_OVER_INTERNAL, TIMER_ONE_SHOT);
        }
    }
}

// PLANE TOUCH MINE
static inline void mine_crash() {
    if (!v_mine.empty()) {
        // SCAN ALL MINE
        for (auto _mine : v_mine) {        
            if ((abs(_mine.x - plane.x) <= (PLANE_ICON_WIDTH)) &&       // CHECK ALL MINE(X,Y) WITH PLANE(X,Y)
                (plane.y >= (_mine.y + 2)) && plane.y <= (_mine.y + MINE_ICON_HEIGHT- 2)) {
                if (plane.state == SHOW) {
                    plane.state = HIDE;
                    // SET COORDINATE EXPLOSION
                    explosion.x = plane.x;
                    explosion.y = plane.y;
                    explosion.ver = VER_I;
                    // APP_DBG("PLANE : X: %d - Y: %d\n", plane.x, plane.y);
                    // APP_DBG("MINE  : X: %d - Y: %d\n", _mine.x, _mine.y);

                    // SOUND BUZZER
                    SOUND_GAME_OVER();
                    // POST TO "SIG_EXPLOSION_PUSH" WITH DATA(explosion)
                    task_post_dynamic_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_PUSH, (uint8_t *)&explosion, sizeof(explosion));
                    // SET TIMER TRAN TO SCREEN GAME OVER
                    timer_set(AC_TASK_DISPLAY_GAME_OVER_ID, SIG_DISPLAY_GAME_OVER_ON_TICK, AC_GAME_OVER_INTERNAL, TIMER_ONE_SHOT);
                    break;
                }
            }
        }
    }
}

// PLANE TOUCH BOM
static inline void bom_crash() {
    if (!v_bom_infor.empty()) {
        // SCAN ALL BOM
        for (auto _bom : v_bom_infor) {
            if ((_bom.x - plane.x <= (PLANE_ICON_WIDTH )) &&                 // CHECK ALL BOM(X,Y) WITH PLANE(X,Y)
                (plane.y >= (_bom.y + 2)) && plane.y <= (_bom.y + BOM_ICON_HEIGHT- 2)) {
                if (plane.state == SHOW) {
                    plane.state = HIDE;
                    // SET COORDINATE EXPLOSION
                    explosion.x = plane.x;
                    explosion.y = plane.y;
                    explosion.ver = VER_I;
                    // APP_DBG("PLANE : X: %d - Y: %d\n", plane.x, plane.y);
                    // APP_DBG("MINE  : X: %d - Y: %d\n", _mine.x, _mine.y);

                    // SOUND BUZZER
                    SOUND_GAME_OVER();

                    // POST TO "SIG_EXPLOSION_PUSH" WITH DATA(explosion)
                    task_post_dynamic_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_PUSH, (uint8_t *)&explosion, sizeof(explosion));
                    // SET TIMER TRAN TO SCREEN GAME OVER
                    timer_set(AC_TASK_DISPLAY_GAME_OVER_ID, SIG_DISPLAY_GAME_OVER_ON_TICK, AC_GAME_OVER_INTERNAL, TIMER_ONE_SHOT);
                    break;
                }
            }
        }
    }
}

// HANDLER CHECK ALL CRASH(WALL, MINE , BOM)
static inline void plane_crash() {
    if(state_game == GAME_ON){
        wall_crash();
        mine_crash();
        bom_crash();
    }
}

/***********************************************************
* PROCESS MINE HANDLE
***********************************************************/
void task_prc_plane_hanle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SIG_PLANE_SETUP:
            APP_DBG_SIG("SIG_PLANE_SETUP\n");
            plane_setup();
            break;

        case SIG_PLANE_DOWN:
            // APP_DBG_SIG("SIG_PLANE_DOWN\n");
            plane_down();
            break;

        case SIG_PLANE_UP:
            // APP_DBG_SIG("SIG_PLANE_UP\n");
            plane_up();
            break;
        case SIG_PLANE_CRASH:
            // APP_DBG_SIG("SIG_PLANE_CRASH\n");
            plane_crash();
            break;

        case SIG_PLANE_ON_TICK:
            task_post_pure_msg(AC_TASK_PLANE_ID, SIG_PLANE_DOWN);
            break;
            
        default:
            break;
    }
}