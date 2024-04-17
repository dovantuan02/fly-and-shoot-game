#include "timer.h"

#include "app.h"
#include "task_list.h"
#include "app_dbg.h"

#include "screen_manager.h"
#include "view_render.h"

#include "scr_menu.h"
#include "scr_idle.h"
#include "scr_game_on.h"
#include "scr_bitmap.h"
#include "scr_tutorial.h"
#include "scr_setting.h"
#include "scr_history.h"
#include "fs_config.h"

#include "buzzer_def.h"

/***********************************************************
* VARIABLE AND ENUM VIEW MENU
***********************************************************/
#define FS_MAX_TITLE_MENU      (5)

static const char* fs_arr_title_menu[FS_MAX_TITLE_MENU] =  {"  PLAY  ", "SETTING", "TUTORIAL", "HISTORY", "  EXIT  "};

typedef enum 
{
    START       = 0,                // OPTION 1
    SETTING     = 1,                // OPTION 2
    TUTORIAL    = 2,                // OPTION 3
    HISTORY     = 3,                // OPTION 4
    EXIT        = 4                 // OPTION 5
}fs_option_menu_t;

fs_option_menu_t fs_option_menu = START;      // SET DEFAULT OPTION : START

/***********************************************************
* VIEW - MENU
***********************************************************/
static void view_scr_menu();

view_dynamic_t dyn_view_menu = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_menu
};

view_screen_t scr_menu= {
	&dyn_view_menu,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

// switch mode up in menu
static inline fs_option_menu_t fs_switch_option_menu_up(fs_option_menu_t curr_menu)
{
    switch(curr_menu) {
        case EXIT:
            return HISTORY;
        case HISTORY:
            return TUTORIAL;
        case TUTORIAL:
            return SETTING;
        case SETTING:
            return START;
        case START:
            return EXIT;
        default :
            return curr_menu;
    }
}

// switch mode down in menu
static inline fs_option_menu_t fs_switch_option_menu_down(fs_option_menu_t curr_menu)
{
    switch(curr_menu) {
        case START:
            return SETTING;
        case SETTING:
            return TUTORIAL;
        case TUTORIAL:
            return HISTORY;
        case HISTORY:
            return EXIT;
        case EXIT:
            return START;
        default :
            return curr_menu;
    }
}

// view title menu screen
void fs_view_menu() {
#define TEXT_X              (42)
#define TEXT_Y              (10)

    view_render.setTextColor(WHITE);
    view_render.setTextSize(1);
    uint8_t temp = fs_option_menu;
    if(temp > 2)
        temp = fs_option_menu - 2;
    for (int i = 0; i < FS_NUM_CELL; i++) {
        view_render.setCursor(TEXT_X, TEXT_Y + (i * FS_CELL_Y));
        if (i != temp) {
            view_render.drawRoundRect(FS_ROUND_RECT_X,\
                                      FS_ROUND_RECT_Y + (i * FS_CELL_Y),\
                                      FS_ROUND_RECT_WIDTH,\
                                      FS_ROUND_RECT_HEIGHT,\
                                      3, WHITE);
            view_render.setTextColor(WHITE);
            if (fs_option_menu > 2) {
                view_render.print(fs_arr_title_menu[i + 2]);
                continue;
            }
            view_render.print(fs_arr_title_menu[i]);
        }
        else {
            view_render.setTextColor(BLACK);
            view_render.fillRoundRect(FS_ROUND_RECT_X,\
                                      FS_ROUND_RECT_Y + (temp * FS_CELL_Y),\
                                      FS_ROUND_RECT_WIDTH,\
                                      FS_ROUND_RECT_HEIGHT,\
                                      3, WHITE);
            if (fs_option_menu > 2) {
                view_render.print(fs_arr_title_menu[i + 2]);
                continue;
            }
            view_render.print(fs_arr_title_menu[i]);
        }
    }
}

// screen tran to mode
void fs_view_scr_menu_switch(fs_option_menu_t mode)
{
    switch (mode) {
        case START: {
            APP_DBG("GAME  START\n");
            fs_state_game = FS_GAME_ON;
            fs_game_score = 0;
            APP_DBG("fs_state_game : %d\n", fs_state_game);
            SCREEN_TRAN(task_scr_game_on_handle, &scr_game_on);
            break;
        }
        case SETTING: {
            SCREEN_TRAN(task_scr_setting_handle, &scr_setting);
            break;
        }
        case TUTORIAL: {
            SCREEN_TRAN(task_scr_tutorial_handle, &scr_tutorial);
            break;
        }
        case HISTORY: {
            SCREEN_TRAN(task_scr_history_handle, &scr_history);
            break;
        }
        case EXIT: {
            SCREEN_TRAN(scr_idle_handle, &scr_idle);
            break;
        }
        default:
            break;
    }
}

// show menu
void view_scr_menu()
{
    fs_view_menu();
}

/***********************************************************
* SCREEN MENU HANDLE
***********************************************************/

void task_scr_menu_handler(ak_msg_t* msg) {
    switch (msg->sig) {
        case AC_DISPLAY_BUTON_DOWN_PRESSED: {
            APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
            fs_option_menu = fs_switch_option_menu_down(fs_option_menu);
            // APP_DBG("Mode : %d\n", fs_option_menu);
            timer_set(AC_TASK_DISPLAY_ID,\
                        AC_DISPLAY_SHOW_IDLE,\
                        AC_DISPLAY_IDLE_INTERVAL,\
                        TIMER_ONE_SHOT);
            break;
        }
        case AC_DISPLAY_BUTON_UP_PRESSED: {
            APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
            fs_option_menu = fs_switch_option_menu_up(fs_option_menu);
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
