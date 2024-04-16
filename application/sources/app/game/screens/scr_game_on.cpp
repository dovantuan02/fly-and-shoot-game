#include <queue>
#include "app.h"
#include "app_dbg.h"
#include "task_display.h"
#include "task_list.h"
#include "timer.h"

#include "prc_bom.h"
#include "prc_explosion.h"
#include "prc_mine.h"
#include "prc_missile.h"
#include "prc_plane.h"
#include "prc_tunnel_wall.h"
#include "scr_bitmap.h"


#include "screen_infor.h"
#include "scr_game_on.h"

/***********************************************************
* VARIABLE AND ARRAY BITMAP VIEW GAME ON
***********************************************************/

#define NUM_MINE_ICON (2)
#define NUM_WALL_ICON (2)

static const unsigned char *arr_mine_icon[NUM_MINE_ICON] = {
    mine_I, mine_II
};

static const unsigned char *arr_wall_icon[NUM_WALL_ICON] = {
    map_I, map_II
};

static const unsigned char *arr_explosion_icon[NUM_WALL_ICON] = {
    explosion_I, explosion_II
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

// SHOW PLANEs
void view_plane() {
    if (plane.state == SHOW)
        view_render.drawBitmap(plane.x, plane.y, plane_icon, PLANE_ICON_WIDTH, PLANE_ICON_HEIGHT, WHITE);
}

// SHOW MISSLEs
void view_missle() {
    if (!v_missile.empty()) {
        for (auto _missle : v_missile) {
            if (_missle.state == SHOW) {
                view_render.drawBitmap(_missle.x, _missle.y, missle_icon, MISSLE_ICON_WIDTH, MISSLE_ICON_HEIGHT, WHITE);
            }
        }
    }
}

// SHOW WALLs
void view_wall() {
    if (!v_wall.empty()) {
        for (uint8_t i = 0; i < NUM_WALL; i++) {
            view_render.drawBitmap(v_wall[i].x, v_wall[i].y, arr_wall_icon[v_wall[i].ver], MAP_WIDTH, MAP_HEIGHT, WHITE);
        }
    }
    // APP_DBG("WALL : v_wall[0].x : %d - v_wall[0].y : %d\n", v_wall[0].x, v_wall[0].y);
}

// SHOW EXPLOSIONs
void view_explosion() {
    for (auto _explosion : v_explosion) {
        view_render.drawBitmap(_explosion.x, _explosion.y,
                               arr_explosion_icon[_explosion.ver],
                               BOM_ICON_WIDTH, BOM_ICON_HEIGHT,
                               WHITE);
    }
}

// SHOW BOMs
void view_bom() {
    if (!v_bom_infor.empty()) {
        for(auto _bom : v_bom_infor) {
            view_render.drawBitmap(_bom.x, _bom.y,
                                   bom_icon,
                                   BOM_ICON_WIDTH, BOM_ICON_HEIGHT,
                                   WHITE);
        }
    }
}

// SHOW MINEs
void view_mine() {
    if (!v_mine.empty()) {
        for (size_t i = 0; i < v_mine.size(); i++) {
            if (v_mine[i].state == SHOW) {
                view_render.drawBitmap(v_mine[i].x, v_mine[i].y,
                                       arr_mine_icon[v_mine[i].ver],
                                       MINE_ICON_WIDTH, MINE_ICON_HEIGHT,
                                       WHITE);
            }
        }
    }
}

// SHOW INFOR (SCORE AND MISSLE AVAILABLE)
void view_infor_fly() {
    view_render.setCursor(0, 57);
    view_render.print("SCORE:");
    view_render.print(score);

    view_render.setCursor(55, 57);
    view_render.print(" TRIGGERS:");
    view_render.print(MAX_MISSLE - v_missile.size());
}

void view_scr_game_on() {
    view_wall();
    view_mine();
    view_explosion();
    view_bom();
    view_plane();
    view_missle();
    view_infor_fly();
}

/***********************************************************
* SCREEN GAME ON HANDLE
***********************************************************/

void task_scr_game_on_handle(ak_msg_t *msg) {
    switch (msg->sig) {
        case SCREEN_ENTRY: {
            APP_DBG_SIG("SCREEN_ENTRY\n");
            // SET UP AND RESET ACTIVE OBJECT
            task_post_pure_msg(AC_TASK_PLANE_ID         , SIG_PLANE_SETUP);
            task_post_pure_msg(AC_TASK_WALL_ID          , SIG_WALL_SETUP);
            task_post_pure_msg(AC_TASK_EXPLOSION_ID     , SIG_EXPLOSION_RESET);
            task_post_pure_msg(AC_TASK_BOM_ID           , SIG_BOM_RESET);
            task_post_pure_msg(AC_TASK_MINE_ID          , SIG_MINE_RESET);

            // SET TIMER FOR DISPLAY 
            timer_set(AC_TASK_DISPLAY_ID,
                      SIG_DISPLAY_GAME_ON_TICK,
                      AC_DISPLAY_GAME_ON_INTERVAL,
                      TIMER_PERIODIC);

            // SET TIMER FOR BOM 150ms 
            timer_set(AC_TASK_BOM_ID,
                      SIG_BOM_ON_TICK,
                      AC_BOM_INTERVAL,
                      TIMER_PERIODIC);

            // SET TIMER FOR MOVE MINE 150ms 
            timer_set(AC_TASK_MINE_ID,
                      SIG_MINE_ON_TICK,
                      AC_MINE_INTERVAL,
                      TIMER_PERIODIC);

            // SET TIMER FOR MOVE WALL 150ms 
            timer_set(AC_TASK_WALL_ID,
                      SIG_WALL_ON_TICK,
                      AC_WALL_INTERVAL,
                      TIMER_PERIODIC);

            // SET TIMER FOR MOVE EXPLOSION 150ms 
            timer_set(AC_TASK_EXPLOSION_ID,
                      SIG_EXPLOSION_ON_TICK,
                      AC_EXPLOSION_INTERVAL,
                      TIMER_PERIODIC);

            // SET TIMER FOR MOVE PLANE 100ms 
            timer_set(AC_TASK_PLANE_ID,
                      SIG_PLANE_ON_TICK,
                      AC_PLANE_INTERVAL,
                      TIMER_PERIODIC);

            // SET TIMER FOR ADD MINE 1500ms 
            timer_set(AC_TASK_MINE_ID,
                      SIG_MINE_PUSH,
                      AC_MINE_PUSH_INTERNAL,
                      TIMER_PERIODIC);

            // SET TIMER FOR ADD BOM 1800m
            timer_set(AC_TASK_BOM_ID,
                      SIG_BOM_PUSH,
                      AC_BOM_PUSH_INTERNAL,
                      TIMER_PERIODIC);
            break;
        }

        case SIG_DISPLAY_GAME_ON_TICK:
            // POST SIGNAL CHECK ALL CRASH
            task_post_pure_msg(AC_TASK_PLANE_ID, SIG_PLANE_CRASH);
            task_post_pure_msg(AC_TASK_MISSLE_ID, SIG_MISSLE_CRASH);
            task_post_pure_msg(AC_TASK_MISSLE_ID, SIG_MISSLE_ON_TICK);
            break;

        default:
            break;
    }
}
