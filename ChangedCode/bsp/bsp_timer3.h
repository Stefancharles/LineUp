/******************************************************************************
* @ File name --> timer.h
* @ Author    --> By Liway.Ma
* @ Version   --> V1.0
* @ Date      --> 2017/04/25
* @ Brief     --> ͨ�ö�ʱ��2������صĺ���
*
* @ Copyright (C) 20**
* @ All rights reserved
******************************************************************************/

#ifndef _hal_timer2_h_
#define _hal_timer2_h_

/*===========================================================================*/
#ifdef __cplusplus  /* C++֧�� */
	extern "C"{
#endif

#include "stdint.h"
#include "stm32f10x_tim.h"
void TIM3_Init(void);
extern	uint16_t led2s;	
/*===========================================================================*/
#ifdef __cplusplus  /* C++֧�� */
	}
#endif
/*===========================================================================*/


#endif  /* endif hal_timer2.h */




