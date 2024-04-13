#include "app.h"
#include "app_dbg.h"
#include "task_display.h"
#include "screen_manager.h"
#include "app_eeprom.h"

#include "eeprom.h"

#include "scr_setting.h"
#include "scr_menu.h"
#include "screen_infor.h"

/***********************************************************
* VARIABLE AND ENUM VIEW SETTING
***********************************************************/

typedef enum {
    GAME_MODE,
    MAX_MISSLE,
    STATE_SOUND, 
    EXIT
} mode_setting_t;

mode_setting_t mode_setting = GAME_MODE;

/***********************************************************
* VIEW - MENU
***********************************************************/

static void view_scr_setting();

static const char *arr_title_setting[4] = {"GAME MODE: ", "MISSLE   : ", "SOUND    : ", "EXIT"};

view_dynamic_t dyn_view_setting = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_setting
};

view_screen_t scr_setting = {
	&dyn_view_setting,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

// SWITCH MODE UP SETTING
static inline mode_setting_t switch_mode_up(mode_setting_t curr_menu) {
    switch (curr_menu) {
        case GAME_MODE:
            return EXIT;

        case EXIT:
            return STATE_SOUND;

        case STATE_SOUND:
            return MAX_MISSLE;

        case MAX_MISSLE:
            return GAME_MODE;

        default:
            return curr_menu;
    }
}
// SWITCH MODE DOWN SETTING
static inline mode_setting_t switch_mode_down(mode_setting_t curr_menu) {
    switch (curr_menu) {
        case GAME_MODE:
            return MAX_MISSLE;

        case MAX_MISSLE:
            return STATE_SOUND;

        case STATE_SOUND:
            return EXIT;

        case EXIT:
            return GAME_MODE;

        default:
            return curr_menu;
    }
}

// SWITCH GAME MODE (EASY, NORMAL , HARD)
static inline game_mode_t switch_game_mode(game_mode_t curr_game_mode)
{
    switch (curr_game_mode) {
        case EASY:
            return NORMAL;

        case NORMAL:
            return HARD;

        case HARD:
            return EASY;

        default:
            return curr_game_mode;
    }
}

// SHOW VIEW SETTING
static void view_setting(mode_setting_t view_mode) {
#define NUM_CELL (3)
#define CELL_Y (17) 
   view_render.setTextColor(WHITE);
    view_render.setTextSize(1);
    uint8_t temp = view_mode;
    if(temp > 2)
        temp = view_mode - 1;
    for (int i = 0; i < NUM_CELL; i++) {
        view_render.setCursor(15, 10 + (i * CELL_Y));
        if (i != temp) {
            view_render.drawRoundRect(ROUND_RECT_X,
                                      ROUND_RECT_Y + (i * CELL_Y),
                                      ROUND_RECT_WIDTH,
                                      ROUND_RECT_HEIGHT,
                                      3, WHITE);
            view_render.setTextColor(WHITE);
            if (view_mode > 2)
                view_render.print(arr_title_setting[i + 1]);

            else
                view_render.print(arr_title_setting[i]);
        } else {
            view_render.setTextColor(BLACK);
            view_render.fillRoundRect(ROUND_RECT_X,
                                      ROUND_RECT_Y + (temp * CELL_Y),
                                      ROUND_RECT_WIDTH,
                                      ROUND_RECT_HEIGHT,
                                      3, WHITE);
            if (view_mode > 2)
                view_render.print(arr_title_setting[i + 1]);

            else
                view_render.print(arr_title_setting[i]);
        }
        if (view_mode > 2) {
            if (i == 0) {
                view_render.print(game_setting.max_missle);
            } else if (i == 1) {
                view_render.print(game_setting.state_sound == false ? "OFF" : "ON");
            }
        }
        else if (view_mode <= 2) {
            if (i == 0) {
                if (game_setting.game_mode == EASY)
                    view_render.print("EASY");
                else if (game_setting.game_mode == NORMAL)
                    view_render.print("NORMAL");
                else
                    view_render.print("HARD");
            } else if (i == 1) {
                view_render.print(game_setting.max_missle);
            } else if (i == 2) {
                view_render.print(game_setting.state_sound == false ? "OFF" : "ON");
            }
        }
    }
}

// WRITE SETTING TO EEPROM
static inline void write_epprom_setting()
{
    eeprom_write(EEPROM_SETTING_ADDR, (uint8_t *)&game_setting, sizeof(game_setting));
}

// VIEW SCREEN SETTING
void view_scr_setting() {
    view_setting(mode_setting);
}

/***********************************************************
* SCREEN MENU HANDLE
***********************************************************/
void task_scr_setting_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SCREEN_ENTRY:
            mode_setting = GAME_MODE;
            break;

        case AC_DISPLAY_BUTON_UP_PRESSED:
            mode_setting = switch_mode_up(mode_setting);
            break;

        case AC_DISPLAY_BUTON_DOWN_PRESSED:
            mode_setting = switch_mode_down(mode_setting);
            break;

        case AC_DISPLAY_BUTON_MODE_LONG_PRESSED:
            break;

        case AC_DISPLAY_BUTON_MODE_PRESSED:
            // APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED - SETTING\n");
            if (mode_setting == GAME_MODE)
                game_setting.game_mode = switch_game_mode(game_setting.game_mode);
            else if (mode_setting == MAX_MISSLE) {
                game_setting.max_missle++;
                game_setting.max_missle = game_setting.max_missle > 5 ? 1 : game_setting.max_missle;
            } else if (mode_setting == STATE_SOUND) {
                game_setting.state_sound = game_setting.state_sound ^ 1;
            } else {
                SCREEN_TRAN(task_scr_menu_handler, &scr_menu);
                write_epprom_setting();
            }
            break;

        default:
            break;
    }
}