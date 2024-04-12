#ifndef SCREEN_INFOR_H
#define SCREEN_INFOR_H

#include "stdio.h"
#include <vector>

#include "ak.h"

using namespace std;

#define NUM_CELL            (3)
#define CELL_Y              (17)

#define ROUND_RECT_WIDTH    (118)
#define ROUND_RECT_HEIGHT   (15)
#define ROUND_RECT_X        (5)
#define ROUND_RECT_Y        (6)

#define MAX_LCD_WDITH   (128)
#define MAX_LCD_HEIGHT  (64)

#define MAX_HISTORY     (12)

typedef enum 
{
    GAME_ON,
    GAME_OFF,
    GAME_OVER
}eState_game_t;

extern eState_game_t state_game;

typedef enum 
{
    HIDE,
    SHOW
}icon_state_t;

typedef enum {
    EASY = 2,
    NORMAL = 3,
    HARD = 4
} game_mode_t;

typedef struct
{
    bool state_sound = false;
    uint8_t max_missle = 5;
    game_mode_t game_mode = EASY;
} game_setting_t;

extern game_setting_t game_setting;

extern uint8_t score;
extern uint16_t arr_score_history[MAX_HISTORY];


#endif