#include "app.h"
#include "app_dbg.h"
#include "task_list.h"

#include "fs_explosion.h"

/***********************************************************
* PROCESS EXPLOSION HANDLE
***********************************************************/

void task_fs_explosion_handle(ak_msg_t *msg) {
	switch (msg->sig) {
		case FS_GAME_EXPLOSION_SETUP_SIG: {
			break;
		}

		case FS_GAME_EXPLOSION_PUSH_SIG: {  
			break;
		}

		case FS_GAME_EXPLOSION_ON_TICK_SIG: {
			break;
		}

		default:
			break;
	}
}