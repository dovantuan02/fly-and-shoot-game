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
#include "prc_missile.h"
#include "buzzer_def.h"

#include "scr_bitmap.h"
#include "screen_infor.h"

/***********************************************************
* VARIABLE PROCESS MISSILE
***********************************************************/
#define MISSLE_SETUP_X (PLANE_ICON_WIDTH)
#define MISSLE_MOVE_X  (4)

/*
* v_missile : VECTOR MISSILE MANAGERMENT
*/
vector<missile> v_missile;

// CLEAR ALL MISSILE AVAILABLE
static inline void missle_reset() {
    if (v_missile.size() != 0) {
        v_missile.clear();
        v_missile.shrink_to_fit();
    }
}

// ADD MISSILE TO MISSILE MANAGERMENT
static inline void missle_push() {
    if(plane.state == SHOW)
    {
        // CHECK MISSILE MANAGERMENT SIZE WITH MAX MISSILE
        if (v_missile.size() < MAX_MISSLE) {                        
            // CRATE NEW MISSILE 
            missile new_missle(plane.x + PLANE_ICON_WIDTH + 2,           
                            plane.y + PLANE_ICON_HEIGHT - MISSLE_ICON_HEIGHT,
                            SHOW);
            if (game_setting.state_sound)                   // CHECK SETTING SOUND
                BUZZER_PlayTones(tones_missle_push);        // SOUND BUZZER
            v_missile.push_back(new_missle);                 // ADD MISSILE TO MISSILE MANAGERMENT
            
        }
    }
}

//  MOVE ALL MISSILE
static inline void missle_move() {
    if (!v_missile.empty()) {
        for (size_t i = 0; i < v_missile.size(); i++) {          // SCAN ALL MISSILE
            v_missile[i].x += MISSLE_MOVE_X;                     // MOVE MISSILE WITH +(X)

            if (v_missile[i].x >= LCD_WIDTH) {                   // CHECK MISSILE(X) WITH LCD_WIDTH(124)
                v_missile.erase(v_missile.begin() + i);           // ERASE MISSILE
            }
        }
    }
}

//  MISSILE HITS MINE OR BOM
static inline void missle_crash() {
    for (size_t i = 0; i < v_missile.size(); i++) {          // SCAN ALL MISSILE 
        for (size_t j = 0; j < v_mine.size(); j++) {        // SCAN ALL MINE
            if ((v_mine[j].x - v_missile[i].x <= (MISSLE_ICON_WIDTH))) {             // CHECK MISSILE(X) WITH MINE(X)
                // CHECK MISSILE(Y) WITH MINE(Y)
                if ((v_missile[i].y) >= (v_mine[j].y ) 
                    && (v_missile[i].y) <= (v_mine[j].y + MINE_ICON_HEIGHT - 1) ) {
                    // SET COORDINATE FOR EXPLOSION
                    explosion.x = v_mine[j].x;
                    explosion.y = v_mine[j].y;
                    explosion.ver = VER_I;

                    if (game_setting.state_sound)           // CHECK SOUND SETTING
                        BUZZER_PlayTones(tones_explosion);  // SOUND BUZZER

                    v_missile.erase(v_missile.begin() + i);       // ERASE MISSILE
                    v_mine.erase(v_mine.begin() + j);           // ERASE MINE

                    // POST TO "SIG_EXPLOSION_PUSH" WITH DATA(explosion)
                    task_post_dynamic_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_PUSH, (uint8_t *)&explosion, sizeof(explosion));
                    
                    // CHECK MINE VER : 0 -> SCORE + 1    ||   1 -> SCORE + 2
                    if (v_mine[i].ver == 0) {       
                        score += 1;
                    } else {
                        score += 2;
                    }
                }
            }
        }
        for (size_t k = 0; k < v_bom_infor.size(); k++) {
            if ((v_bom_infor[k].x - v_missile[i].x <= (MISSLE_ICON_WIDTH))) {            // CHECK MISSILE(X) WITH BOM(X)
                
                if ((v_missile[i].y >= (v_bom_infor[k].y)                                // CHECK MISSILE(Y) WITH BOM(Y)
                    && (v_missile[i].y <= (v_bom_infor[k].y + BOM_ICON_HEIGHT - 1)))) {
                    // SET EXPLOSION COORDINATE
                    explosion.x = v_bom_infor[k].x;     
                    explosion.y = v_bom_infor[k].y;
                    explosion.ver = VER_I;

                    if (game_setting.state_sound)               // CHECK SOUND SETTING
                        BUZZER_PlayTones(tones_explosion);      // SOUND BUZZER

                    v_missile.erase(v_missile.begin() + i);       // ERASE MISSILE
                    v_bom_infor.erase(v_bom_infor.begin() + k); // ERASE BOM

                    // POST TO "SIG_EXPLOSION_PUSH" WITH DATA(explosion)
                    task_post_dynamic_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_PUSH, (uint8_t *)&explosion, sizeof(explosion));
                }
            }
        }
    }
}

/***********************************************************
* PROCESS MINE HANDLE
***********************************************************/
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