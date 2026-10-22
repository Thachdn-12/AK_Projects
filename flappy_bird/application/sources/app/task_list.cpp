#include "task_list.h"
#include "timer.h"

const task_t app_task_table[] = {
	/*************************************************************************/
	/* SYSTEM TASK */
	/*************************************************************************/
	{TASK_TIMER_TICK_ID,		TASK_PRI_LEVEL_7,		task_timer_tick			},

	/*************************************************************************/
	/* APP TASK */
	/*************************************************************************/
	{AC_TASK_SYSTEM_ID			,	TASK_PRI_LEVEL_2	,	task_system			},
	{AC_TASK_FW_ID				,	TASK_PRI_LEVEL_2	,	task_fw				},
	{AC_TASK_SHELL_ID			,	TASK_PRI_LEVEL_2	,	task_shell			},
	{AC_TASK_LIFE_ID			,	TASK_PRI_LEVEL_6	,	task_life			},
	{AC_TASK_IF_ID				,	TASK_PRI_LEVEL_4	,	task_if				},
	{AC_TASK_RF24_IF_ID			,	TASK_PRI_LEVEL_4	,	task_rf24_if		},
	{AC_TASK_UART_IF_ID			,	TASK_PRI_LEVEL_4	,	task_uart_if		},
	{AC_TASK_DBG_ID				,	TASK_PRI_LEVEL_4	,	task_dbg			},
	{AC_TASK_DISPLAY_ID			,	TASK_PRI_LEVEL_4	,	task_display		},

	#if defined(TASK_ZIGBEE_EN)
	{AC_TASK_ZIGBEE_ID			,	TASK_PRI_LEVEL_4	,	task_zigbee			},
	#endif
	
	/*************************************************************************/
	/* RF24 NETWORK TASK */
	/*************************************************************************/
	#if defined (IF_NETWORK_NRF24_EN)
	{AC_RF24_PHY_ID				,	TASK_PRI_LEVEL_3	,	task_rf24_phy		},
	{AC_RF24_MAC_ID				,	TASK_PRI_LEVEL_4	,	task_rf24_mac		},
	{AC_RF24_NWK_ID				,	TASK_PRI_LEVEL_5	,	task_rf24_nwk		},
	#endif

	/*************************************************************************/
	/* LINK TASK */
	/*************************************************************************/
	#if defined (IF_LINK_UART_EN)
	{AC_LINK_PHY_ID				,	TASK_PRI_LEVEL_3	,	task_link_phy		},
	{AC_LINK_MAC_ID				,	TASK_PRI_LEVEL_4	,	task_link_mac		},
	{AC_LINK_ID					,	TASK_PRI_LEVEL_5	,	task_link			},
	#endif

	/*************************************************************************/
	/* END OF TABLE */
	/*************************************************************************/
	// Drain the tick chain before the game task accepts another input.
	{FB_GAME_ID, TASK_PRI_LEVEL_5, fb_game_handle},
	{FB_BIRD_ID, TASK_PRI_LEVEL_6, fb_bird_handle},
	{FB_PIPE_ID, TASK_PRI_LEVEL_6, fb_pipe_handle},
	{FB_COLLISION_ID, TASK_PRI_LEVEL_7, fb_collision_handle},

	{AK_TASK_EOT_ID				,	TASK_PRI_LEVEL_0	,	(pf_task)0			}
};

const task_polling_t app_task_polling_table[] = {
	{AC_TASK_POLLING_ZIGBEE_ID	,	AK_ENABLE	,	task_polling_zigbee			},
	{AC_TASK_POLLING_CONSOLE_ID	,	AK_ENABLE	,	task_polling_console		},
	{AK_TASK_POLLING_EOT_ID		,	AK_DISABLE	,	(pf_task_polling)0			},
};
