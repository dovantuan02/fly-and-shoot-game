#include <vector>

#include "app.h"
#include "task_display.h"
#include "task_list.h"

#include "fs_tunnel_wall.h"
#include "fs_plane.h"
#include "fs_config.h"
#include "fs_object.h"

#include "fs_bitmap.h"
#include "scr_game_over.h"

using namespace std;
using namespace FsGame;

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
			ObjectEntry firstWall;
			firstWall.type = ObjectType::TunnelWall;
			firstWall.obj = new FsTunnelWall(0, 0);
			task_post_common_msg(AC_TASK_DISPLAY_ID, FS_GAME_DISPLAY_ON_ACTIVE_OBJECT, (uint8_t*)&firstWall, sizeof(ObjectEntry));

			ObjectEntry secondWall;
			secondWall.type = ObjectType::TunnelWall;
			secondWall.obj = new FsTunnelWall(MAP_WIDTH, 1);
			task_post_common_msg(AC_TASK_DISPLAY_ID, FS_GAME_DISPLAY_ON_ACTIVE_OBJECT, (uint8_t*)&secondWall, sizeof(ObjectEntry));
			break;
		}

		case FS_GAME_WALL_ON_TICK_SIG: {
			break;
		}

		default:
			break;
	}
}
