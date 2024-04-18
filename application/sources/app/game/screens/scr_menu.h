#ifndef SCR_MENU_H
#define SCR_MENU_H

#include "view_render.h"
#include "screen_manager.h"

extern view_dynamic_t dyn_view_menu;
extern view_screen_t scr_menu;
extern void task_scr_fs_menu_handler(ak_msg_t *msg);
#endif