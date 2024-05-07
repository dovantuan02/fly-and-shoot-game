#include "app.h"
#include "app_dbg.h"
#include "task_list.h"
#include "screen_manager.h"

#include "qrcode.h"

#include "fs_bitmap.h"
#include "scr_tutorial.h"
#include "scr_menu.h"
#include "scr_idle.h"

/***********************************************************
* VARIABLE TUTORIAL
***********************************************************/
static void view_scr_fs_tutorial();

view_dynamic_t dyn_view_tutorial = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_fs_tutorial
};

view_screen_t scr_tutorial = {
    &dyn_view_tutorial,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

// view tutorial 
void view_scr_fs_tutorial()
{
	QRCode qrcode;
	uint8_t qrcodeData[qrcode_getBufferSize(8)];
	qrcode_initText(&qrcode, qrcodeData, 8, 0, "https://github.com/DoVanTuan2805/_fly-and-shoot-game");

	for (uint8_t y = 0; y < qrcode.size; y++) {
		for (uint8_t x = 0; x < qrcode.size; x++) {
			view_render.drawPixel(x+40, y+2, qrcode_getModule(&qrcode, x, y) ? 1 : 0);
		}
	}
}

/***********************************************************
* TASK TUTORIAL HANDLE
***********************************************************/

void task_scr_fs_tutorial_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SCREEN_ENTRY: {
            timer_set(AC_TASK_DISPLAY_ID,\
                        AC_DISPLAY_SHOW_IDLE,\
                        AC_DISPLAY_IDLE_INTERVAL,\
                        TIMER_ONE_SHOT);
            break;
        }

        case AC_DISPLAY_BUTON_MODE_PRESSED: {
            SCREEN_TRAN(task_scr_fs_menu_handler, &scr_menu);
            break;
        }

        case AC_DISPLAY_BUTON_UP_RELEASED: {
            SCREEN_TRAN(task_scr_fs_menu_handler, &scr_menu);
            break;
        }

        case AC_DISPLAY_BUTON_DOWN_RELEASED: {
            SCREEN_TRAN(task_scr_fs_menu_handler, &scr_menu);
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
