#include <vector>

#include "app.h"
#include "task_display.h"
#include "task_list.h"

#include "fs_tunnel_wall.h"
#include "fs_plane.h"
#include "fs_config.h"

#include "fs_bitmap.h"
#include "scr_game_over.h"

using namespace std;

/***********************************************************
* PROCESS TUNNEL HANDLE
***********************************************************/

void task_fs_wall_handle(ak_msg_t *msg) {
	switch (msg->sig) {
		case FS_GAME_WALL_RESET_SIG: {
			APP_DBG_SIG("FS_GAME_WALL_RESET\n");
			break;
		}

		case FS_GAME_WALL_SETUP_SIG: {
			break;
		}

		case FS_GAME_WALL_ON_TICK_SIG: {
			break;
		}

		default:
			break;
	}
}