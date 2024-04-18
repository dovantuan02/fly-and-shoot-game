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
    FS_SETTING_FISRT = 0,
    FS_SETTING_GAME_MODE,
    FS_SETTING_MAX_MISSLE,
    FS_SETTING_STATE_SOUND, 
    FS_SETTING_EXIT,
    FS_SETTING_END
} fs_option_setting_t;

#define FS_OPTION_SETTING_MAX       (FS_SETTING_END - FS_SETTING_FISRT - 1)
#define FS_MAX_TITLE_SETTING    (4)

fs_option_setting_t fs_option_setting = FS_SETTING_GAME_MODE;
static const char *fs_arr_title_setting[FS_MAX_TITLE_SETTING] = {"GAME MODE: ", "MISSLE   : ", "SOUND    : ", "EXIT"};

/***********************************************************
* VIEW - MENU
***********************************************************/

static void view_scr_fs_setting();

view_dynamic_t dyn_view_setting = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_fs_setting
};

view_screen_t scr_setting = {
	&dyn_view_setting,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

// switch mode down setting
static inline fs_option_setting_t fs_switch_option_setting(fs_option_setting_t curr_option, ak_msg_t *msg) {
    switch (msg->sig) {
        case AC_DISPLAY_BUTON_DOWN_PRESSED: {
            if (static_cast<int>(curr_option) > FS_OPTION_SETTING_MAX - 1) {
                return static_cast<fs_option_setting_t>(1);
            } else {
                return static_cast<fs_option_setting_t>(curr_option + 1);
            }
            break;
        }
        case AC_DISPLAY_BUTON_UP_PRESSED: {
            if (static_cast<int>(curr_option) <= 1) {
                return static_cast<fs_option_setting_t>(FS_OPTION_SETTING_MAX);
            } else {
                return static_cast<fs_option_setting_t>(curr_option - 1);
            }
            break;
        }
        default: {
            return curr_option;
            break;
        }
    }
}

// switch setting game mode (easy, normal , hard)
static inline fs_game_mode_t fs_switch_game_mode( fs_game_mode_t curr_setting_game_mode) {
    if (static_cast<int>(curr_setting_game_mode) > FS_GAME_MODE_OPTION_MAX) {
        return static_cast<fs_game_mode_t>(2);
    } else {
        return static_cast<fs_game_mode_t>(curr_setting_game_mode + 1);
    }
}

// show view setting
static void fs_view_setting() {
    view_render.setTextColor(WHITE);
    view_render.setTextSize(1);
    view_render.setTextSize(1);

    uint8_t temp_option = static_cast<int>(fs_option_setting);
    if (temp_option > FS_NUM_CELL) {
        temp_option = temp_option - FS_NUM_CELL;
    }
    for (int i = 0; i < FS_NUM_CELL; i++) {
        if (i == (temp_option - 1)) {
            view_render.fillRoundRect(FS_ROUND_RECT_X,\
                                        FS_ROUND_RECT_Y + (i * FS_CELL_Y),\
                                        FS_ROUND_RECT_WIDTH, \
                                        FS_ROUND_RECT_HEIGHT, \
                                        3, WHITE);
            view_render.setCursor(15, 10 + (i * FS_CELL_Y));
            view_render.setTextColor(BLACK);
            view_render.print(
                fs_arr_title_setting[static_cast<int>(fs_option_setting) - 1]);
        } 
        else {
            view_render.setTextColor(WHITE);
            view_render.setCursor(15, 10 + (i * FS_CELL_Y));
            view_render.drawRoundRect(FS_ROUND_RECT_X, \
                                        FS_ROUND_RECT_Y + (i * FS_CELL_Y),\
                                        FS_ROUND_RECT_WIDTH, \
                                        FS_ROUND_RECT_HEIGHT, \
                                        2, WHITE);
            if (static_cast<int>(fs_option_setting) <= 3) {
                view_render.print(fs_arr_title_setting[i]);
            } 
            else {
                if (FS_MAX_TITLE_SETTING > (FS_NUM_CELL + i)) {
                    view_render.print(fs_arr_title_setting[FS_NUM_CELL + i]);
                } 
                else {
                    view_render.drawRoundRect(FS_ROUND_RECT_X,\
                                                FS_ROUND_RECT_Y + (i * FS_CELL_Y),\
                                                FS_ROUND_RECT_WIDTH,\
                                                FS_ROUND_RECT_HEIGHT,\
                                                2, BLACK);
                }
            }
        }
        if (static_cast<int>(fs_option_setting) <= 3) {
            if (i == 0) {
                if (fs_game_setting.fs_setting_game_mode == FS_GAME_MODE_EASY) {
                    view_render.print("EASY");
                } 
                else if (fs_game_setting.fs_setting_game_mode ==
                           FS_GAME_MODE_NORMAL) {
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
                view_render.print(
                    fs_game_setting.fs_setting_sound == false ? "OFF" : "ON");
            }
        }
    }
}

static inline void fs_write_epprom_setting()
{
    eeprom_write(EEPROM_SETTING_ADDR, (uint8_t *)&fs_game_setting, sizeof(fs_game_setting));
}

// view screen setting
void view_scr_fs_setting() {
    fs_view_setting();
}

/***********************************************************
* SCREEN MENU HANDLE
***********************************************************/

void task_scr_fs_setting_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SCREEN_ENTRY: {
            fs_option_setting = FS_SETTING_GAME_MODE;
            break;
        }
        case AC_DISPLAY_BUTON_UP_PRESSED: {
            fs_option_setting = fs_switch_option_setting(fs_option_setting, msg);
            break;
        }
        case AC_DISPLAY_BUTON_DOWN_PRESSED: {
            fs_option_setting = fs_switch_option_setting(fs_option_setting, msg);
            break;
        }
        case AC_DISPLAY_BUTON_MODE_PRESSED: {  
            // APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_PRESSED - SETTING\n");
            if (fs_option_setting == FS_SETTING_GAME_MODE) {
                fs_game_setting.fs_setting_game_mode = fs_switch_game_mode(fs_game_setting.fs_setting_game_mode);
            } 
            else if (fs_option_setting == FS_SETTING_MAX_MISSLE) {
                fs_game_setting.fs_setting_missle++;
                fs_game_setting.fs_setting_missle = fs_game_setting.fs_setting_missle > 5 ? 1 : fs_game_setting.fs_setting_missle;
            } 
            else if (fs_option_setting == FS_SETTING_STATE_SOUND) {
                fs_game_setting.fs_setting_sound =
                    fs_game_setting.fs_setting_sound ^ 1;
            } 
            else {
                SCREEN_TRAN(task_scr_fs_menu_handler, &scr_menu);
                fs_write_epprom_setting();
            }
            break;
        }
        default:
            break;
    }
}