#include "scr_game_on.h"

#include <queue>

#include "app.h"
#include "app_dbg.h"
#include "fs_bitmap.h"
#include "fs_config.h"
#include "fs_explosion.h"
#include "fs_missile.h"
#include "fs_object.h"
#include "fs_obstacle.h"
#include "fs_plane.h"
#include "fs_tunnel_wall.h"
#include "task_display.h"
#include "task_list.h"
#include "timer.h"

/***********************************************************
 * VARIABLE AND ARRAY BITMAP VIEW GAME ON
 ***********************************************************/

#define FS_NUM_MINE_ICON (2)
#define FS_NUM_WALL_ICON (2)
#define FS_NUM_EXPLOSION_ICON (2)

static const unsigned char* fs_arr_wall_icon[FS_NUM_WALL_ICON] = {map_I,
                                                                  map_II};

static const unsigned char* fs_arr_explosion_icon[FS_NUM_EXPLOSION_ICON] = {
    explosion_I_icon, explosion_II_icon};

/***********************************************************
 * VIEW - GAME ON
 ***********************************************************/

static void view_scr_fs_game_on();

view_dynamic_t dyn_view_game_on = {{
                                       .item_type = ITEM_TYPE_DYNAMIC,
                                   },
                                   view_scr_fs_game_on};

view_screen_t scr_game_on = {
    &dyn_view_game_on,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

FsGame::FsScreen* g_fs_screen = nullptr;

// show infor (score and missle available)
void fs_game_view_infor_fly() {
    view_render.setCursor(0, 57);
    view_render.print("SCORE:");
    view_render.print(fs_game_score);

    view_render.setCursor(55, 57);
    view_render.print(" TRIGGERS:");
    // view_render.print(FS_MAX_MISSLE - fs_vec_missile.size());
}

void view_scr_fs_game_on() {

    if (g_fs_screen) {
        g_fs_screen->render();
    }
    fs_game_view_infor_fly();
}

/***********************************************************
 * SCREEN GAME ON HANDLE
 ***********************************************************/

void task_scr_fs_game_on_handle(ak_msg_t* msg) {
    switch (msg->sig) {
        case SCREEN_ENTRY: {
            APP_DBG_SIG("SCREEN_ENTRY\n");
            if (g_fs_screen == nullptr) {
                APP_DBG("Create new screen\n");
                g_fs_screen = new FsGame::FsScreen(
                    [&view_render](int16_t x, int16_t y, const uint8_t* bitmap, int16_t w, int16_t h, uint16_t color) {
                        view_render.drawBitmap(x, y, bitmap, w, h, color);
                    });  // TODO: need clean up when exit screen
            }
            // set up and reset active object
            task_post_pure_msg(FS_GAME_TASK_PLANE_ID, FS_GAME_PLANE_SETUP_SIG);
            task_post_pure_msg(FS_GAME_TASK_WALL_ID, FS_GAME_WALL_SETUP_SIG);
            task_post_pure_msg(FS_GAME_TASK_EXPLOSION_ID, FS_GAME_EXPLOSION_RESET_SIG);
            task_post_pure_msg(FS_GAME_TASK_OBSTACLE_ID, FS_GAME_OBSTACLE_RESET_SIG);

            // set timer for display
            timer_set(AC_TASK_DISPLAY_ID, FS_GAME_DISPLAY_ON_TICK, AC_DISPLAY_GAME_ON_INTERVAL, TIMER_PERIODIC);
            break;
        }

        case FS_GAME_DISPLAY_ON_TICK: {
            task_post_pure_msg(FS_GAME_TASK_PLANE_ID, FS_GAME_PLANE_CRASH_SIG);
            task_post_pure_msg(FS_GAME_TASK_MISSLE_ID, FS_GAME_MISSLE_CRASH_SIG);
            task_post_pure_msg(FS_GAME_TASK_MISSLE_ID, FS_GAME_MISSLE_ON_TICK_SIG);
            break;
        }
        case FS_GAME_DISPLAY_ON_ADD_OBJECT: {
            // FsGame::ObjectEntry *obj = (FsGame::ObjectEntry *) get_data_common_msg(msg);
            FsGame::ObjectEntry obj;
            memcpy(&obj, get_data_common_msg(msg), sizeof(FsGame::ObjectEntry));
            g_fs_screen->addObject(obj);
            break;
        }
        case FS_GAME_DISPLAY_ON_REMOVE_OBJECT: {
            // memcpy(&obj, get_data_common_msg(msg), sizeof(FsGame::ObjectEntry));
            FsGame::ObjectEntry *obj = (FsGame::ObjectEntry *) get_data_common_msg(msg);
            // g_fs_screen->removeObject(obj->obj);
            break;
        }
        case FS_GAME_DISPLAY_ON_CLEAR_OBJECT: {
            delete g_fs_screen;
            break;
        }
        default:
            break;
    }
}
