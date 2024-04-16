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
#include "screen_infor.h"

#include "buzzer_def.h"

/***********************************************************
* VARIABLE AND ENUM VIEW MENU
***********************************************************/
#define MAX_TITLE_MENU      (5)

static const char* arr_title_menu[MAX_TITLE_MENU] =  {"  PLAY  ", "SETTING", "TUTORIAL", "HISTORY", "  EXIT  "};

typedef enum 
{
    START       = 0,                // MODE 1
    SETTING     = 1,                // MODE 2
    TUTORIAL    = 2,                // MODE 3
    HISTORY     = 3,                // MODE 4
    EXIT        = 4                 // MODE 5
}mode_menu_t;

mode_menu_t mode_menu = START;      // SET DEFAULT MODE : START

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

// SWITCH MODE UP IN MENU
static inline mode_menu_t switch_mode_up(mode_menu_t curr_menu)
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
// SWITCH MODE DOWN IN MENU
static inline mode_menu_t switch_mode_down(mode_menu_t curr_menu)
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

// VIEW TITLE MENU SCREEN
void view_menu() {
#define NUM_CELL (3)
#define CELL_Y (17)

#define TEXT_X              (42)
#define TEXT_Y              (10)

    view_render.setTextColor(WHITE);
    view_render.setTextSize(1);
    uint8_t temp = mode_menu;
    if(temp > 2)
        temp = mode_menu - 2;
    for (int i = 0; i < NUM_CELL; i++) {
        view_render.setCursor(TEXT_X, TEXT_Y + (i * CELL_Y));
        if (i != temp) {
            view_render.drawRoundRect(ROUND_RECT_X,
                                      ROUND_RECT_Y + (i * CELL_Y),
                                      ROUND_RECT_WIDTH,
                                      ROUND_RECT_HEIGHT,
                                      3, WHITE);
            view_render.setTextColor(WHITE);
            if(mode_menu > 2)
            {
                view_render.print(arr_title_menu[i + 2]);
                continue;
            }
            view_render.print(arr_title_menu[i]);
        } else {
            view_render.setTextColor(BLACK);
            view_render.fillRoundRect(ROUND_RECT_X,
                                      ROUND_RECT_Y + (temp * CELL_Y),
                                      ROUND_RECT_WIDTH,
                                      ROUND_RECT_HEIGHT,
                                      3, WHITE);
            if(mode_menu > 2)
            {
                view_render.print(arr_title_menu[i + 2]);
                continue;
            }
            view_render.print(arr_title_menu[i]);
        }
    }
}

// SCREEN TRAN TO MODE
void view_scr_menu_switch(mode_menu_t mode)
{
    switch (mode)
    {
    case START:
        APP_DBG("GAME  START\n");
        state_game = GAME_ON;
        score = 0;
        APP_DBG("state_game : %d\n", state_game);
        SCREEN_TRAN(task_scr_game_on_handle, &scr_game_on);
        break;

    case SETTING:
        SCREEN_TRAN(task_scr_setting_handle, &scr_setting);
        break;
    
    case TUTORIAL:
        SCREEN_TRAN(task_scr_tutorial_handle, &scr_tutorial);
        break;

    case HISTORY:
        SCREEN_TRAN(task_scr_history_handle, &scr_history);
        break;

    case EXIT:
        SCREEN_TRAN(scr_idle_handle, &scr_idle);
        break;
        
    default:
        break;
    }
}

// SHOW MENU
void view_scr_menu()
{
    view_menu();
}

/***********************************************************
* SCREEN MENU HANDLE
***********************************************************/

void task_scr_menu_handler(ak_msg_t *msg)
{
    switch (msg->sig)
    {
        case AC_DISPLAY_BUTON_DOWN_PRESSED:
            APP_DBG_SIG("AC_DISPLAY_BUTON_DOWN_PRESSED\n");
            mode_menu = switch_mode_down(mode_menu);
            // APP_DBG("Mode : %d\n", mode_menu);
            timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE, AC_DISPLAY_IDLE_INTERVAL, TIMER_ONE_SHOT);      
            break;

        case AC_DISPLAY_BUTON_UP_PRESSED:
            APP_DBG_SIG("AC_DISPLAY_BUTON_UP_PRESSED\n");
            mode_menu = switch_mode_up(mode_menu);
            timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE, AC_DISPLAY_IDLE_INTERVAL, TIMER_ONE_SHOT);
            break;

        case AC_DISPLAY_BUTON_MODE_PRESSED:
            APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED\n");
            view_scr_menu_switch(mode_menu);
            
            if (game_setting.state_sound == true)
                BUZZER_PlayTones(tones_choose_menu);

            timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE, AC_DISPLAY_IDLE_INTERVAL, TIMER_ONE_SHOT);
            break;

        case AC_DISPLAY_SHOW_IDLE:
            APP_DBG_SIG("AC_DISPLAY_SHOW_IDLE");
            SCREEN_TRAN(scr_idle_handle, &scr_idle);
            break;
        default:
            break;
    }
}

