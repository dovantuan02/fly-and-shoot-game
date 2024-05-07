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
#include "fs_buzzer_def.h"

#include "fs_bitmap.h"


/***********************************************************
* VARIABLE PROCESS MISSILE
***********************************************************/

#define FS_MISSLE_SETUP_X (PLANE_ICON_WIDTH)
#define FS_MISSLE_MOVE_X  (4)

/*
* fs_vec_missile : VECTOR MISSILE MANAGERMENT
*/
vector<fs_missile_info_t> fs_vec_missile;

#define FS_SOUND_EXPLOSION()               \
    if (fs_game_setting.fs_setting_sound) { \
        BUZZER_PlayTones(tones_explosion);   \
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
    if (fs_plane.visible == true) {
        // check missile managerment size with max missile
        if (fs_vec_missile.size() < FS_MAX_MISSLE) {                        
            // crate new missile 
            fs_missile_info_t new_missle(fs_plane.coordinate.x + PLANE_ICON_WIDTH + 2, \
                                                fs_plane.coordinate.y + PLANE_ICON_HEIGHT - MISSLE_ICON_HEIGHT,\
                                                true);
            // check setting sound
            if (fs_game_setting.fs_setting_sound) {
                BUZZER_PlayTones(tones_missle_push); // sound buzzer
            }
            fs_vec_missile.push_back(new_missle); // add missile to missile managerment
        }
    }
}

// move all missile to right screen
static inline void fs_game_missle_move() {
    if (!fs_vec_missile.empty()) {
        for (size_t i = 0; i < fs_vec_missile.size(); i++) { // scan all missile
            fs_vec_missile[i].coordinate.x += FS_MISSLE_MOVE_X; // move missile with + (x)

            if (fs_vec_missile[i].coordinate.x >= LCD_WIDTH) { // check missile(x) with lcd_width(124)
                fs_vec_missile.erase(fs_vec_missile.begin() + i); // erase missile
            }
        }
    }
}

// missile hits mine or bom
static inline void fs_game_missle_crash() {
    for (size_t i = 0; i < fs_vec_missile.size(); i++) {            // scan all missile 
        for (size_t j = 0; j < fs_vec_mine.size(); j++) {                // scan all mine
            if ((fs_vec_mine[j].coordinate.x - fs_vec_missile[i].coordinate.x <= (MISSLE_ICON_WIDTH))) { // check missile(x) with mine(x)
                // check missile(y) with mine(y)
                if ((fs_vec_missile[i].coordinate.y) >= (fs_vec_mine[j].coordinate.y ) 
                    && (fs_vec_missile[i].coordinate.y) <= (fs_vec_mine[j].coordinate.y + MINE_ICON_HEIGHT - 1) ) {
                    // set coordinate for fs_explosion
                    fs_explosion.coordinate.x = fs_vec_mine[j].coordinate.x;
                    fs_explosion.coordinate.y = fs_vec_mine[j].coordinate.y;
                    fs_explosion.ver = FS_EXPLOSION_VER_I;

                    FS_SOUND_EXPLOSION();

                    fs_vec_missile.erase(fs_vec_missile.begin() + i); // erase missile
                    fs_vec_mine.erase(fs_vec_mine.begin() + j); // erase mine

                    // post to "sig_explosion_push" with data(fs_explosion)
                    task_post_pure_msg(FS_GAME_TASK_EXPLOSION_ID, FS_GAME_EXPLOSION_PUSH_SIG);
                    
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
            if ((fs_vec_bom[k].coordinate.x - fs_vec_missile[i].coordinate.x <= (MISSLE_ICON_WIDTH))) { // check missile(x) with bom(x)
                if ((fs_vec_missile[i].coordinate.y >= (fs_vec_bom[k].coordinate.y) // check missile(y) with bom(y)
                    && (fs_vec_missile[i].coordinate.y <= (fs_vec_bom[k].coordinate.y + BOM_ICON_HEIGHT - 1)))) {
                    // set fs_explosion coordinate
                    fs_explosion.coordinate.x = fs_vec_bom[k].coordinate.x;     
                    fs_explosion.coordinate.y = fs_vec_bom[k].coordinate.y;
                    fs_explosion.ver = FS_EXPLOSION_VER_I;

                    FS_SOUND_EXPLOSION();

                    fs_vec_missile.erase(fs_vec_missile.begin() + i); // erase missile
                    fs_vec_bom.erase(fs_vec_bom.begin() + k); // erase bom

                    // post to "sig_explosion_push" with data(fs_explosion)
                    task_post_dynamic_msg(FS_GAME_TASK_EXPLOSION_ID, FS_GAME_EXPLOSION_PUSH_SIG);
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
        case FS_GAME_MISSLE_PUSH_SIG: {
            APP_DBG_SIG("FS_GAME_MISSLE_PUSH\n");
            fs_game_missle_push();
            break;
        } 

        case FS_GAME_MISSLE_RESET_SIG: {
            APP_DBG_SIG("FS_GAME_MISSLE_RESET\n");
            fs_game_missle_reset();
            break;
        } 

        case FS_GAME_MISSLE_ON_TICK_SIG: {
            fs_game_missle_move();
            break;
        }

        case FS_GAME_MISSLE_CRASH_SIG: {
            fs_game_missle_crash();
            break;
        }
		
        default:
            break;
    }
}