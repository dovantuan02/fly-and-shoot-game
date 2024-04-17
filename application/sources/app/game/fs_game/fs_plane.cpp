#include "app.h"
#include "app_dbg.h"
#include "task_display.h"
#include "screen_manager.h"
#include "buzzer.h"

#include "fs_bom.h"
#include "fs_plane.h"
#include "fs_tunnel_wall.h"
#include "fs_mine.h"
#include "fs_explosion.h"
#include "fs_buzzer_def.h"

#include "scr_bitmap.h"
#include "scr_game_over.h"

/***********************************************************
* VARIABLE PROCESS PLANE
***********************************************************/
#define PLANE_Y_DOWN (1)
#define PLANE_Y_UP   (6)

/*
*   fs_plane : VARIABLE CONTROL PLANE
*/
fs_game_plane_infor_t fs_plane;

// play sound when plane crash
#define FS_SOUND_GAME_OVER()               \
    if (fs_game_setting.fs_setting_sound) {        \
        BUZZER_PlayTones(tones_game_over); \
    }

// plane set coordinates default when game on
static inline void fs_game_plane_setup() {
    fs_plane.x = 5;
    fs_plane.y = 15;
    fs_plane.state = FS_SHOW;
}

// plane down every 100ms
static inline void fs_game_plane_down() {
    fs_plane.y += PLANE_Y_DOWN;
    if (fs_plane.y > MAP_HEIGHT) {
        fs_plane.state = FS_HIDE;
        fs_plane.y = MAX_LCD_HEIGHT;
    }
}

// player press button -> plane up
static inline void fs_game_plane_up() {
    fs_plane.y -= PLANE_Y_UP;
}

// plane touch wall top or bot
static inline void fs_game_wall_crash() {
    // check plane(y) with wall limmit(x,y)
    if ((fs_plane.y + PLANE_ICON_HEIGHT) > (MAP_HEIGHT - fs_vec_limit_wall_y[1][0]) 
        || fs_plane.y < fs_vec_limit_wall_y[0][0]) {     
        if (fs_plane.state == FS_SHOW) {
            fs_plane.state = FS_HIDE;
            // set coordinate explosion
            fs_explosion.x = fs_plane.x;
            fs_explosion.y = fs_plane.y;
            fs_explosion.ver = VER_I;

            // sound buzzer
            FS_SOUND_GAME_OVER();

            // turn off timer move plane down
            timer_remove_attr(FS_GAME_TASK_PLANE_ID, FS_GAME_PLANE_ON_TICK);         
            // POST TO "FS_GAME_EXPLOSION_PUSH" WITH DATA(fs_explosion)
            task_post_dynamic_msg(FS_GAME_TASK_EXPLOSION_ID, FS_GAME_EXPLOSION_PUSH, (uint8_t *)&fs_explosion, sizeof(fs_explosion));
            // set timer tran to screen game over
            timer_set(FS_GAME_TASK_DISPLAY_GAME_OVER_ID, FS_GAME_DISPLAY_GAME_OVER_ON_TICK, AC_GAME_OVER_INTERNAL, TIMER_ONE_SHOT);
        }
    }
}

// plane touch mine
static inline void fs_game_mine_crash() {
    if (!fs_vec_mine.empty()) {
        // scan all mine
        for (auto _mine : fs_vec_mine) {
            // check all mine(x,y) with plane(x,y)
            if ((PLANE_ICON_WIDTH > abs(_mine.x - fs_plane.x) )) { 
                if ((fs_plane.y >= (_mine.y + 1)) && fs_plane.y <= (_mine.y + MINE_ICON_HEIGHT - 1)) {
                    if (fs_plane.state == FS_SHOW) {
                        APP_DBG("PLANE : X: %d - Y: %d\n", fs_plane.x, fs_plane.y);
                        APP_DBG("MINE  : X: %d - Y: %d\n", _mine.x, _mine.y);
                        // set coordinate explosion
                        fs_explosion.x = fs_plane.x;
                        fs_explosion.y = fs_plane.y;
                        fs_explosion.ver = VER_I;
                        
                        // sound buzzer
                        FS_SOUND_GAME_OVER();
                        fs_plane.state = FS_HIDE;
                        // post to "sig_explosion_push" with data(fs_explosion)
                        task_post_dynamic_msg(FS_GAME_TASK_EXPLOSION_ID, FS_GAME_EXPLOSION_PUSH, (uint8_t *)&fs_explosion, sizeof(fs_explosion));
                        // set timer tran to screen game over
                        timer_set(FS_GAME_TASK_DISPLAY_GAME_OVER_ID, FS_GAME_DISPLAY_GAME_OVER_ON_TICK, AC_GAME_OVER_INTERNAL, TIMER_ONE_SHOT);
                        break;
                    }
                    break;
                }
            }
        }
    }
}

// plane touch bom
static inline void fs_game_bom_crash() {
    if (!fs_vec_bom.empty()) {
        // scan all bom
        for (auto _bom : fs_vec_bom) {
            if ((abs(_bom.x - fs_plane.x )<= (PLANE_ICON_WIDTH )) &&                 // check all bom(x,y) with plane(x,y)
                (fs_plane.y >= (_bom.y + 1)) && fs_plane.y <= (_bom.y + BOM_ICON_HEIGHT- 1)) {
                if (fs_plane.state == FS_SHOW) {
                    fs_plane.state = FS_HIDE;
                    // set coordinate explosion
                    fs_explosion.x = fs_plane.x;
                    fs_explosion.y = fs_plane.y;
                    fs_explosion.ver = VER_I;
                    // APP_DBG("PLANE : X: %d - Y: %d\n", fs_plane.x, fs_plane.y);
                    // APP_DBG("MINE  : X: %d - Y: %d\n", _mine.x, _mine.y);

                    // sound buzzer
                    FS_SOUND_GAME_OVER();

                    // post to "sig_explosion_push" with data(fs_explosion)
                    task_post_dynamic_msg(FS_GAME_TASK_EXPLOSION_ID, FS_GAME_EXPLOSION_PUSH, (uint8_t *)&fs_explosion, sizeof(fs_explosion));
                    // set timer tran to screen game over
                    timer_set(FS_GAME_TASK_DISPLAY_GAME_OVER_ID, FS_GAME_DISPLAY_GAME_OVER_ON_TICK, AC_GAME_OVER_INTERNAL, TIMER_ONE_SHOT);
                    break;
                }
            }
        }
    }
}

// handler check all crash(wall, mine , bom)
static inline void fs_game_plane_crash() {
    if(fs_state_game == FS_GAME_ON){
        fs_game_mine_crash();
        fs_game_bom_crash();
        fs_game_wall_crash();
    }
}

/***********************************************************
* PROCESS MINE HANDLE
***********************************************************/

void task_fs_plane_hanle(ak_msg_t *msg) {
    switch (msg->sig) {
        case FS_GAME_PLANE_SETUP: {
            APP_DBG_SIG("FS_GAME_PLANE_SETUP\n");
            fs_game_plane_setup();
            break;
        }
        case FS_GAME_PLANE_DOWN: {  // APP_DBG_SIG("FS_GAME_PLANE_DOWN\n");
            fs_game_plane_down();
            break;
        }
        case FS_GAME_PLANE_UP: {  // APP_DBG_SIG("FS_GAME_PLANE_UP\n");
            fs_game_plane_up();
            break;
        }
        case FS_GAME_PLANE_CRASH: {  // APP_DBG_SIG("FS_GAME_PLANE_CRASH\n");
            fs_game_plane_crash();
            break;
        }
        case FS_GAME_PLANE_ON_TICK: {
            task_post_pure_msg(FS_GAME_TASK_PLANE_ID, FS_GAME_PLANE_DOWN);
            break;
        }
        default:
            break;
    }
}