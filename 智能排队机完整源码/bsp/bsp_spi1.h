/**
  ******************************************************************************
  * @file    
  * @author  
  * @version 
  * @date    
  * @brief   
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __BSP_SPI1_H_
#define __BSP_SPI1_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

/* Public typedef ------------------------------------------------------------*/
/* Public define -------------------------------------------------------------*/
/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
//void BSP_ConfigSPI1(void);
//uint8_t BSP_SPI1SendByte(uint8_t byte);
//uint8_t BSP_SPI1ReceiveByte(void);
void bsp_spi_init(void);
uint8_t bsp_spi_read_write_byte(uint8_t dat);


#endif /* __BSP_SPI1_H_ */
#ifdef __cplusplus
}
#endif

