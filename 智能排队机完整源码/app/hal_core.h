/******************************************************************************
******************************************************************************
* @file    app/hal_core.h 
* @author  zhao
* @version V1.0.0
* @date    2018.07.17
* @update  2018.07.17 
* @brief   app core 
******************************************************************************
******************************************************************************/
#ifndef __HAL_CORE_H__
#define __HAL_CORE_H__
#ifdef __cplusplus  /* C++支持 */
	extern "C"{
#endif
#include "systick.h"
#include "key_button.h"
#include "timer.h"
#include "sys.h"
#include "usart.h"	
#include "usmart.h"	
#include "bsp_uart4.h"
#include "LCD12864.h"
#include "usart3.h"
#include "xfs5152ce.h"
#include "lf125k.h"
#include "buzzer.h"
#include "rf_si4332a.h"
#include "rf_si4332b.h"
#include "bsp_rtc.h"
#include "potocol.h"
#include "74hc595.h"
#include "ledNixietube.h"
#include "bsp_timer3.h"

/***********include app**********************/
#include "app_pwd.h"
#include "app_cloud.h"
int32_t	hal_init(void);
/*===========================================================================*/
#ifdef __cplusplus  /* C++支持 */
	}
#endif
/*===========================================================================*/
#endif

