#include "scr_history.h"
#include "scr_menu.h"

#include "fs_config.h"

/***********************************************************
* VARIABLE AND STRUCT VIEW HISTORY
***********************************************************/
typedef struct {
    int8_t pointer;
    int8_t page_history;
} fs_table_setting_info_t;

fs_table_setting_info_t fs_table_setting_infor;

/***********************************************************
* VIEW - HISTORY
***********************************************************/

static void view_scr_fs_history();

view_dynamic_t dyn_view_history = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_fs_history};

view_screen_t scr_history = {
    &dyn_view_history,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

void view_scr_fs_history() {
#define TEXT_X (10)
#define TEXT_Y (10)

    view_render.setTextColor(WHITE);
    view_render.setTextSize(1);
    for (int i = 0; i < FS_NUM_CELL; i++) {
        view_render.setCursor(TEXT_X, TEXT_Y + (i * FS_CELL_Y));
        char temp[18];

        if (i != fs_table_setting_infor.pointer) {
            view_render.fillRoundRect(FS_ROUND_RECT_X,\
                                        FS_ROUND_RECT_Y + (i * FS_CELL_Y),\
                                        FS_SLIDER_WIDTH,\
                                        FS_SLIDER_HEIGHT, 1, BLACK);
            
        } 
        else {
            // view_render.setTextColor(BLACK);
            view_render.fillRoundRect(FS_ROUND_RECT_X,\
                                        FS_ROUND_RECT_Y + (i * FS_CELL_Y),\
                                        FS_SLIDER_WIDTH,\
                                        FS_SLIDER_HEIGHT, 1, WHITE);
        }
        sprintf(temp, "   SCORE %d : ", (fs_table_setting_infor.page_history * 3) + i + 1);
        view_render.print(temp);
        view_render.print(fs_game_score_history[(fs_table_setting_infor.page_history * 3) + i]);
    }
}

/***********************************************************
* SCREEN HISTORY HANDLE
***********************************************************/

void task_scr_fs_history_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SCREEN_ENTRY: {
            timer_set(AC_TASK_DISPLAY_ID,\
                        AC_DISPLAY_SHOW_IDLE,\
                        AC_DISPLAY_IDLE_INTERVAL,\
                        TIMER_ONE_SHOT);
            break;
        }

        case AC_DISPLAY_BUTON_MODE_PRESSED: {
            timer_set(AC_TASK_DISPLAY_ID,\
                        AC_DISPLAY_SHOW_IDLE,\
                        AC_DISPLAY_IDLE_INTERVAL,\
                        TIMER_ONE_SHOT);
            SCREEN_TRAN(task_scr_fs_menu_handler, &scr_menu);
            break;
        }

        case AC_DISPLAY_BUTON_UP_RELEASED: {
            APP_DBG("AC_DISPLAY_BUTON_UP_RELEASED - HISTORY\n");
            timer_set(AC_TASK_DISPLAY_ID,\
                        AC_DISPLAY_SHOW_IDLE,\
                        AC_DISPLAY_IDLE_INTERVAL,\
                        TIMER_ONE_SHOT);
            fs_table_setting_infor.pointer--;
            if (fs_table_setting_infor.pointer < 0) {
                fs_table_setting_infor.pointer = 2;
                fs_table_setting_infor.page_history--;
                if (fs_table_setting_infor.page_history < 0) {
                    fs_table_setting_infor.page_history = (FS_MAX_HISTORY / 3) - 1;
                }
            }
            break;
        }

        case AC_DISPLAY_BUTON_DOWN_RELEASED: {
            APP_DBG("AC_DISPLAY_BUTON_DOWN_RELEASED - HISTORY\n");
            timer_set(AC_TASK_DISPLAY_ID,\
                        AC_DISPLAY_SHOW_IDLE,\
                        AC_DISPLAY_IDLE_INTERVAL,\
                        TIMER_ONE_SHOT);
            fs_table_setting_infor.pointer++;
            if (fs_table_setting_infor.pointer > 2) {
                fs_table_setting_infor.pointer = 0;
                fs_table_setting_infor.page_history++;
                if (fs_table_setting_infor.page_history > (FS_MAX_HISTORY / 3) - 1) {
                    fs_table_setting_infor.page_history = 0;
                }
            }
            break;
        }

        case AC_DISPLAY_SHOW_IDLE: {
            SCREEN_TRAN(scr_idle_handle, &scr_idle);
            break;
        }

        default:
            break;
    }
}
