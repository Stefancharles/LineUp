/**********************************
**头文件：
**文件名: 74hc595.c 
**创建人: 
**日期:   
**功能描述: 两串 74HC595 串行移位16位输出
**占用资源: 595SI, 595SCK,595RCK
*/
#ifndef _HAL_74HC595_H_
#define _HAL_74HC595_H_

#include "stdint.h"
/*******************74HC595的信号接口*********************/
#define IC74HC595_DAT_PIN_PORT		GPIOC
#define IC74HC595_DAT_PIN_PORT_CLK	RCC_APB2Periph_GPIOC
#define IC74HC595_DAT_PIN			GPIO_Pin_8
#define IC74HC595_DAT_Init			IC74HC595_DAT_PIN_PORT_CLK,\
									IC74HC595_DAT_PIN_PORT,\
									IC74HC595_DAT_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define IC74HC595_DAT				PCout(8)

#define IC74HC595_SCK_PIN_PORT		GPIOC
#define IC74HC595_SCK_PIN_PORT_CLK	RCC_APB2Periph_GPIOC
#define IC74HC595_SCK_PIN			GPIO_Pin_9
#define IC74HC595_SCK_Init			IC74HC595_SCK_PIN_PORT_CLK,\
									IC74HC595_SCK_PIN_PORT,\
									IC74HC595_SCK_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define IC74HC595_SCK				PCout(9)

#define IC74HC595_RCK_PIN_PORT		GPIOC
#define IC74HC595_RCK_PIN_PORT_CLK	RCC_APB2Periph_GPIOC
#define IC74HC595_RCK_PIN			GPIO_Pin_12
#define IC74HC595_RCK_Init			IC74HC595_RCK_PIN_PORT_CLK,\
									IC74HC595_RCK_PIN_PORT,\
									IC74HC595_RCK_PIN,GPIO_Speed_2MHz,GPIO_Mode_Out_PP
#define IC74HC595_RCK				PCout(12)


/************************************************************
*函数：void HC595_GpioInit(void)
*功能：74HC595信号引脚对应的I/O口功能初始化
*输入：无
*输出：无
*特殊说明：无
************************************************************/
void HC595_GpioInit(void);
void HC595_close(void);
void hc595_out(uint8_t dh, uint8_t dl);

#endif
