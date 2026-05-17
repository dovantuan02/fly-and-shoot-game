#include <queue>

#include "app.h"
#include "app_dbg.h"
#include "task_display.h"
#include "task_list.h"
#include "timer.h"

#include "fs_explosion.h"
#include "fs_missile.h"
#include "fs_plane.h"
#include "fs_tunnel_wall.h"
#include "fs_obstacle.h"
#include "fs_config.h"

#include "scr_game_on.h"
#include "fs_bitmap.h"

/***********************************************************
* VARIABLE AND ARRAY BITMAP VIEW GAME ON
***********************************************************/

#define FS_NUM_MINE_ICON        (2)
#define FS_NUM_WALL_ICON        (2)
#define FS_NUM_EXPLOSION_ICON   (2)

static const unsigned char *fs_arr_wall_icon[FS_NUM_WALL_ICON] = {
	map_I, map_II
};

static const unsigned char *fs_arr_explosion_icon[FS_NUM_EXPLOSION_ICON] = {
	explosion_I_icon, explosion_II_icon
};

/***********************************************************
* VIEW - GAME ON
***********************************************************/

static void view_scr_fs_game_on();

view_dynamic_t dyn_view_game_on = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_fs_game_on
};

view_screen_t scr_game_on = {
	&dyn_view_game_on,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

// show plane
void fs_game_view_plane() {
	if (fs_plane.visible == true)
		view_render.drawBitmap(fs_plane.coordinate.x, fs_plane.coordinate.y, \
								plane_icon, \
								PLANE_ICON_WIDTH, PLANE_ICON_HEIGHT, \
								WHITE);
}

// show all missile available
void fs_game_view_missle() {
	if (!fs_vec_missile.empty()) {
		for (auto _missle : fs_vec_missile) {
			if (_missle.visible == true) {
				view_render.drawBitmap(_missle.coordinate.x, _missle.coordinate.y, \
										missle_icon, \
										MISSLE_ICON_WIDTH, MISSLE_ICON_HEIGHT, \
										WHITE);
			}
		}
	}
}

// show all wall
void fs_game_view_wall() {
	if (!fs_vec_wall.empty()) {
		for (uint8_t i = 0; i < NUM_WALL; i++) {
			view_render.drawBitmap(fs_vec_wall[i].x, fs_vec_wall[i].y, \
									fs_arr_wall_icon[fs_vec_wall[i].ver], \
									MAP_WIDTH, MAP_HEIGHT, \
									WHITE);
		}
	}
}

// show all explosion available
void fs_game_view_explosion() {
	for (auto _explosion : fs_vec_explosion) {
		view_render.drawBitmap(_explosion.coordinate.x, _explosion.coordinate.y,\
							fs_arr_explosion_icon[_explosion.ver],\
							EXPLOSION_ICON_WIDTH, EXPLOSION_ICON_HEIGHT,\
							WHITE);
	}
}

// show all obstacle
void fs_game_view_obstacle() {
	if (!fs_vec_obstacle.empty()) {
		for(size_t i = 0; i < fs_vec_obstacle.size(); i++) {
			view_render.drawBitmap(fs_vec_obstacle[i].coordinate.x, fs_vec_obstacle[i].coordinate.y,\
									fs_vec_obstacle[i].bitmap,\
									FS_OBSTACLE_WIDTH, FS_OBSTACLE_HEIGHT,
									WHITE
									);
		}
	}
}

// show infor (score and missle available)
void fs_game_view_infor_fly() {
	view_render.setCursor(0, 57);
	view_render.print("SCORE:");
	view_render.print(fs_game_score);

	view_render.setCursor(55, 57);
	view_render.print(" TRIGGERS:");
	view_render.print(FS_MAX_MISSLE - fs_vec_missile.size());
}

void view_scr_fs_game_on() {
	fs_game_view_wall();
	fs_game_view_obstacle();
	fs_game_view_explosion();
	
	fs_game_view_plane();
	fs_game_view_missle();
	fs_game_view_infor_fly();
}

/***********************************************************
* SCREEN GAME ON HANDLE
***********************************************************/

void task_scr_fs_game_on_handle(ak_msg_t *msg) {
	switch (msg->sig) {
		case SCREEN_ENTRY: {
			APP_DBG_SIG("SCREEN_ENTRY\n");
			// set up and reset active object
			task_post_pure_msg(FS_GAME_TASK_PLANE_ID         , FS_GAME_PLANE_SETUP_SIG);
			task_post_pure_msg(FS_GAME_TASK_WALL_ID          , FS_GAME_WALL_SETUP_SIG);
			task_post_pure_msg(FS_GAME_TASK_EXPLOSION_ID     , FS_GAME_EXPLOSION_RESET_SIG);
			task_post_pure_msg(FS_GAME_TASK_OBSTACLE_ID		 , FS_GAME_OBSTACLE_RESET_SIG);
			
			// set timer for display 
			timer_set(AC_TASK_DISPLAY_ID,\
					FS_GAME_DISPLAY_ON_TICK,\
					AC_DISPLAY_GAME_ON_INTERVAL,\
					TIMER_PERIODIC);

			// set timer for move wall 150ms 
			timer_set(FS_GAME_TASK_WALL_ID,\
					FS_GAME_WALL_ON_TICK_SIG,\
					AC_WALL_INTERVAL,\
					TIMER_PERIODIC);

			// set timer for move explosion 150ms 
			timer_set(FS_GAME_TASK_EXPLOSION_ID,\
					FS_GAME_EXPLOSION_ON_TICK_SIG,\
					AC_EXPLOSION_INTERVAL,\
					TIMER_PERIODIC);

			// set timer for move plane 100ms 
			timer_set(FS_GAME_TASK_PLANE_ID,\
					FS_GAME_PLANE_ON_TICK_SIG,\
					AC_PLANE_INTERVAL,\
					TIMER_PERIODIC);
			
			// set timer add obstacle
			timer_set(FS_GAME_TASK_OBSTACLE_ID,\
					FS_GAME_OBSTACLE_PUSH_SIG,\
					AC_OBSTACLE_ADD_INTERNAL,\
					TIMER_PERIODIC);
			
			// set timer move obstacle
			timer_set(FS_GAME_TASK_OBSTACLE_ID,\
					FS_GAME_OBSTACLE_ON_TICK_SIG,\
					AC_OBSTACLE_INTERVAL,\
					TIMER_PERIODIC);
			break;
		}

		case FS_GAME_DISPLAY_ON_TICK: {  
			// POST SIGNAL CHECK ALL CRASH
			task_post_pure_msg(FS_GAME_TASK_PLANE_ID    , FS_GAME_PLANE_CRASH_SIG);
			task_post_pure_msg(FS_GAME_TASK_MISSLE_ID   , FS_GAME_MISSLE_CRASH_SIG);
			task_post_pure_msg(FS_GAME_TASK_MISSLE_ID   , FS_GAME_MISSLE_ON_TICK_SIG);
			break;
		}

		default:
			break;
	}
}
