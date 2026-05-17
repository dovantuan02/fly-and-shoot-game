#include "app.h"
#include "app_dbg.h"
#include "task_display.h"
#include "screen_manager.h"
#include "buzzer.h"

#include "fs_plane.h"
#include "fs_tunnel_wall.h"
#include "fs_obstacle.h"
#include "fs_explosion.h"
#include "fs_buzzer_def.h"

#include "fs_bitmap.h"
#include "scr_game_over.h"

/***********************************************************
* VARIABLE PROCESS PLANE
***********************************************************/

#define FS_PLANE_Y_DOWN (1)
#define FS_PLANE_Y_UP   (6)

/*
*   fs_plane : VARIABLE CONTROL PLANE
*/
fs_plane_info_t fs_plane;

// play sound when plane crash
#define FS_SOUND_GAME_OVER()               \
	if (fs_game_setting.fs_setting_sound) { \
		BUZZER_PlayTones(tones_game_over);   \
	}

// plane set coordinates default when game on
static inline void fs_game_plane_setup() {
	fs_plane.coordinate.x = 5;
	fs_plane.coordinate.y = 15;
	fs_plane.visible = true;
}

// plane down every 100ms
static inline void fs_game_plane_down() {
	fs_plane.coordinate.y += FS_PLANE_Y_DOWN;
	if (fs_plane.coordinate.y > MAP_HEIGHT) {
		fs_plane.visible = false;
		fs_plane.coordinate.y = MAX_LCD_HEIGHT;
	}
}

// player press button -> plane up
static inline void fs_game_plane_up() {
	fs_plane.coordinate.y -= FS_PLANE_Y_UP;
}

// plane touch wall top or bot
static inline void fs_game_wall_crash() {
	// check plane(y) with wall limmit(x,y)
	if ((fs_plane.coordinate.y + PLANE_ICON_HEIGHT) > (MAP_HEIGHT - fs_vec_limit_wall_y[1][0]) 
		|| fs_plane.coordinate.y < fs_vec_limit_wall_y[0][0]) {     
		if (fs_plane.visible == true) {
			fs_plane.visible = false;
			// set coordinate explosion
			fs_explosion.coordinate.x = fs_plane.coordinate.x;
			fs_explosion.coordinate.y = fs_plane.coordinate.y;
			fs_explosion.ver = FS_EXPLOSION_VER_I;

			// sound buzzer
			FS_SOUND_GAME_OVER();

			// turn off timer move plane down
			timer_remove_attr(FS_GAME_TASK_PLANE_ID, FS_GAME_PLANE_ON_TICK_SIG);         
			// POST TO "FS_GAME_EXPLOSION_PUSH" WITH DATA(fs_explosion)
			task_post_pure_msg(FS_GAME_TASK_EXPLOSION_ID, FS_GAME_EXPLOSION_PUSH_SIG);
			// set timer tran to screen game over
			timer_set(FS_GAME_TASK_DISPLAY_GAME_OVER_ID, FS_GAME_DISPLAY_OVER_ON_TICK, AC_GAME_OVER_INTERNAL, TIMER_ONE_SHOT);
		}
	}
}

static inline bool fs_check_coordinate_plane(fs_obstacle_info_t _obstacle, fs_plane_info_t _plane) {
	if ((PLANE_ICON_WIDTH > abs(_obstacle.coordinate.x - _plane.coordinate.x))) {
		if ((_plane.coordinate.y >= (_obstacle.coordinate.y + 1)) &&
			(_plane.coordinate.y <= _obstacle.coordinate.y + FS_OBSTACLE_HEIGHT - 1)) {
			if (_plane.visible == true) {
				return true;
			}
		}
	}
	return false;
}

// plane touch mine
static void fs_game_obstacle_crash() {
	if (!fs_vec_obstacle.empty()) {
		for (auto _obstacle : fs_vec_obstacle) {
			// check all mine(x,y) with plane(x,y)
			if (fs_check_coordinate_plane(_obstacle, fs_plane)) {
				// set coordinate explosion
				fs_explosion.coordinate.x = fs_plane.coordinate.x;
				fs_explosion.coordinate.y = fs_plane.coordinate.y;
				fs_explosion.ver = FS_EXPLOSION_VER_I;
				
				// sound buzzer
				FS_SOUND_GAME_OVER();
				fs_plane.visible = false;

				// post to "sig_explosion_push" with data(fs_explosion)
				task_post_pure_msg(FS_GAME_TASK_EXPLOSION_ID, FS_GAME_EXPLOSION_PUSH_SIG);
				// set timer tran to screen game over
				timer_set(FS_GAME_TASK_DISPLAY_GAME_OVER_ID, FS_GAME_DISPLAY_OVER_ON_TICK, AC_GAME_OVER_INTERNAL, TIMER_ONE_SHOT);
				break;
			}
		}
	}
}

// handler check all crash(wall, mine , bom)
static inline void fs_game_plane_crash() {
	if(fs_state_game == FS_GAME_ON){
		fs_game_obstacle_crash();
		fs_game_wall_crash();
	}
}

/***********************************************************
* PROCESS PLANE HANDLE
***********************************************************/

void task_fs_plane_hanle(ak_msg_t *msg) {
	switch (msg->sig) {
		case FS_GAME_PLANE_SETUP_SIG: {
			APP_DBG_SIG("FS_GAME_PLANE_SETUP\n");
			fs_game_plane_setup();
			break;
		}

		case FS_GAME_PLANE_UP_SIG: {  
			fs_game_plane_up();
			break;
		}

		case FS_GAME_PLANE_CRASH_SIG: {  
			fs_game_plane_crash();
			break;
		}

		case FS_GAME_PLANE_ON_TICK_SIG: {
			fs_game_plane_down();
			break;
		}

		default:
			break;
	}
}