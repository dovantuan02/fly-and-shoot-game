#include "app_eeprom.h"

#include "eeprom.h"

#include "scr_startup.h"
#include "scr_menu.h"
#include "screen_infor.h"


static void view_scr_startup();

view_dynamic_t dyn_view_startup = {
	{
		.item_type = ITEM_TYPE_DYNAMIC,
	},
	view_scr_startup
};

view_screen_t scr_startup = {
	&dyn_view_startup,
	ITEM_NULL,
	ITEM_NULL,

	.focus_item = 0,
};

void view_scr_startup() {
#define AK_LOGO_AXIS_X		(23)
#define AK_LOGO_TEXT		(AK_LOGO_AXIS_X + 4)
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

void scr_startup_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case AC_DISPLAY_INITIAL: {
            APP_DBG_SIG("AC_DISPLAY_INITIAL\n");
            view_render.initialize();
            view_render_display_on();

            if (eeprom_read(EEPROM_HISTORY_ADDR,
                            (uint8_t *)&arr_score_history,
                            MAX_HISTORY) == EEPROM_DRIVER_OK) {
                for (uint8_t i = 0; i < MAX_HISTORY; i++) {
                    APP_DBG("SCORE HISTORY [%d] :  %d\n", i, arr_score_history[i]);
                }
            }
            // game_setting.game_mode = EASY;
            // game_setting.max_missle = 5;
            // game_setting.state_sound = 0;
            // eeprom_write(EEPROM_SETTING_ADDR, (uint8_t *)&game_setting, sizeof(game_setting));
            if (eeprom_read(EEPROM_SETTING_ADDR,
                            (uint8_t *)&game_setting,
                            sizeof(game_setting)) == EEPROM_DRIVER_OK) {
                APP_DBG("SETTING MODE   :  %d\n", game_setting.game_mode);
                APP_DBG("SETTING MISSLE :  %d\n", game_setting.max_missle);
                APP_DBG("SETTING SOUND  :  %d\n", game_setting.state_sound);
            }

            timer_set(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_LOGO, AC_DISPLAY_STARTUP_INTERVAL, TIMER_ONE_SHOT);
        } break;

        case AC_DISPLAY_BUTON_MODE_RELEASED: {
            APP_DBG_SIG("AC_DISPLAY_BUTON_MODE_RELEASED\n");
            timer_remove_attr(AC_TASK_DISPLAY_ID, AC_DISPLAY_SHOW_IDLE);
            SCREEN_TRAN(task_scr_menu_handler, &scr_menu);
        } break;

        case AC_DISPLAY_SHOW_LOGO: {
            APP_DBG_SIG("AC_DISPLAY_SHOW_LOGO\n");
            SCREEN_TRAN(task_scr_menu_handler, &scr_menu);
        } break;

        case AC_DISPLAY_SHOW_IDLE: {
            APP_DBG_SIG("AC_DISPLAY_SHOW_IDLE\n");
            SCREEN_TRAN(scr_idle_handle, &scr_idle);
        } break;

        default:
            break;
    }
}
