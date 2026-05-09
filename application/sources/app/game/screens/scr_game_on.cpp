#include "scr_game_on.h"

#include <queue>

#include "app.h"
#include "app_dbg.h"
#include "fs_bitmap.h"
#include "fs_config.h"
#include "fs_missile.h"
#include "fs_core.h"
#include "fs_obstacle.h"
#include "fs_plane.h"
#include "fs_tunnel_wall.h"
#include "task_display.h"
#include "task_list.h"
#include "timer.h"

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

FsGame::FsCore* g_fs_core = nullptr;

// show infor (score and missle available)
void fs_game_view_infor_fly(int score, int missleAvailable) {
    view_render.setCursor(0, 57);
    view_render.print("SCORE:");
    view_render.print(g_fs_core->getScore());

    view_render.setCursor(55, 57);
    view_render.print(" TRIGGERS:");
    view_render.print(missleAvailable);
}

void view_scr_fs_game_on() {
    if (g_fs_core != nullptr) {
        FsGame::CrashType objCrash = g_fs_core->render();
        if (objCrash == FsGame::CrashType::PlaneCrash) {
            APP_DBG("Plane crash, game over -> score: %d\n", g_fs_core->getScore());
            timer_remove_attr(AC_TASK_DISPLAY_ID, FS_GAME_DISPLAY_ON_TICK);
            timer_remove_attr(FS_GAME_TASK_OBSTACLE_ID, FS_GAME_OBSTACLE_PUSH_SIG);        
            task_post_pure_msg(FS_GAME_TASK_DISPLAY_GAME_OVER_ID, FS_GAME_DISPLAY_OVER_ON_TICK);
        }
        else if (objCrash == FsGame::CrashType::BossCrash) {
            // Continue game
            task_post_pure_msg(FS_GAME_TASK_OBSTACLE_ID, FS_GAME_OBSTACLE_SETUP_SIG);
        }
        if (g_fs_core->needBossAppear() == true) {
            APP_DBG("Boss appear, score: %d\n", g_fs_core->getScore());
            task_post_pure_msg(FS_GAME_TASK_BOSS_ID, FS_GAME_BOSS_APPEAR_SIG);
        }
        fs_game_view_infor_fly(g_fs_core->getScore(), g_fs_core->getMisslePlane());
    }
}

/***********************************************************
 * SCREEN GAME ON HANDLE
 ***********************************************************/

void task_scr_fs_game_on_handle(ak_msg_t* msg) {
    switch (msg->sig) {
        case SCREEN_ENTRY: {
            APP_DBG_SIG("SCREEN_ENTRY\n");
            if (g_fs_core == nullptr) {
                APP_DBG("Create new screen\n");
                g_fs_core = new FsGame::FsCore(
                    [](int16_t x, int16_t y, const uint8_t* bitmap, int16_t w, int16_t h, uint16_t color) {
                        view_render.drawBitmap(x, y, bitmap, w, h, color);
                    });
            }
            // set up and reset active object
            task_post_pure_msg(FS_GAME_TASK_PLANE_ID, FS_GAME_PLANE_SETUP_SIG);
            task_post_pure_msg(FS_GAME_TASK_WALL_ID, FS_GAME_WALL_SETUP_SIG);
            task_post_pure_msg(FS_GAME_TASK_OBSTACLE_ID, FS_GAME_OBSTACLE_SETUP_SIG);

            timer_set(AC_TASK_DISPLAY_ID, FS_GAME_DISPLAY_ON_TICK, AC_GAME_DISPLAY_ON_TICK, TIMER_PERIODIC);
            break;
        }

        case FS_GAME_DISPLAY_ON_ACTIVE_OBJECT: {
            if (g_fs_core == nullptr) {
                APP_DBG("Screen is not ready\n");
                break;
            }
            FsGame::ObjectEntry obj;
            memcpy(&obj, get_data_common_msg(msg), sizeof(FsGame::ObjectEntry));
            int ret = 0;
            if ((ret = g_fs_core->addObject(obj)) != 0) {
                APP_WRN("Game core add object failed -> ret=%d\n", ret);
            }
            break;
        }
        case FS_GAME_DISPLAY_ON_CLEAR_OBJECT: {
            APP_DBG_SIG("FS_GAME_DISPLAY_ON_CLEAR_OBJECT\n");
            if (g_fs_core != nullptr) {
                APP_DBG("Clear screen\n");
                delete g_fs_core;
                g_fs_core = nullptr;
            }
            break;
        }

        case FS_GAME_DISPLAY_ON_TICK: {
            // APP_DBG_SIG("FS_GAME_DISPLAY_ON_TICK\n");
            if (g_fs_core == nullptr) {
                APP_DBG("Screen is not ready\n");
            }
            break;
        }
        default:
            break;
    }
}
