#include "app.h"
#include "app_dbg.h"
#include "task_list.h"

#include "fs_explosion.h"

/***********************************************************
* VARIABLE PROCESS EXPLOSION
***********************************************************/

/*
* fs_vec_explosion : VECTOR EXPLOSION MANAGERMENT
*/
vector<fs_game_explosion_infor_t> fs_vec_explosion;

fs_game_explosion_infor_t fs_explosion = {0, 0, VER_I};            // set default for explosion

// clear all explosion
void fs_game_explosion_reset() {
    if (!fs_vec_explosion.empty()) {
        fs_vec_explosion.clear();
        fs_vec_explosion.shrink_to_fit();
    }
}

// add explsion to explosion managerment
void fs_game_explosion_push() {
    fs_vec_explosion.push_back(fs_explosion);
}

// animate explsion and erase when complete
void fs_game_explosion_show()
{
    if (!fs_vec_explosion.empty()) {
        for (size_t i = 0; i < fs_vec_explosion.size();i++) {        // scan all explosion
            if (fs_vec_explosion[i].ver == VER_I) {                  
                fs_vec_explosion[i].ver = VER_II;                    // animate explosion
            } 
            else if (fs_vec_explosion[i].ver == VER_II) {
                fs_vec_explosion.erase(fs_vec_explosion.begin() + i);     // erase explosion
            }
        }
    }
}

/***********************************************************
* PROCESS EXPLOSION HANDLE
***********************************************************/

void task_fs_explosion_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SIG_EXPLOSION_RESET: {
            fs_game_explosion_reset();
            break;
        }
        case SIG_EXPLOSION_PUSH: {  
            // APP_DBG("EXPLOSION : X : %d , Y : %d\n", fs_explosion.x, fs_explosion.y );
            fs_game_explosion_push();
            break;
        }
        case SIG_EXPLOSION_UPDATE: {
            fs_game_explosion_show();
            break;
        }

        case SIG_EXPLOSION_ON_TICK: {
            task_post_pure_msg(AC_TASK_EXPLOSION_ID, SIG_EXPLOSION_UPDATE);
            break;
        }

        default:
            break;
    }
}