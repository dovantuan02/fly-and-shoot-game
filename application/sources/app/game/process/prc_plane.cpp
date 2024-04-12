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

#define PLANE_Y_DOWN (1)
#define PLANE_Y_UP   (6)

plane_infor_t plane;

#define SOUND_GAME_OVER()         \
    if (game_setting.state_sound) \
        BUZZER_PlayTones(tones_game_over);
 
static inline void plane_setup() {
    plane.x = 5;
    plane.y = 15;
    plane.state = SHOW;
}

static inline void plane_down() {
    plane.y += PLANE_Y_DOWN;
    if (plane.y > MAP_HEIGHT) {
        plane.state = HIDE;
        plane.y = MAX_LCD_HEIGHT;
    }
}

static inline void plane_up() {
    plane.y -= PLANE_Y_UP;
}

static inline void wall_crash() {
    if ((plane.y + PLANE_ICON_HEIGHT) > (MAP_HEIGHT - v_limit_y[1][0]) || plane.y < v_limit_y[0][0]) {
        if (plane.state == SHOW) {
            plane.state = HIDE;

            explosion.x = plane.x;
            explosion.y = plane.y;
            explosion.ver = VER_I;
            SOUND_GAME_OVER();
            timer_remove_attr(AC_TASK_PLANE_ID, SIG_PLANE_ON_TICK);
            task_post_dynamic_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_PUSH, (uint8_t *)&explosion, sizeof(explosion));
            timer_set(AC_TASK_DISPLAY_GAME_OVER_ID, SIG_DISPLAY_GAME_OVER_ON_TICK, AC_GAME_OVER_INTERNAL, TIMER_ONE_SHOT);
        }
    }
}

static inline void mine_crash() {
    if (!v_mine.empty()) {
        for (auto _mine : v_mine) {

            if ((abs(_mine.x - plane.x) <= (PLANE_ICON_WIDTH)) &&
                (plane.y >= (_mine.y + 2)) && plane.y <= (_mine.y + MINE_ICON_HEIGHT- 2)) {
                if (plane.state == SHOW) {
                    plane.state = HIDE;

                    explosion.x = plane.x;
                    explosion.y = plane.y;
                    explosion.ver = VER_I;
                    // APP_DBG("PLANE : X: %d - Y: %d\n", plane.x, plane.y);
                    // APP_DBG("MINE  : X: %d - Y: %d\n", _mine.x, _mine.y);
                    SOUND_GAME_OVER();
                    task_post_dynamic_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_PUSH, (uint8_t *)&explosion, sizeof(explosion));
                    timer_set(AC_TASK_DISPLAY_GAME_OVER_ID, SIG_DISPLAY_GAME_OVER_ON_TICK, AC_GAME_OVER_INTERNAL, TIMER_ONE_SHOT);
                }
                break;
            }
        }
    }
}

static inline void bom_crash() {
    if (!v_bom_infor.empty()) {
        for (auto _bom : v_bom_infor) {
            if ((_bom.x - plane.x <= (PLANE_ICON_WIDTH)) &&
                (plane.y >= (_bom.y + 2)) && plane.y <= (_bom.y + BOM_ICON_HEIGHT- 2)) {
                if (plane.state == SHOW) {
                    plane.state = HIDE;

                    explosion.x = plane.x;
                    explosion.y = plane.y;
                    explosion.ver = VER_I;
                    // APP_DBG("PLANE : X: %d - Y: %d\n", plane.x, plane.y);
                    // APP_DBG("MINE  : X: %d - Y: %d\n", _mine.x, _mine.y);
                    SOUND_GAME_OVER();
                    task_post_dynamic_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_PUSH, (uint8_t *)&explosion, sizeof(explosion));
                    timer_set(AC_TASK_DISPLAY_GAME_OVER_ID, SIG_DISPLAY_GAME_OVER_ON_TICK, AC_GAME_OVER_INTERNAL, TIMER_ONE_SHOT);
                }
                break;
            }
        }
    }
}

static inline void plane_crash() {
    if(state_game == GAME_ON){
        wall_crash();
        mine_crash();
        bom_crash();
    }
}

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