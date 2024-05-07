#ifndef FS_BUZZER_DEF_H
#define FS_BUZZER_DEF_H

#include "buzzer.h"

static const Tone_TypeDef tones_explosion[] =
{
    {2000, 9},
    {0	 , 9},
    {2000, 9}
};

static const Tone_TypeDef tones_missle_push[] =
{
    {7500 , 8}
};

static const Tone_TypeDef tones_game_over[] = {
    {2000, 5},
    {0, 5},
    {3000, 5},
    {0, 5},
    {4000, 5},
    {0, 5},
    {1200, 6},
    {0, 8},
    {4500, 8},
    {2000, 3},
    {0, 3},
    {3000, 3},
    {0, 3},
    {4000, 3},
    {0, 3},
    {1200, 4},
    {0, 6},
    {4500, 6}
};
static const Tone_TypeDef tones_choose_menu[] =
{
    {2000, 5}
};

#endif