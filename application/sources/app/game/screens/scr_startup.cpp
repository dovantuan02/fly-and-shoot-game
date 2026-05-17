#include "scr_startup.h"

#include "app_eeprom.h"

#include "eeprom.h"

#include "scr_menu.h"


/***********************************************************
* VARIABLE SCREEN STARTUP
***********************************************************/

/*
*   fs_game_score : VARIABLE GLOBAL SAVE SCORE NOW
*/
uint8_t fs_game_score = 0;

/*
*   fs_state_game : VARIABLE GLOBAL STATE GAME's MANEGERMENT
*/
fs_game_state_t fs_state_game = FS_GAME_OFF;

/*
*   fs_game_setting : VARIABLE GLOBAL SETTING GAME's MANEGERMENT
*   variable have purpose:
*       - save setting to eeprom 
*       - read setting to eeprom
*       - config for game(mode, missle, sound)
*/
fs_game_setting_t fs_game_setting;

/*
*   fs_game_score_history : VARIABLE GLOBAL HISTORY GAME's MANEGERMENT
*   variable have purpose:
*       - save to eeprom
*       - read from eeprom
*       - show in screen history
*/
uint16_t fs_game_score_history[FS_MAX_HISTORY] = {};

static void view_scr_fs_startup();

view_dynamic_t dyn_view_startup = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_fs_startup
};

view_screen_t scr_startup = {
	&dyn_view_startup,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void fs_init_setting() {
	fs_game_setting.fs_setting_game_mode = FS_GAME_MODE_EASY;
	fs_game_setting.fs_setting_missle = 5;
	fs_game_setting.fs_setting_sound = true;
}

// view logo startup
void view_scr_fs_startup() {
#define AK_LOGO_AXIS_X (23)
#define AK_LOGO_TEXT (AK_LOGO_AXIS_X + 4)
	/* ak logo */
	view_render.clear();
	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	view_render.setCursor(AK_LOGO_AXIS_X, 3);
	view_render.print("   __    _  _ ");
	view_render.setCursor(AK_LOGO_AXIS_X, 10);
	view_render.print("  /__\\  ( )/ )");
	view_render.setCursor(AK_LOGO_AXIS_X, 20);
	view_render.print(" /(__)\\ (   (");
	view_render.setCursor(AK_LOGO_AXIS_X, 30);
	view_render.print("(__)(__)(_)\\_)");
	view_render.setCursor(AK_LOGO_TEXT, 42);
	view_render.print("Active Kernel");
}

/***********************************************************
* TASK STARTUP HANDLE 
***********************************************************/

void task_scr_fs_startup_handle(ak_msg_t *msg) {
	switch (msg->sig) {
		case AC_DISPLAY_INITIAL: {
			APP_DBG_SIG("AC_DISPLAY_INITIAL\n");
			// init screen display
			view_render.initialize();
			view_render_display_on();
			
			// read all data from eeprom
			if (eeprom_read(EEPROM_HISTORY_ADDR,\
							(uint8_t *)&fs_game_score_history,\
							FS_MAX_HISTORY) == EEPROM_DRIVER_OK) {
				for (uint8_t i = 0; i < FS_MAX_HISTORY; i++) {
					// APP_DBG("SCORE HISTORY [%d] :  %d\n", i, fs_game_score_history[i]);
				}
			}
			if (eeprom_read(EEPROM_SETTING_ADDR,\
							(uint8_t *)&fs_game_setting,\
							sizeof(fs_game_setting)) == EEPROM_DRIVER_OK) {
				APP_DBG("GAME SETTING MODE   :  %d\n", fs_game_setting.fs_setting_game_mode);
				APP_DBG("GAME SETTING MISSLE :  %d\n", fs_game_setting.fs_setting_missle);
				APP_DBG("GAME SETTING SOUND  :  %d\n", fs_game_setting.fs_setting_sound);

				if (fs_game_setting.fs_setting_game_mode == 0 &&\
					fs_game_setting.fs_setting_missle == 0 &&\
					fs_game_setting.fs_setting_sound == 0) {

					fs_init_setting();
				}
			}

			timer_set(AC_TASK_DISPLAY_ID,\
						AC_DISPLAY_SHOW_MENU,\
						AC_DISPLAY_STARTUP_INTERVAL,\
						TIMER_ONE_SHOT);
			break;
		}

		case AC_DISPLAY_BUTON_MODE_RELEASED: {
			APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_RELEASED\n");
			timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
			SCREEN_TRAN(task_scr_fs_menu_handler, &scr_menu);
			break;
		}

		case AC_DISPLAY_SHOW_MENU: {
			APP_DBG_SIG("AC_DISPLAY_SHOW_MENU\n");
			SCREEN_TRAN(task_scr_fs_menu_handler, &scr_menu);
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
