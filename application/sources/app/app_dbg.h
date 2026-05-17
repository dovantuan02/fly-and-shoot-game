#ifndef __APP_DBG_H__
#define __APP_DBG_H__

#include <string.h>
#if defined(APP_DBG_TIME_EN)
#include "sys_ctrl.h"
#endif

#include "xprintf.h"

#define KNRM "\x1B[0m"
#define KRED "\x1B[31m"
#define KGRN "\x1B[32m"
#define KYEL "\x1B[33m"
#define KBLU "\x1B[34m"
#define KMAG "\x1B[35m"
#define KCYN "\x1B[36m"
#define KWHT "\x1B[37m"

#define __SHORT_FILE__ (strrchr(__FILE__, '/') ? strrchr(__FILE__, '/') + 1 : __FILE__)
#define __DBG_POS__ __SHORT_FILE__, __LINE__
#if defined(APP_DBG_TIME_EN)
#define __DBG_TIME__ ((unsigned long)sys_ctrl_millis())
#else
#define __DBG_TIME__ (0UL)
#endif

#if defined(APP_DBG_EN)
#define APP_DBG(fmt, ...)       xprintf(KGRN "[%lu][DBG]" KCYN "[%s:%d] " KNRM fmt KNRM, __DBG_TIME__, __DBG_POS__, ##__VA_ARGS__)
#else
#define APP_DBG(fmt, ...)
#endif

#if defined(APP_ERR_EN)
#define APP_ERR(fmt, ...)       xprintf(KRED "[%lu][ERR]" KCYN "[%s:%d] " KNRM fmt KNRM, __DBG_TIME__, __DBG_POS__, ##__VA_ARGS__)
#else
#define APP_ERR(fmt, ...)
#endif

#if defined(APP_WRN_EN)
#define APP_WRN(fmt, ...)       xprintf(KYEL "[%lu][WRN]" KCYN "[%s:%d] " KNRM fmt KNRM, __DBG_TIME__, __DBG_POS__, ##__VA_ARGS__)
#else
#define APP_WRN(fmt, ...)
#endif

#if defined(APP_PRINT_EN)
#define APP_PRINT(fmt, ...)     xprintf(KWHT "[%lu][PRINT]" KCYN "[%s:%d] " KNRM fmt KNRM, __DBG_TIME__, __DBG_POS__, ##__VA_ARGS__)
#else
#define APP_PRINT(fmt, ...)
#endif

#if defined(LOGIN_PRINT_EN)
#define LOGIN_PRINT(fmt, ...)       xprintf(fmt, ##__VA_ARGS__)
#else
#define LOGIN_PRINT(fmt, ...)
#endif

#if defined(APP_DBG_SIG_EN)
#define APP_DBG_SIG(fmt, ...)   xprintf(KYEL "[%lu]-SIG->" KCYN "[%s:%d] " KNRM fmt KNRM, __DBG_TIME__, __DBG_POS__, ##__VA_ARGS__)
#else
#define APP_DBG_SIG(fmt, ...)
#endif

#endif //__APP_DBG_H__
