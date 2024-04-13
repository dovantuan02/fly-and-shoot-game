#include <vector>

#include "screen_infor.h"

using namespace std;

/*
*   score : VARIABLE GLOBAL SAVE SCORE NOW
*/
uint8_t score = 0;

/*
*   state_game : VARIABLE GLOBAL STATE GAME's MANEGERMENT
*/
eState_game_t state_game = GAME_OFF;

/*
*   game_setting : VARIABLE GLOBAL SETTING GAME's MANEGERMENT
*   VARIABLE HAVE PURPOSE:
*       - SAVE SETTING TO EEPROM 
*       - READ SETTING TO EEPROM
*       - CONFIG FOR GAME(MODE, MISSLE, SOUND)
*/
game_setting_t game_setting;

/*
*   arr_score_history : VARIABLE GLOBAL HISTORY GAME's MANEGERMENT
*   VARIABLE HAVE PURPOSE:
*       - SAVE TO EEPROM
*       - READ FROM EEPROM
*       - SHOW IN SCREEN HISTORY
*/
uint16_t arr_score_history[MAX_HISTORY] = {};

