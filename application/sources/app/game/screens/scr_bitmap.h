#ifndef SRC_BITMAP_H
#define SRC_BITMAP_H
#include "pgmspace.h"

#define BUTTON_MAIN_HEIGHT  18
#define BUTTON_MAIN_WIDTH   45
extern const unsigned char button_screen_main [] PROGMEM;

#define POINTER_MENU_HEIGHT 20
#define POINTER_MENU_WIDTH  20
extern const uint8_t pointer_menu[] PROGMEM;

#define START_ICON_HEIGHT   15
#define START_ICON_WIDTH    15
extern const unsigned char start_icon [] PROGMEM;

#define SETTING_ICON_HEIGHT   15
#define SETTING_ICON_WIDTH    15
extern const unsigned char setting_icon [] PROGMEM;

#define TUTORIAL_ICON_HEIGHT   15
#define TUTORIAL_ICON_WIDTH    15
extern const unsigned char tutorial_icon [] PROGMEM;

#define EXIT_ICON_HEIGHT   15
#define EXIT_ICON_WIDTH    15
extern const unsigned char exit_icon [] PROGMEM;

#define PLANE_ICON_HEIGHT   6
#define PLANE_ICON_WIDTH    10
extern const unsigned char plane_icon [] PROGMEM;

#define MISSLE_ICON_HEIGHT   2
#define MISSLE_ICON_WIDTH    5
extern const unsigned char missle_icon [] PROGMEM;

#define BOM_ICON_HEIGHT   7
#define BOM_ICON_WIDTH    7
extern const unsigned char bom_icon [] PROGMEM;

#define EXPLOSION_ICON_HEIGHT   7
#define EXPLOSION_ICON_WIDTH    7
extern const unsigned char explosion_I_icon[] PROGMEM;
extern const unsigned char explosion_II_icon[] PROGMEM;

#define MINE_ICON_HEIGHT   5
#define MINE_ICON_WIDTH    5
extern const unsigned char mine_I_icon [] PROGMEM;
extern const unsigned char mine_II_icon [] PROGMEM;

#define MAP_HEIGHT  54
#define MAP_WIDTH   128
extern const unsigned char map_I [] PROGMEM;
extern const unsigned char map_II [] PROGMEM;

#define QR_TUTORIAL_HEIGHT      60
#define QR_TUTORIAL_WIDTH       60
extern const unsigned char QR_tutorial[] PROGMEM;

#endif