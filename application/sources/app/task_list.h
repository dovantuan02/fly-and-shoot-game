#ifndef __TASK_LIST_H__
#define __TASK_LIST_H__

#include "ak.h"
#include "task.h"

extern const task_t app_task_table[];
extern const task_polling_t app_task_polling_table[];

/*****************************************************************************/
/*  DECLARE: Internal Task ID
 *  Note: Task id MUST be increasing order.
 */
/*****************************************************************************/
enum
{
	/* SYSTEM TASKS */
	TASK_TIMER_TICK_ID,

	/* APP TASKS */
	AC_TASK_SYSTEM_ID,
	AC_TASK_FW_ID,
	AC_TASK_SHELL_ID,
	AC_TASK_LIFE_ID,
	AC_TASK_IF_ID,
	AC_TASK_RF24_IF_ID,
	AC_TASK_UART_IF_ID,
	AC_TASK_DBG_ID,
	AC_TASK_DISPLAY_ID,


	/*	 GAME ON	*/
	AC_TASK_DISPLAY_GAME_ON_ID,
	AC_TASK_PLANE_ID,
	AC_TASK_MISSLE_ID,
	AC_TASK_WALL_ID,
	AC_TASK_EXPLOSION_ID,
	AC_TASK_BOM_ID,
	AC_TASK_MINE_ID,
	AC_TASK_DISPLAY_GAME_OVER_ID,

#if defined(TASK_ZIGBEE_EN)
	AC_TASK_ZIGBEE_ID,
#endif

/* NRF24 NETWORKS */
#if defined(IF_NETWORK_NRF24_EN)
	AC_RF24_PHY_ID,
	AC_RF24_MAC_ID,
	AC_RF24_NWK_ID,
#endif

/* LINK */
#if defined(IF_LINK_UART_EN)
	AC_LINK_PHY_ID,
	AC_LINK_MAC_ID,
	AC_LINK_ID,
#endif

	/* EOT task ID */
	AK_TASK_EOT_ID,
};

/*****************************************************************************/
/*  DECLARE: Internal Polling Task ID
 *  Note: Task id MUST be increasing order.
 */
/*****************************************************************************/
enum
{
	/* APP TASKS */
	AC_TASK_POLLING_ZIGBEE_ID,
	AC_TASK_POLLING_CONSOLE_ID,

	/* EOT polling task ID */
	AK_TASK_POLLING_EOT_ID,
};

/*****************************************************************************/
/*  DECLARE: Task entry point
 */
/*****************************************************************************/
/* APP TASKS */
extern void task_system(ak_msg_t *);
extern void task_fw(ak_msg_t *);
extern void task_shell(ak_msg_t *);
extern void task_life(ak_msg_t *);
extern void task_if(ak_msg_t *);
extern void task_rf24_if(ak_msg_t *);
extern void task_uart_if(ak_msg_t *);
extern void task_dbg(ak_msg_t *);
extern void task_display(ak_msg_t *);
extern void task_zigbee(ak_msg_t *);


/* RF24 NETWORK TASK */
extern void task_rf24_phy(ak_msg_t *);
extern void task_rf24_mac(ak_msg_t *);
extern void task_rf24_nwk(ak_msg_t *);

/* LINK TASK */
extern void task_link_phy(ak_msg_t *);
extern void task_link_mac(ak_msg_t *);
extern void task_link(ak_msg_t *);

/* RF24 DEMO TASK */
extern void task_rf24_demo(ak_msg_t *);

/*****************************************************************************/
/*  DECLARE: Task polling
 */
/*****************************************************************************/
extern void task_polling_zigbee();
extern void task_polling_console();


/*	GAME TASK	*/
extern void task_dispalay_game_on_handle(ak_msg_t *);
extern void task_scr_game_over_handle(ak_msg_t *);
extern void task_prc_plane_hanle(ak_msg_t *);
extern void task_prc_missle_handle(ak_msg_t *);
extern void task_prc_wall_handle(ak_msg_t *);
extern void task_prc_explosion_handle(ak_msg_t *);
extern void task_prc_mine_handle(ak_msg_t *);
extern void task_prc_bom_handle(ak_msg_t *);
#endif //__TASK_LIST_H__
