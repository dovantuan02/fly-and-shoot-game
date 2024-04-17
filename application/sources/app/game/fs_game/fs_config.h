#ifndef SCREEN_INFOR_H
#define SCREEN_INFOR_H

#include "stdio.h"
#include <vector>

#include "ak.h"

using namespace std;

#define FS_NUM_CELL            (3)
#define FS_CELL_Y              (17)

#define FS_ROUND_RECT_WIDTH    (118)
#define FS_ROUND_RECT_HEIGHT   (15)
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
}fs_game_state_t;

extern fs_game_state_t fs_state_game;

typedef enum 
{
    FS_HIDE,
    FS_SHOW
}fs_game_icon_state_t;

typedef enum {
    FS_GAME_MODE_EASY = 2,
    FS_GAME_MODE_NORMAL = 3,
    FS_GAME_MODE_HARD = 4
} fs_game_mode_t;

typedef struct
{
    int8_t x;
    int8_t y;
}fs_game_coordinate_t;

typedef struct
{
    bool fs_setting_sound = false;
    uint8_t fs_setting_missle = 5;
    fs_game_mode_t fs_setting_game_mode = FS_GAME_MODE_EASY;
} fs_game_setting_t;

extern fs_game_setting_t fs_game_setting;
extern uint8_t fs_game_score;
extern uint16_t fs_game_score_history[FS_MAX_HISTORY];


#endif