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
vector<fs_explosion_info_t> fs_vec_explosion;

fs_explosion_info_t fs_explosion; // set default for explosion

// clear all explosion
static inline void fs_game_explosion_reset() {
	if (!fs_vec_explosion.empty()) {
		fs_vec_explosion.clear();
		fs_vec_explosion.shrink_to_fit();
	}
}

// add explsion to explosion managerment
static inline void fs_game_explosion_push() {
	fs_vec_explosion.push_back(fs_explosion);
}

// animate explsion and erase when complete
static void fs_game_explosion_update()
{
	if (!fs_vec_explosion.empty()) {
		for (size_t i = 0; i < fs_vec_explosion.size();i++) { // scan all explosion
			if (fs_vec_explosion[i].ver == FS_EXPLOSION_VER_I) {
				fs_vec_explosion[i].ver = FS_EXPLOSION_VER_II; // animate explosion
			} 
			else if (fs_vec_explosion[i].ver == FS_EXPLOSION_VER_II) {
				fs_vec_explosion[i].ver = FS_EXPLOSION_VER_III; 
			}
			else if (fs_vec_explosion[i].ver == FS_EXPLOSION_VER_III) {
				fs_vec_explosion.erase(fs_vec_explosion.begin() + i); // erase explosion
			}
		}
	}
}

/***********************************************************
* PROCESS EXPLOSION HANDLE
***********************************************************/

void task_fs_explosion_handle(ak_msg_t *msg) {
	switch (msg->sig) {
		case FS_GAME_EXPLOSION_RESET_SIG: {
			fs_game_explosion_reset();
			break;
		}

		case FS_GAME_EXPLOSION_PUSH_SIG: {  
			fs_game_explosion_push();
			break;
		}

		case FS_GAME_EXPLOSION_ON_TICK_SIG: {
			fs_game_explosion_update();
			break;
		}

		default:
			break;
	}
}