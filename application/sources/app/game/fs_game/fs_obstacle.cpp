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
* PROCESS OBSTACLE HANDLE
***********************************************************/

void task_fs_obstacle_handle(ak_msg_t *msg) {
	switch (msg->sig)
	{
	case FS_GAME_OBSTACLE_RESET_SIG: {
		APP_DBG_SIG("FS_GAME_OBSTACLE_RESET_SIG\n");
		break;
	}
	
	case FS_GAME_OBSTACLE_PUSH_SIG: {
		break;
	}

	case FS_GAME_OBSTACLE_ON_TICK_SIG: {
		break;
	}

	default:
		break;

	}
}