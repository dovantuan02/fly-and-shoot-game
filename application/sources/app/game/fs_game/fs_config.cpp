#include <vector>

#include "fs_config.h"

using namespace std;

/*
*   fs_game_score : VARIABLE GLOBAL SAVE SCORE NOW
*/
uint8_t fs_game_score = 0;

/*
*   fs_state_game : VARIABLE GLOBAL STATE GAME's MANEGERMENT
*/
fs_game_state_t fs_state_game = FS_GAME_OFF;

/*
*   fs_game_setting : VARIABLE GLOBAL SETTING GAME's MANEGERMENT
*   variable have purpose:
*       - save setting to eeprom 
*       - read setting to eeprom
*       - config for game(mode, missle, sound)
*/
fs_game_setting_t fs_game_setting;

/*
*   fs_game_score_history : VARIABLE GLOBAL HISTORY GAME's MANEGERMENT
*   variable have purpose:
*       - save to eeprom
*       - read from eeprom
*       - show in screen history
*/
uint16_t fs_game_score_history[FS_MAX_HISTORY] = {};

