#include "timer.h"

#include "app.h"
#include "app_dbg.h"
#include "app_eeprom.h"
#include "screen_manager.h"
#include "task_list.h"

#include "eeprom.h"

#include "scr_menu.h"
#include "scr_game_on.h"
#include "scr_game_over.h"
#include "screen_infor.h"

/***********************************************************
* VIEW - GAME OVER
***********************************************************/

static void view_scr_game_over();

view_dynamic_t dyn_view_game_over = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_game_over};

view_screen_t scr_game_over = {
    &dyn_view_game_over,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

// ADD SCORE TO TABLE 
static inline void add_score(uint16_t score) {
    for (int i = MAX_HISTORY - 1; i > 0; i--) {
        arr_score_history[i] = arr_score_history[i - 1];
    }
    arr_score_history[0] = score;
}

// WRITE TABLE SCORE TO EEPROM
void write_history_epprom() {
    add_score(score);
    
    eeprom_write(EEPROM_HISTORY_ADDR,
                 (uint8_t *)&arr_score_history,
                 MAX_HISTORY);

    eeprom_read(EEPROM_HISTORY_ADDR,
                (uint8_t *)&arr_score_history,
                MAX_HISTORY);
}

// SHOW SCORE NOW
void view_scr_game_over() {
    view_render.setCursor(40, 10);
    view_render.print("GAME OVER");
    view_render.drawRoundRect(10, 25, 110, 18, 5, WHITE);
    view_render.setCursor(20, 30);
    view_render.print("YOUR SCORE : ");
    view_render.print(score);
    char temp[22];
    sprintf(temp, " MENU         RE-PLAY");
    view_render.setCursor(0, 54);
    view_render.print(temp);
}

/***********************************************************
* SCREEN GAME ON HANDLE
***********************************************************/

void task_scr_game_over_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SCREEN_ENTRY: {
            APP_DBG_SIG("SCREEN_ENTRY_GAME_OVER\n");

            // REMOVE TIMER ACTIVE OBJECTs
            timer_remove_attr(AC_TASK_MINE_ID           , SIG_MINE_ON_TICK);
            timer_remove_attr(AC_TASK_MINE_ID           , SIG_MINE_PUSH);

            timer_remove_attr(AC_TASK_WALL_ID           , SIG_WALL_ON_TICK);
            timer_remove_attr(AC_TASK_PLANE_ID          , SIG_PLANE_ON_TICK);

            timer_remove_attr(AC_TASK_EXPLOSION_ID      , SIG_EXPLOSION_ON_TICK);
            timer_remove_attr(AC_TASK_EXPLOSION_ID      , SIG_EXPLOSION_PUSH);

            timer_remove_attr(AC_TASK_DISPLAY_ID        , SIG_DISPLAY_GAME_ON_TICK);
            timer_remove_attr(AC_TASK_BOM_ID            , SIG_BOM_PUSH);

            task_post_pure_msg(AC_TASK_MINE_ID          , SIG_MINE_RESET);
            task_post_pure_msg(AC_TASK_MISSLE_ID        , SIG_MISSLE_RESET);
            task_post_pure_msg(AC_TASK_WALL_ID          , SIG_WALL_RESET);
            task_post_pure_msg(AC_TASK_BOM_ID           , SIG_BOM_RESET);
            task_post_pure_msg(AC_TASK_EXPLOSION_ID     , SIG_EXPLOSION_RESET);

            // SAVE SCORE TO HISTORY
            write_history_epprom();
            break;
        }
        case SIG_DISPLAY_GAME_OVER_DOWN_PRESSED:
            state_game = GAME_OFF;
            SCREEN_TRAN(task_scr_menu_handler, &scr_menu);
            break;

        case SIG_DISPLAY_GAME_OVER_ON_TICK:
            state_game = GAME_OVER;
            SCREEN_TRAN(task_scr_game_over_handle, &scr_game_over);
            break;

        case SIG_DISPLAY_GAME_OVER_MODE_PRESSED:
            if(state_game == GAME_OVER){
                SCREEN_TRAN(task_scr_game_on_handle, &scr_game_on);
                state_game = GAME_ON;
                score = 0;
            }
            break;

        default:
            break;
    }
}