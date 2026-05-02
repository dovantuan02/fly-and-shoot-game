#include "app.h"
#include "app_dbg.h"
#include "task_display.h"
#include "task_list.h"
#include "timer.h"
#include "buzzer.h"

#include "fs_plane.h"
#include "fs_missile.h"
#include "fs_obstacle.h"
#include "fs_config.h"
#include "fs_buzzer_def.h"

#include "fs_bitmap.h"
#include "fs_core.h"

using namespace FsGame;

/***********************************************************
* VARIABLE PROCESS MISSILE
***********************************************************/

#define FS_MISSLE_SETUP_X (PLANE_ICON_WIDTH)
#define FS_MISSLE_MOVE_X  (4)

void task_fs_missle_handle(ak_msg_t *msg) {
	switch (msg->sig) {
		case FS_GAME_MISSLE_PUSH_SIG: {
			APP_DBG_SIG("FS_GAME_MISSLE_PUSH\n");
			FsGame::FsMissile *missle = new FsGame::FsMissile(missle_icon, {FS_MISSLE_SETUP_X, MAX_LCD_HEIGHT / 2});
			FsGame::ObjectEntry objEntry;
			objEntry.type =	FsGame::ObjectType::Missile;
			objEntry.obj =	missle;
			task_post_common_msg(AC_TASK_DISPLAY_ID, FS_GAME_DISPLAY_ON_ACTIVE_OBJECT, (uint8_t*)&objEntry, sizeof(FsGame::ObjectEntry));
			break;
		} 
		default:
			break;
	}
}