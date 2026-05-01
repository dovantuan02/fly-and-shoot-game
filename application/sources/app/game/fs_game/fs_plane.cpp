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
#include "fs_object.h"

/***********************************************************
* VARIABLE PROCESS PLANE
***********************************************************/

#define FS_PLANE_Y_DOWN (1)
#define FS_PLANE_Y_UP   (6)
#define PLANE_START_X   (10)
#define PLANE_START_Y   (MAP_HEIGHT / 2)
/***********************************************************
* PROCESS PLANE HANDLE
***********************************************************/

void task_fs_plane_hanle(ak_msg_t *msg) {
	switch (msg->sig) {
		case FS_GAME_PLANE_SETUP_SIG: {
			APP_DBG_SIG("FS_GAME_PLANE_SETUP\n");
			// fs_game_plane_setup();
			FsGame::FsPlane *plane = new FsGame::FsPlane(plane_icon, {PLANE_START_X, PLANE_START_Y});

			FsGame::ObjectEntry objEntry;
    		objEntry.type =	FsGame::ObjectType::Plane;
    		objEntry.obj = plane;
			
			task_post_common_msg(AC_TASK_DISPLAY_ID, FS_GAME_DISPLAY_ON_ACTIVE_OBJECT, (uint8_t*)&objEntry, sizeof(FsGame::ObjectEntry));
			break;
		}

		case FS_GAME_PLANE_UP_SIG: {  
			extern FsGame::FsScreen* g_fs_screen;
			std::vector<FsGame::FsObject*> planes;
			if (g_fs_screen->getObject(planes, FsGame::ObjectType::Plane) == 0) {
				FsGame::Coordinate coor = planes[0]->getCoordinate();
				coor.y = coor.y - FS_PLANE_Y_UP - 10;
				planes[0]->move(coor);
			}
			break;
		}

		default:
			break;
	}
}