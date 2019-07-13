#ifndef __BSP_SPI_H
#define __BSP_SPI_H
#include "sys.h"
#include "stm32f10x_spi.h"
void SPI2_Init(void);			 //初始化SPI口
void SPI2_SetSpeed(u8 SpeedSet); //设置SPI速度   
u8 SPI2_ReadWriteByte(u8 TxData);//SPI总线读写一个字节
uint8_t bsp_spi2_read_write_byte(uint8_t dat);

#endif



