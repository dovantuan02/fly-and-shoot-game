#ifndef __APP_EEPROM_H__
#define __APP_EEPROM_H__

#include <stdint.h>
#include "app.h"

/**
  *****************************************************************************
  * EEPROM define address.
  *
  *****************************************************************************
  */
#define EEPROM_START_ADDR								(0X0000)
#define EEPROM_END_ADDR									(0X1000)

#define EEPROM_HISTORY_ADDR								(0X0010)

#define EEPROM_SETTING_ADDR								(0X00fa)
#endif //__APP_EEPROM_H__
