#include "ak.h"
#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "task_display.h"

#include "fs_obstacle.h"
#include "fs_tunnel_wall.h"
#include "fs_config.h"

#include "fs_bitmap.h"
#include "fs_object.h"
/***********************************************************
* PROCESS OBSTACLE HANDLE
***********************************************************/

using namespace FsGame;

typedef struct {
	const unsigned char *bitmap;
	ObstacleInfo info;
} fs_obstacle_inf_t;

static fs_obstacle_inf_t obstacle_inf[] = {
	{bom_icon, 		{ObstacleType::Boom, 	0, {0, 0}}},
	{mine_I_icon, 	{ObstacleType::MineI, 	1, {0, 0}}},
	{mine_II_icon, 	{ObstacleType::Boom, 	2, {0, 0}}}
};

void task_fs_obstacle_handle(ak_msg_t *msg) {
	switch (msg->sig)
	{
	case FS_GAME_OBSTACLE_SETUP_SIG: {
		APP_DBG_SIG("FS_GAME_OBSTACLE_SETUP_SIG\n");
		timer_set(FS_GAME_TASK_OBSTACLE_ID, FS_GAME_OBSTACLE_PUSH_SIG, 500, TIMER_PERIODIC);
		break;
	}
	
	case FS_GAME_OBSTACLE_PUSH_SIG: {
		APP_DBG_SIG("FS_GAME_OBSTACLE_PUSH_SIG\n");
		int idx = rand() % (sizeof(obstacle_inf) / sizeof(obstacle_inf[0]));

		fs_obstacle_inf_t obj = obstacle_inf[idx];
		/** _____________________
		 * |	    Wall         |
		 * |		Boom/Mine	 |
		 * |		Boom/Mine	 |
		 * |	    Wall         |
		 * |_____________________|
		 */
		obj.info.coordinate.x = MAX_LCD_WIDTH;
		obj.info.coordinate.y = rand() % MAX_LCD_HEIGHT;
		FsObstacle *obstacle = new FsObstacle(obj.bitmap, obj.info);
		ObjectEntry objEntry;
		objEntry.type =	ObjectType::Obstacle;
		objEntry.obj =	obstacle;
		// APP_DBG("Object Obstacle Info: {%d, %d}\n", obj.info.coordinate.x, obj.info.coordinate.y);

		task_post_common_msg(AC_TASK_DISPLAY_ID, FS_GAME_DISPLAY_ON_ACTIVE_OBJECT, (uint8_t*)&objEntry, sizeof(FsGame::ObjectEntry));
		break;
	}

	case FS_GAME_OBSTACLE_ON_TICK_SIG: {
		break;
	}

	default:
		break;

	}
}