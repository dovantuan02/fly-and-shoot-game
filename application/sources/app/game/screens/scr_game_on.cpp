#include <queue>
#include "app.h"
#include "app_dbg.h"
#include "task_display.h"
#include "task_list.h"
#include "timer.h"

#include "fs_bom.h"
#include "fs_explosion.h"
#include "fs_mine.h"
#include "fs_missile.h"
#include "fs_plane.h"
#include "fs_tunnel_wall.h"
#include "scr_bitmap.h"


#include "fs_config.h"
#include "scr_game_on.h"

/***********************************************************
* VARIABLE AND ARRAY BITMAP VIEW GAME ON
***********************************************************/

#define FS_NUM_MINE_ICON        (2)
#define FS_NUM_WALL_ICON        (2)
#define FS_NUM_EXPLOSION_ICON   (2)

static const unsigned char *fs_arr_mine_icon[FS_NUM_MINE_ICON] = {
    mine_I_icon, mine_II_icon
};

static const unsigned char *fs_arr_wall_icon[FS_NUM_WALL_ICON] = {
    map_I, map_II
};

static const unsigned char *fs_arr_explosion_icon[FS_NUM_EXPLOSION_ICON] = {
    explosion_I_icon, explosion_II_icon
};

/***********************************************************
* VIEW - GAME ON
***********************************************************/

static void view_scr_game_on();

view_dynamic_t dyn_view_game_on = {
    {
        .item_type = ITEM_TYPE_DYNAMIC,
    },
    view_scr_game_on
};

view_screen_t scr_game_on = {
    &dyn_view_game_on,
    ITEM_NULL,
    ITEM_NULL,

    .focus_item = 0,
};

// show plane
void fs_game_view_plane() {
    if (fs_plane.state == FS_SHOW)
        view_render.drawBitmap(fs_plane.coordinate.x, fs_plane.coordinate.y, \
                                plane_icon, \
                                PLANE_ICON_WIDTH, PLANE_ICON_HEIGHT, \
                                WHITE);
}

// show all missile available
void fs_game_view_missle() {
    if (!fs_vec_missile.empty()) {
        for (auto _missle : fs_vec_missile) {
            if (_missle.state == FS_SHOW) {
                view_render.drawBitmap(_missle.coordinate.x, _missle.coordinate.y, \
                                        missle_icon, \
                                        MISSLE_ICON_WIDTH, MISSLE_ICON_HEIGHT, \
                                        WHITE);
            }
        }
    }
}

// show all wall
void fs_game_view_wall() {
    if (!fs_vec_wall.empty()) {
        for (uint8_t i = 0; i < NUM_WALL; i++) {
            view_render.drawBitmap(fs_vec_wall[i].x, fs_vec_wall[i].y, \
                                    fs_arr_wall_icon[fs_vec_wall[i].ver], \
                                    MAP_WIDTH, MAP_HEIGHT, \
                                    WHITE);
        }
    }
    // APP_DBG("WALL : fs_vec_wall[0].x : %d - fs_vec_wall[0].y : %d\n", fs_vec_wall[0].x, fs_vec_wall[0].y);
}

// show all explosion available
void fs_game_view_explosion() {
    for (auto _explosion : fs_vec_explosion) {
        view_render.drawBitmap(_explosion.coordinate.x, _explosion.coordinate.y,\
                               fs_arr_explosion_icon[_explosion.ver],\
                               BOM_ICON_WIDTH, BOM_ICON_HEIGHT,\
                               WHITE);
    }
}

// show all bom available
void fs_game_view_bom() {
    if (!fs_vec_bom.empty()) {
        for(auto _bom : fs_vec_bom) {
            view_render.drawBitmap(_bom.coordinate.x, _bom.coordinate.y,\
                                   bom_icon,\
                                   BOM_ICON_WIDTH, BOM_ICON_HEIGHT,\
                                   WHITE);
        }
    }
}

// show all mine available
void fs_game_view_mine() {
    if (!fs_vec_mine.empty()) {
        for (size_t i = 0; i < fs_vec_mine.size(); i++) {
            if (fs_vec_mine[i].state == FS_SHOW) {
                view_render.drawBitmap(fs_vec_mine[i].coordinate.x, fs_vec_mine[i].coordinate.y,\
                                       fs_arr_mine_icon[fs_vec_mine[i].ver],\
                                       MINE_ICON_WIDTH, MINE_ICON_HEIGHT,\
                                       WHITE);
            }
        }
    }
}

// show infor (score and missle available)
void fs_game_view_infor_fly() {
    view_render.setCursor(0, 57);
    view_render.print("SCORE:");
    view_render.print(fs_game_score);

    view_render.setCursor(55, 57);
    view_render.print(" TRIGGERS:");
    view_render.print(FS_MAX_MISSLE - fs_vec_missile.size());
}

void view_scr_game_on() {
    fs_game_view_wall();
    fs_game_view_mine();
    fs_game_view_explosion();
    fs_game_view_bom();
    fs_game_view_plane();
    fs_game_view_missle();
    fs_game_view_infor_fly();
}

/***********************************************************
* SCREEN GAME ON HANDLE
***********************************************************/

void task_scr_game_on_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SCREEN_ENTRY: {
            APP_DBG_SIG("SCREEN_ENTRY\n");
            // set up and reset active object
            task_post_pure_msg(FS_GAME_TASK_PLANE_ID         , FS_GAME_PLANE_SETUP_SIG);
            task_post_pure_msg(FS_GAME_TASK_WALL_ID          , FS_GAME_WALL_SETUP_SIG);
            task_post_pure_msg(FS_GAME_TASK_EXPLOSION_ID     , FS_GAME_EXPLOSION_RESET_SIG);
            task_post_pure_msg(FS_GAME_TASK_BOM_ID           , FS_GAME_BOM_RESET_SIG);
            task_post_pure_msg(FS_GAME_TASK_MINE_ID          , FS_GAME_MINE_RESET_SIG);

            // set timer for display 
            timer_set(AC_TASK_DISPLAY_ID,\
                      FS_GAME_DISPLAY_ON_TICK,\
                      AC_DISPLAY_GAME_ON_INTERVAL,\
                      TIMER_PERIODIC);

            // set timer for bom 150ms 
            timer_set(FS_GAME_TASK_BOM_ID,\
                      FS_GAME_BOM_ON_TICK_SIG,\
                      AC_BOM_INTERVAL,\
                      TIMER_PERIODIC);

            // set timer for move mine 150ms 
            timer_set(FS_GAME_TASK_MINE_ID,\
                      FS_GAME_MINE_ON_TICK_SIG,\
                      AC_MINE_INTERVAL,\
                      TIMER_PERIODIC);

            // set timer for move wall 150ms 
            timer_set(FS_GAME_TASK_WALL_ID,\
                      FS_GAME_WALL_ON_TICK_SIG,\
                      AC_WALL_INTERVAL,\
                      TIMER_PERIODIC);

            // set timer for move explosion 150ms 
            timer_set(FS_GAME_TASK_EXPLOSION_ID,\
                      FS_GAME_EXPLOSION_ON_TICK_SIG,\
                      AC_EXPLOSION_INTERVAL,\
                      TIMER_PERIODIC);

            // set timer for move plane 100ms 
            timer_set(FS_GAME_TASK_PLANE_ID,\
                      FS_GAME_PLANE_ON_TICK_SIG,\
                      AC_PLANE_INTERVAL,\
                      TIMER_PERIODIC);

            // set timer for add mine 1500ms 
            timer_set(FS_GAME_TASK_MINE_ID,\
                      FS_GAME_MINE_PUSH_SIG,\
                      AC_MINE_PUSH_INTERNAL,\
                      TIMER_PERIODIC);

            // set timer for add bom 1800m
            timer_set(FS_GAME_TASK_BOM_ID,\
                      FS_GAME_BOM_PUSH_SIG,\
                      AC_BOM_PUSH_INTERNAL,\
                      TIMER_PERIODIC);
            break;
        }
        case FS_GAME_DISPLAY_ON_TICK: {  
            // POST SIGNAL CHECK ALL CRASH
            task_post_pure_msg(FS_GAME_TASK_PLANE_ID    , FS_GAME_PLANE_CRASH_SIG);
            task_post_pure_msg(FS_GAME_TASK_MISSLE_ID   , FS_GAME_MISSLE_CRASH_SIG);
            task_post_pure_msg(FS_GAME_TASK_MISSLE_ID   , FS_GAME_MISSLE_ON_TICK_SIG);
            break;
        }
        default:
            break;
    }
}
