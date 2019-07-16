/**********************************
**ͷ�ļ���
**�ļ���: 74hc595.c 
**������: 
**����:   
**��������: ���� 74HC595 ������λ16λ���
**ռ����Դ: 595SI, 595SCK,595RCK
*/
#ifndef _HAL_74HC595_H_
#define _HAL_74HC595_H_

#include "stdint.h"
/*******************74HC595���źŽӿ�*********************/
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
*������void HC595_GpioInit(void)
*���ܣ�74HC595�ź����Ŷ�Ӧ��I/O�ڹ��ܳ�ʼ��
*���룺��
*�������
*����˵������
************************************************************/
void HC595_GpioInit(void);
void HC595_close(void);
void hc595_out(uint8_t dh, uint8_t dl);

#endif
