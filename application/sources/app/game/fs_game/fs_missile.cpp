#include "app.h"
#include "app_dbg.h"
#include "task_display.h"
#include "task_list.h"
#include "timer.h"
#include "buzzer.h"

#include "fs_bom.h"
#include "fs_explosion.h"
#include "fs_mine.h"
#include "fs_plane.h"
#include "fs_missile.h"
#include "fs_config.h"
#include "buzzer_def.h"

#include "scr_bitmap.h"


/***********************************************************
* VARIABLE PROCESS MISSILE
***********************************************************/

#define MISSLE_SETUP_X (PLANE_ICON_WIDTH)
#define MISSLE_MOVE_X  (4)

/*
* fs_vec_missile : VECTOR MISSILE MANAGERMENT
*/
vector<fs_game_missile_infor_t> fs_vec_missile;

#define FS_SOUND_EXPLOSION()               \
    if (fs_game_setting.fs_setting_sound) {        \
        BUZZER_PlayTones(tones_explosion); \
    }

// clear all missile available
static inline void fs_game_missle_reset() {
    if (fs_vec_missile.size() != 0) {
        fs_vec_missile.clear();
        fs_vec_missile.shrink_to_fit();
    }
}

// add missile to missile managerment
static inline void fs_game_missle_push() {
    if (fs_plane.state == SHOW) {
        // check missile managerment size with max missile
        if (fs_vec_missile.size() < FS_MAX_MISSLE) {                        
            // crate new missile 
            fs_game_missile_infor_t new_missle(fs_plane.x + PLANE_ICON_WIDTH + 2, \
                                                fs_plane.y + PLANE_ICON_HEIGHT - MISSLE_ICON_HEIGHT,\
                                                SHOW);
            // check setting sound
            if (fs_game_setting.fs_setting_sound) {
                BUZZER_PlayTones(tones_missle_push);
            }                                               // sound buzzer
            fs_vec_missile.push_back(new_missle);           // add missile to missile managerment
        }
    }
}

// move all missile to right screen
static inline void fs_game_missle_move() {
    if (!fs_vec_missile.empty()) {
        for (size_t i = 0; i < fs_vec_missile.size(); i++) {          // SCAN ALL MISSILE
            fs_vec_missile[i].x += MISSLE_MOVE_X;                     // MOVE MISSILE WITH +(X)

            if (fs_vec_missile[i].x >= LCD_WIDTH) {                   // CHECK MISSILE(X) WITH LCD_WIDTH(124)
                fs_vec_missile.erase(fs_vec_missile.begin() + i);           // ERASE MISSILE
            }
        }
    }
}

// missile hits mine or bom
static inline void fs_game_missle_crash() {
    for (size_t i = 0; i < fs_vec_missile.size(); i++) {            // scan all missile 
        for (size_t j = 0; j < fs_vec_mine.size(); j++) {                // scan all mine
            if ((fs_vec_mine[j].x - fs_vec_missile[i].x <= (MISSLE_ICON_WIDTH))) {             // check missile(x) with mine(x)
                // check missile(y) with mine(y)
                if ((fs_vec_missile[i].y) >= (fs_vec_mine[j].y ) 
                    && (fs_vec_missile[i].y) <= (fs_vec_mine[j].y + MINE_ICON_HEIGHT - 1) ) {
                    // set coordinate for fs_explosion
                    fs_explosion.x = fs_vec_mine[j].x;
                    fs_explosion.y = fs_vec_mine[j].y;
                    fs_explosion.ver = VER_I;

                    FS_SOUND_EXPLOSION();

                    fs_vec_missile.erase(fs_vec_missile.begin() + i);       // erase missile
                    fs_vec_mine.erase(fs_vec_mine.begin() + j);                       // erase mine

                    // post to "sig_explosion_push" with data(fs_explosion)
                    task_post_dynamic_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_PUSH, (uint8_t *)&fs_explosion, sizeof(fs_explosion));
                    
                    // check mine ver : 0 -> fs_game_score + 1    ||   1 -> fs_game_score + 2
                    if (fs_vec_mine[i].ver == 0) {       
                        fs_game_score += 1;
                    } 
                    else {
                        fs_game_score += 2;
                    }
                }
            }
        }
        for (size_t k = 0; k < fs_vec_bom.size(); k++) {
            if ((fs_vec_bom[k].x - fs_vec_missile[i].x <= (MISSLE_ICON_WIDTH))) {            // check missile(x) with bom(x)
                if ((fs_vec_missile[i].y >= (fs_vec_bom[k].y)                                // check missile(y) with bom(y)
                    && (fs_vec_missile[i].y <= (fs_vec_bom[k].y + BOM_ICON_HEIGHT - 1)))) {
                    // set fs_explosion coordinate
                    fs_explosion.x = fs_vec_bom[k].x;     
                    fs_explosion.y = fs_vec_bom[k].y;
                    fs_explosion.ver = VER_I;

                    FS_SOUND_EXPLOSION();

                    fs_vec_missile.erase(fs_vec_missile.begin() + i);       // erase missile
                    fs_vec_bom.erase(fs_vec_bom.begin() + k);               // erase bom

                    // post to "sig_explosion_push" with data(fs_explosion)
                    task_post_dynamic_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_PUSH, (uint8_t *)&fs_explosion, sizeof(fs_explosion));
                }
            }
        }
    }
}

/***********************************************************
* PROCESS MINE HANDLE
***********************************************************/

void task_fs_missle_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SIG_MISSLE_PUSH: {
            APP_DBG_SIG("SIG_MISSLE_PUSH\n");
            fs_game_missle_push();
        } break;

        case SIG_MISSLE_MOVE: {  // APP_DBG_SIG("SIG_MISSLE_MOVE\n");
            fs_game_missle_move();
        } break;

        case SIG_MISSLE_RESET: {
            APP_DBG_SIG("SIG_MISSLE_RESET\n");
            fs_game_missle_reset();
        } break;

        case SIG_MISSLE_ON_TICK: {
            task_post_pure_msg(AC_TASK_MISSLE_ID, SIG_MISSLE_MOVE);
            break;
        }
        case SIG_MISSLE_CRASH: {  // APP_DBG_SIG("SIG_MISSLE_CRASH\n");
            fs_game_missle_crash();
            break;
        }
        default:
            break;
    }
}