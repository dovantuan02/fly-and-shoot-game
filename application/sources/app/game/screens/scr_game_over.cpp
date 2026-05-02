#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "app_eeprom.h"
#include "screen_manager.h"
#include "task_display.h"
#include "task_list.h"

#include "eeprom.h"

#include "scr_menu.h"
#include "scr_game_on.h"
#include "scr_game_over.h"
#include "fs_config.h"

/***********************************************************
* VIEW - GAME OVER
***********************************************************/

static void view_scr_fs_game_over();

view_dynamic_t dyn_view_game_over = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_fs_game_over};

view_screen_t scr_game_over = {
	&dyn_view_game_over,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

// add score to table 
static inline void fs_add_score(uint16_t fs_game_score) {
	for (int i = FS_MAX_HISTORY - 1; i > 0; i--) {
		fs_game_score_history[i] = fs_game_score_history[i - 1];
	}
	fs_game_score_history[0] = fs_game_score;
}

// write table score to eeprom
void fs_write_history_epprom() {
	fs_add_score(fs_game_score);
	
	eeprom_write(EEPROM_HISTORY_ADDR,\
				(uint8_t *)&fs_game_score_history,\
				FS_MAX_HISTORY);

	eeprom_read(EEPROM_HISTORY_ADDR,\
				(uint8_t *)&fs_game_score_history,\
				FS_MAX_HISTORY);
}

// show score now
void view_scr_fs_game_over() {
	view_render.setCursor(40, 10);
	view_render.print("GAME OVER");
	view_render.drawRoundRect(10, 25, 110, 18, 5, WHITE);
	view_render.setCursor(20, 30);
	view_render.print("YOUR SCORE : ");
	view_render.print(fs_game_score);
	view_render.setCursor(0, 54);
	view_render.print(" MENU         RE-PLAY");
}

/***********************************************************
* SCREEN GAME ON HANDLE
***********************************************************/

void task_scr_fs_game_over_handle(ak_msg_t *msg) {
	switch (msg->sig) {
		case SCREEN_ENTRY: {
			APP_DBG_SIG("SCREEN_ENTRY_GAME_OVER\n");
			// SAVE SCORE TO HISTORY
			fs_write_history_epprom();
			timer_set(AC_TASK_DISPLAY_ID,\
						AC_DISPLAY_SHOW_IDLE,\
						AC_DISPLAY_IDLE_INTERVAL,\
						TIMER_ONE_SHOT);
			break;
		}

		case FS_GAME_DISPLAY_OVER_DOWN_PRESSED: {
			APP_DBG_SIG("FS_GAME_DISPLAY_OVER_DOWN_PRESSED\n");
			fs_state_game = FS_GAME_OFF;
			
			// SCREEN_TRAN(task_scr_fs_menu_handler, &scr_menu);
			break;
		}

		case FS_GAME_DISPLAY_OVER_ON_TICK: {
			APP_DBG_SIG("FS_GAME_DISPLAY_OVER_ON_TICK\n");
			fs_state_game = FS_GAME_OVER;

			task_post_pure_msg(FS_GAME_TASK_DISPLAY_GAME_ON_ID, FS_GAME_DISPLAY_ON_CLEAR_OBJECT);
			SCREEN_TRAN(task_scr_fs_game_over_handle, &scr_game_over);
			break;
		}

		case FS_GAME_DISPLAY_OVER_MODE_PRESSED: {
			APP_DBG_SIG("FS_GAME_DISPLAY_OVER_MODE_PRESSED\n");
			if (fs_state_game == FS_GAME_OVER) {
				SCREEN_TRAN(task_scr_fs_game_on_handle, &scr_game_on);
				fs_state_game = FS_GAME_ON;
				fs_game_score = 0;
			}
			break;
		}

		case AC_DISPLAY_SHOW_IDLE: {
			APP_DBG_SIG("AC_DISPLAY_SHOW_IDLE\n");
			SCREEN_TRAN(scr_idle_handle, &scr_idle);
			break;
		}

		default:
			break;
	}
}