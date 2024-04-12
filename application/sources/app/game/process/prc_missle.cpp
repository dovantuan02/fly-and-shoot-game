#include "app.h"
#include "app_dbg.h"
#include "task_display.h"
#include "task_list.h"
#include "timer.h"
#include "buzzer.h"

#include "prc_bom.h"
#include "prc_explosion.h"
#include "prc_mine.h"
#include "prc_plane.h"
#include "prc_missle.h"
#include "buzzer_def.h"

#include "scr_bitmap.h"
#include "screen_infor.h"

#define MISSLE_SETUP_X (PLANE_ICON_WIDTH)
#define MISSLE_MOVE_X  (4)

vector<missle> v_missle;

static inline void missle_reset() {
    if (v_missle.size() != 0) {
        v_missle.clear();
        v_missle.shrink_to_fit();
    }
}

static inline void missle_push() {
    if(plane.state == SHOW)
    {
        if (v_missle.size() < MAX_MISSLE) {
            missle new_missle(plane.x + PLANE_ICON_WIDTH + 2,
                            plane.y + PLANE_ICON_HEIGHT - MISSLE_ICON_HEIGHT,
                            SHOW);
            if (game_setting.state_sound)
                BUZZER_PlayTones(tones_missle_push);
            v_missle.push_back(new_missle);
            
        }
    }
}

static inline void missle_move() {
    if (!v_missle.empty()) {
        for (size_t i = 0; i < v_missle.size(); i++) {
            v_missle[i].x += MISSLE_MOVE_X;

            if (v_missle[i].x >= LCD_WIDTH) {
                v_missle.erase(v_missle.begin() + i);
            }
        }
    }
}

static inline void missle_crash() {
    for (size_t i = 0; i < v_missle.size(); i++) {
        for (size_t j = 0; j < v_mine.size(); j++) {

            if ((v_mine[j].x - v_missle[i].x <= (MISSLE_ICON_WIDTH))) {

                if ((v_missle[i].y) >= (v_mine[j].y ) 
                    && (v_missle[i].y) <= (v_mine[j].y + MINE_ICON_HEIGHT - 1) ) {

                    explosion.x = v_mine[j].x;
                    explosion.y = v_mine[j].y;
                    explosion.ver = VER_I;

                    if (game_setting.state_sound)
                        BUZZER_PlayTones(tones_explosion);

                    v_missle.erase(v_missle.begin() + i);
                    v_mine.erase(v_mine.begin() + j);

                    task_post_dynamic_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_PUSH, (uint8_t *)&explosion, sizeof(explosion));
                    
                    if (v_mine[i].ver == 0) {
                        score += 1;
                    } else {
                        score += 2;
                    }
                }
            }
        }
        for (size_t k = 0; k < v_bom_infor.size(); k++) {

            if ((v_bom_infor[k].x - v_missle[i].x <= (MISSLE_ICON_WIDTH))) {

                if ((v_missle[i].y >= (v_bom_infor[k].y) 
                    && (v_missle[i].y <= (v_bom_infor[k].y + BOM_ICON_HEIGHT - 1)))) {
                        
                    explosion.x = v_bom_infor[k].x;
                    explosion.y = v_bom_infor[k].y;
                    explosion.ver = VER_I;

                    if (game_setting.state_sound)
                        BUZZER_PlayTones(tones_explosion);

                    v_missle.erase(v_missle.begin() + i);
                    v_bom_infor.erase(v_bom_infor.begin() + k);

                    task_post_dynamic_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_PUSH, (uint8_t *)&explosion, sizeof(explosion));
                }
            }
        }
    }
}

void task_prc_missle_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SIG_MISSLE_PUSH:
            APP_DBG_SIG("SIG_MISSLE_PUSH\n");
            missle_push();
            break;

        case SIG_MISSLE_MOVE:
            // APP_DBG_SIG("SIG_MISSLE_MOVE\n");
            missle_move();
            break;

        case SIG_MISSLE_RESET:
            APP_DBG_SIG("SIG_MISSLE_RESET\n");
            missle_reset();
            break;

        case SIG_MISSLE_ON_TICK:
            task_post_pure_msg(AC_TASK_MISSLE_ID, SIG_MISSLE_MOVE);
            break;

        case SIG_MISSLE_CRASH:
            // APP_DBG_SIG("SIG_MISSLE_CRASH\n");
            missle_crash();
            break;

        default:
            break;
    }
}