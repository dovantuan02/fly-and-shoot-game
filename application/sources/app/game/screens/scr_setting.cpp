#include "app.h"
#include "app_dbg.h"
#include "task_display.h"
#include "screen_manager.h"
#include "app_eeprom.h"

#include "eeprom.h"

#include "scr_setting.h"
#include "scr_menu.h"
#include "fs_config.h"

/***********************************************************
* VARIABLE AND ENUM VIEW SETTING
***********************************************************/

typedef enum {
    GAME_MODE,
    MAX_MISSLE,
    STATE_SOUND, 
    EXIT
} fs_setting_option_t;

fs_setting_option_t fs_option_setting = GAME_MODE;

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

// switch mode up setting
static inline fs_setting_option_t fs_switch_option_setting_up(fs_setting_option_t curr_menu) {
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

// switch mode down setting
static inline fs_setting_option_t fs_switch_option_setting_down(fs_setting_option_t curr_menu) {
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

// switch setting game mode (easy, normal , hard)
static inline fs_game_mode_t fs_switch_game_mode( fs_game_mode_t curr_setting_game_mode) {
    switch (curr_setting_game_mode) {
        case FS_GAME_MODE_EASY:
            return FS_GAME_MODE_NORMAL;
        case FS_GAME_MODE_NORMAL:
            return FS_GAME_MODE_HARD;
        case FS_GAME_MODE_HARD:
            return FS_GAME_MODE_EASY;
        default:
            return curr_setting_game_mode;
    }
}

// show view setting
static void fs_view_setting(fs_setting_option_t view_mode) {
   view_render.setTextColor(WHITE);
    view_render.setTextSize(1);
    uint8_t temp = view_mode;
    if (temp > 2) {
        temp = view_mode - 1;
    }
    for (int i = 0; i < FS_NUM_CELL; i++) {
        view_render.setCursor(15, 10 + (i * FS_CELL_Y));
        if (i != temp) {
            view_render.drawRoundRect(FS_ROUND_RECT_X,\
                                      FS_ROUND_RECT_Y + (i * FS_CELL_Y),\
                                      FS_ROUND_RECT_WIDTH,\
                                      FS_ROUND_RECT_HEIGHT,\
                                      3, WHITE);
            view_render.setTextColor(WHITE);
            if (view_mode > 2) {
                view_render.print(arr_title_setting[i + 1]);
            }
            else {
                view_render.print(arr_title_setting[i]);
            }
        } 
        else {
            view_render.setTextColor(BLACK);
            view_render.fillRoundRect(FS_ROUND_RECT_X,\
                                      FS_ROUND_RECT_Y + (temp * FS_CELL_Y),\
                                      FS_ROUND_RECT_WIDTH,\
                                      FS_ROUND_RECT_HEIGHT,\
                                      3, WHITE);
            if (view_mode > 2) {
                view_render.print(arr_title_setting[i + 1]);
            }
            else {
                view_render.print(arr_title_setting[i]);
            }
        }
        if (view_mode > 2) {
            if (i == 0) {
                view_render.print(fs_game_setting.fs_setting_missle);
            } 
            else if (i == 1) {
                view_render.print(fs_game_setting.fs_setting_sound == false ? "OFF" : "ON");
            }
        }
        else if (view_mode <= 2) {
            if (i == 0) {
                if (fs_game_setting.fs_setting_game_mode == FS_GAME_MODE_EASY) {
                    view_render.print("EASY");
                } 
                else if (fs_game_setting.fs_setting_game_mode == FS_GAME_MODE_NORMAL) {
                    view_render.print("NORMAL");
                } 
                else {
                    view_render.print("HARD");
                }
            } 
            else if (i == 1) {
                view_render.print(fs_game_setting.fs_setting_missle);
            } 
            else if (i == 2) {
                view_render.print(fs_game_setting.fs_setting_sound == false ? "OFF" : "ON");
            }
        }
    }
}

// write setting to eeprom
static inline void write_epprom_setting()
{
    eeprom_write(EEPROM_SETTING_ADDR, (uint8_t *)&fs_game_setting, sizeof(fs_game_setting));
}

// view screen setting
void view_scr_setting() {
    fs_view_setting(fs_option_setting);
}

/***********************************************************
* SCREEN MENU HANDLE
***********************************************************/

void task_scr_setting_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SCREEN_ENTRY: {
            fs_option_setting = GAME_MODE;
            break;
        }
        case AC_DISPLAY_BUTON_UP_PRESSED: {
            fs_option_setting = fs_switch_option_setting_up(fs_option_setting);
            break;
        }
        case AC_DISPLAY_BUTON_DOWN_PRESSED: {
            fs_option_setting = fs_switch_option_setting_down(fs_option_setting);
            break;
        }
        case AC_DISPLAY_BUTON_MODE_PRESSED: {  
            // APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED - SETTING\n");
            if (fs_option_setting == GAME_MODE) {
                fs_game_setting.fs_setting_game_mode = fs_switch_game_mode(fs_game_setting.fs_setting_game_mode);
            } 
            else if (fs_option_setting == MAX_MISSLE) {
                fs_game_setting.fs_setting_missle++;
                fs_game_setting.fs_setting_missle = fs_game_setting.fs_setting_missle > 5 ? 1 : fs_game_setting.fs_setting_missle;
            } 
            else if (fs_option_setting == STATE_SOUND) {
                fs_game_setting.fs_setting_sound =
                    fs_game_setting.fs_setting_sound ^ 1;
            } 
            else {
                SCREEN_TRAN(task_scr_menu_handler, &scr_menu);
                write_epprom_setting();
            }
            break;
        }
        default:
            break;
    }
}