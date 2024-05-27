#include "timer.h"

#include "app.h"
#include "task_list.h"
#include "app_dbg.h"

#include "screen_manager.h"
#include "view_render.h"

#include "scr_menu.h"
#include "scr_idle.h"
#include "scr_game_on.h"
#include "scr_tutorial.h"
#include "scr_setting.h"
#include "scr_history.h"
#include "fs_bitmap.h"

#include "fs_config.h"
#include "fs_buzzer_def.h"

/***********************************************************
* VARIABLE AND ENUM VIEW MENU
***********************************************************/

#define FS_MAX_TITLE_MENU      (5)

static const char* fs_arr_title_menu[FS_MAX_TITLE_MENU] =  {"PLAY  ", "SETTING", "TUTORIAL", "HISTORY", "EXIT  "};

typedef enum {
	FS_MENU_FISRT = 0,
	FS_MENU_START,     // OPTION 1
	FS_MENU_SETTING,   // OPTION 2
	FS_MENU_TUTORIAL,  // OPTION 2
	FS_MENU_HISTORY,   // OPTION 4
	FS_MENU_EXIT,      // OPTION 5
	FS_MENU_END
} fs_option_menu_t;

#define FS_OPTION_MENU_MAX       (FS_MENU_END - FS_MENU_FISRT - 1)

fs_option_menu_t fs_option_menu = FS_MENU_START; // set default option : fs_menu_start

/***********************************************************
* VIEW - MENU
***********************************************************/
static void view_scr_fs_menu();

view_dynamic_t dyn_view_menu = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_fs_menu
};

view_screen_t scr_menu= {
	&dyn_view_menu,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

static inline fs_option_menu_t fs_switch_option_menu (fs_option_menu_t fs_curr_option_menu, ak_msg_t *msg)
{
	switch (msg->sig) {
		case AC_DISPLAY_BUTON_DOWN_PRESSED: {
			if (static_cast<int>(fs_curr_option_menu) > FS_OPTION_MENU_MAX - 1) {
				return static_cast<fs_option_menu_t>(1);
			} else {
				return static_cast<fs_option_menu_t>(fs_curr_option_menu + 1);
			}
			break;
		}

		case AC_DISPLAY_BUTON_UP_PRESSED: {
			if (static_cast<int>(fs_curr_option_menu) <= 1) {
				return static_cast<fs_option_menu_t>(FS_OPTION_MENU_MAX);
			} else {
				return static_cast<fs_option_menu_t>(fs_curr_option_menu - 1);
			}
			break;
		}

		default: {
			return fs_curr_option_menu;
			break;
		}
	}
}

// view title menu screen
void fs_view_menu() {
#define TEXT_X              (15)
#define TEXT_Y              (10)

	view_render.setTextSize(1);
	view_render.setTextColor(WHITE);
	
	uint8_t temp_option = static_cast<int>(fs_option_menu);
	if(temp_option > FS_NUM_CELL)
	{
		temp_option = temp_option - FS_NUM_CELL;
	}
	for (int i = 0; i < FS_NUM_CELL; i++) {
		view_render.setCursor(TEXT_X, TEXT_Y + (i * FS_CELL_Y));
		if(i == (temp_option - 1))
		{
			view_render.fillRoundRect(FS_ROUND_RECT_X,\
							FS_ROUND_RECT_Y + (i * FS_CELL_Y),\
							FS_SLIDER_WIDTH,\
							FS_SLIDER_HEIGHT,\
							1, WHITE);
			
			view_render.print(fs_arr_title_menu[static_cast<int>(fs_option_menu) - 1]);
		}
		else {
			view_render.fillRoundRect(FS_ROUND_RECT_X,\
										FS_ROUND_RECT_Y + (i * FS_CELL_Y),\
										FS_SLIDER_WIDTH,\
										FS_SLIDER_HEIGHT,\
										1, BLACK);
			if (static_cast<int>(fs_option_menu) <= 3) {
				view_render.print(fs_arr_title_menu[i]);
			}
			else 
			{
				if (FS_MAX_TITLE_MENU > (FS_NUM_CELL + i)) {
					view_render.print(fs_arr_title_menu[FS_NUM_CELL + i]);
				}
				else {
					view_render.fillRoundRect(FS_ROUND_RECT_X,\
										FS_ROUND_RECT_Y + (i * FS_CELL_Y),\
										FS_SLIDER_WIDTH,\
										FS_SLIDER_HEIGHT,\
										1, BLACK);
				}
			}
		} 
	}
}

// screen tran to mode
void fs_view_scr_menu_switch(fs_option_menu_t mode)
{
	switch (mode) {
		case FS_MENU_START: {
			APP_DBG("GAME  FS_MENU_START\n");
			fs_state_game = FS_GAME_ON;
			fs_game_score = 0;
			APP_DBG("fs_state_game : %d\n", fs_state_game);
			SCREEN_TRAN(task_scr_fs_game_on_handle, &scr_game_on);
			break;
		}

		case FS_MENU_SETTING: {
			SCREEN_TRAN(task_scr_fs_setting_handle, &scr_setting);
			break;
		}

		case FS_MENU_TUTORIAL: {
			SCREEN_TRAN(task_scr_fs_tutorial_handle, &scr_tutorial);
			break;
		}

		case FS_MENU_HISTORY: {
			SCREEN_TRAN(task_scr_fs_history_handle, &scr_history);
			break;
		}

		case FS_MENU_EXIT: {
			SCREEN_TRAN(scr_idle_handle, &scr_idle);
			break;
		}

		default:
			break;
	}
}

// show menu
void view_scr_fs_menu()
{
	fs_view_menu();
}

/***********************************************************
* SCREEN MENU HANDLE
***********************************************************/

void task_scr_fs_menu_handler(ak_msg_t* msg) {
	switch (msg->sig) {
		case SCREEN_ENTRY: {
			timer_set(AC_TASK_DISPLAY_ID,\
						AC_DISPLAY_SHOW_IDLE,\
						AC_DISPLAY_IDLE_INTERVAL,\
						TIMER_ONE_SHOT);
			break;
		}

		case AC_DISPLAY_BUTON_DOWN_PRESSED: {
			APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
			fs_option_menu = fs_switch_option_menu(fs_option_menu,  msg);

			timer_set(AC_TASK_DISPLAY_ID,\
						AC_DISPLAY_SHOW_IDLE,\
						AC_DISPLAY_IDLE_INTERVAL,\
						TIMER_ONE_SHOT);
			break;
		}

		case AC_DISPLAY_BUTON_UP_PRESSED: {
			APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
			fs_option_menu = fs_switch_option_menu(fs_option_menu, msg);
			timer_set(AC_TASK_DISPLAY_ID,\
						AC_DISPLAY_SHOW_IDLE,\
						AC_DISPLAY_IDLE_INTERVAL,\
						TIMER_ONE_SHOT);
			break;
		}

		case AC_DISPLAY_BUTON_MODE_PRESSED: {
			APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
			fs_view_scr_menu_switch(fs_option_menu);

			if (fs_game_setting.fs_setting_sound == true) {
				BUZZER_PlayTones(tones_choose_menu);
			}

			timer_set(AC_TASK_DISPLAY_ID,\
						AC_DISPLAY_SHOW_IDLE,\
						AC_DISPLAY_IDLE_INTERVAL,\
						TIMER_ONE_SHOT);
			break;
		}

		case AC_DISPLAY_SHOW_IDLE: {
			APP_DBG_SIG("AC_DISPLAY_SHOW_IDLE");
			SCREEN_TRAN(scr_idle_handle, &scr_idle);
			break;
		}

		default:
			break;
	}
}
