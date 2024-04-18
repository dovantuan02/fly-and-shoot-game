#ifndef FS_BITMAP_H
#define FS_BITMAP_H

#include "pgmspace.h"

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

#endif  //FS_BITMAP_H