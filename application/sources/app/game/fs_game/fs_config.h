#ifndef FS_CONFIG_H
#define FS_CONFIG_H

#include "stdio.h"
#include <vector>

#include "ak.h"

using namespace std;

#define FS_NUM_CELL            (3)
#define FS_CELL_Y              (17)

#define FS_SLIDER_WIDTH    (5)
#define FS_SLIDER_HEIGHT   (15)
#define FS_ROUND_RECT_X        (5)
#define FS_ROUND_RECT_Y        (6)

#define MAX_LCD_WDITH   (128)
#define MAX_LCD_HEIGHT  (64)

#define FS_MAX_HISTORY     (12)

typedef enum 
{
    FS_GAME_ON,
    FS_GAME_OFF,
    FS_GAME_OVER
} fs_game_state_t;

typedef enum {
    FS_GAME_MODE_FISRT = 1,
    FS_GAME_MODE_EASY,
    FS_GAME_MODE_NORMAL,
    FS_GAME_MODE_HARD,
    FS_GAME_MODE_END
} fs_game_mode_t;

#define FS_GAME_MODE_OPTION_MAX     (FS_GAME_MODE_END - FS_GAME_MODE_FISRT -1)

typedef struct {
    int8_t x;
    int8_t y;
} fs_game_coordinate_t;

typedef struct {
    bool fs_setting_sound = false;
    uint8_t fs_setting_missle = 5;
    fs_game_mode_t fs_setting_game_mode = FS_GAME_MODE_EASY;
} fs_game_setting_t;

extern fs_game_state_t fs_state_game;
extern fs_game_setting_t fs_game_setting;
extern uint8_t fs_game_score;
extern uint16_t fs_game_score_history[FS_MAX_HISTORY];


#endif  //FS_CONFIG_H