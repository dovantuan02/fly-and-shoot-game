#include "ak.h"
#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "fs_obstacle.h"
#include "fs_tunnel_wall.h"
#include "fs_config.h"

#include "fs_bitmap.h"

/***********************************************************
* VARIABLE PROCESS PLANE
***********************************************************/
#define FS_OBSTACLE_MAX		(4 * fs_game_setting.fs_setting_game_mode)
#define FS_OBSTACLE_MOVE_X	(4)

/*
*	TABLE MANAGER OBSTACLE 
*	User can add to obstacle_tbl:
*		- Size Bitmap MUST 5x5 pixel
*		- Increase or decrease FS_OBSTACLE_TBL
*		- Add info to obstacle_tbl with (ID, COORDINATE, BITMAP, SCORE)
*/
#define FS_OBSTACLE_TBL		(3)
const fs_obstacle_info_t obstacle_tbl[FS_OBSTACLE_TBL] = {
	{FS_BOM_ID		,	{0,0},	bom_icon	,	0},
	{FS_MINE_I_ID	,	{0,0},	mine_I_icon	,	1},
	{FS_MINE_II_ID	,	{0,0},	mine_II_icon,	2},
};
/*
*   fs_vec_obstacle : VARIABLE CONTROL OBSTACLE
*/
vector<fs_obstacle_info_t> fs_vec_obstacle;

// delete all obstacle
static inline void fs_obstacle_reset() {
	if (!fs_vec_obstacle.empty()) {
		fs_vec_obstacle.clear();
		fs_vec_obstacle.shrink_to_fit();
	}
}

// handle add obstacle to fs_vec_obstacle
static inline void fs_obstacle_add() {
	if(fs_vec_obstacle.size() < FS_OBSTACLE_MAX) {
		uint8_t bot_limmit = MAP_HEIGHT - fs_vec_limit_wall_y[1][fs_vec_limit_wall_y[1].size() - 1] - (10); // get bot limmit
		uint8_t top_limmit =              fs_vec_limit_wall_y[0][fs_vec_limit_wall_y[0].size() - 1] + (10); // get top limmit

		fs_obstacle_info_t obstacle = obstacle_tbl[rand() % FS_OBSTACLE_TBL];
		
		obstacle.coordinate.x = LCD_WIDTH;
		obstacle.coordinate.y = rand() % (bot_limmit - top_limmit + 1) + top_limmit;
		fs_vec_obstacle.push_back(obstacle);
	}
}

// handle move all obstacle
static void fs_obstacle_move() {
	if(!fs_vec_obstacle.empty()) {
		for(size_t i = 0; i < fs_vec_obstacle.size(); i++) {
			fs_vec_obstacle[i].coordinate.x -= FS_OBSTACLE_MOVE_X;
			if (fs_vec_obstacle[i].coordinate.x < 0) {
				fs_vec_obstacle.erase(fs_vec_obstacle.begin() - i);
			}
		}
	}
}

/***********************************************************
* PROCESS OBSTACLE HANDLE
***********************************************************/

void task_fs_obstacle_handle(ak_msg_t *msg) {
	switch (msg->sig)
	{
	case FS_GAME_OBSTACLE_RESET_SIG: {
		APP_DBG_SIG("FS_GAME_OBSTACLE_RESET_SIG\n");
		fs_obstacle_reset();
		break;
	}
	
	case FS_GAME_OBSTACLE_PUSH_SIG: {
		fs_obstacle_add();
		break;
	}

	case FS_GAME_OBSTACLE_ON_TICK_SIG: {
		fs_obstacle_move();
		break;
	}

	default:
		break;

	}
}