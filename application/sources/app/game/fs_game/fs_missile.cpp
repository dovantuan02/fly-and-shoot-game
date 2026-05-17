#include "app.h"
#include "app_dbg.h"
#include "task_display.h"
#include "task_list.h"
#include "timer.h"
#include "buzzer.h"

#include "fs_explosion.h"
#include "fs_plane.h"
#include "fs_missile.h"
#include "fs_obstacle.h"
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
static void fs_game_missle_move() {
	if (!fs_vec_missile.empty()) {
		for (size_t i = 0; i < fs_vec_missile.size(); i++) { // scan all missile
			fs_vec_missile[i].coordinate.x += FS_MISSLE_MOVE_X; // move missile with + (x)

			if (fs_vec_missile[i].coordinate.x >= LCD_WIDTH) { // check missile(x) with lcd_width(124)
				fs_vec_missile.erase(fs_vec_missile.begin() + i); // erase missile
			}
		}
	}
}

static inline bool fs_check_coordinate_missle(fs_obstacle_info_t _obstacle, fs_missile_info_t _missle) {
	if ((_obstacle.coordinate.x - _missle.coordinate.x <= (MISSLE_ICON_WIDTH))) {
		if ((_missle.coordinate.y) >= (_obstacle.coordinate.y ) && \ 
			(_missle.coordinate.y) <= (_obstacle.coordinate.y + FS_OBSTACLE_HEIGHT - 1) ) {
				return true;
			}
	}
	return false;
}


// missile hits obstacle
static void fs_game_missle_crash() {
	for (size_t i = 0; i < fs_vec_missile.size(); i++) { // scan all missile 
		for (size_t j = 0; j < fs_vec_obstacle.size(); j++) { // scan all obstacle
			if (fs_check_coordinate_missle(fs_vec_obstacle[j], fs_vec_missile[i])) {
					// set coordinate for fs_explosion
					fs_explosion.coordinate.x = fs_vec_obstacle[j].coordinate.x;
					fs_explosion.coordinate.y = fs_vec_obstacle[j].coordinate.y;
					fs_explosion.ver = FS_EXPLOSION_VER_I;

					FS_SOUND_EXPLOSION();

					fs_vec_missile.erase(fs_vec_missile.begin() + i); // erase missile
					fs_vec_obstacle.erase(fs_vec_obstacle.begin() + j); // erase obstacle

					// post to "sig_explosion_push" with data(fs_explosion)
					task_post_pure_msg(FS_GAME_TASK_EXPLOSION_ID, FS_GAME_EXPLOSION_PUSH_SIG);
					
					fs_game_score += fs_vec_obstacle[i].score;	// get score obstacle
			}
		}
	}
}

/***********************************************************
* PROCESS MISSLE HANDLE
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