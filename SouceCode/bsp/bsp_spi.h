#ifndef __BSP_SPI_H
#define __BSP_SPI_H
#include "sys.h"
#include "stm32f10x_spi.h"
void SPI2_Init(void);			 //��ʼ��SPI��
void SPI2_SetSpeed(u8 SpeedSet); //����SPI�ٶ�   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI���߶�дһ���ֽ�
uint8_t bsp_spi2_read_write_byte(uint8_t dat);

#endif



