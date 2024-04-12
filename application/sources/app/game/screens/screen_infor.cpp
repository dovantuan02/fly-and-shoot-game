#include <vector>

#include "screen_infor.h"

using namespace std;

uint8_t score = 0;

eState_game_t state_game = GAME_OFF;

game_setting_t game_setting;

uint16_t arr_score_history[MAX_HISTORY] = {};

