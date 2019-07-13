/******************************************************************************
******************************************************************************
* @file    bsp/bsp_flash.h
* @author  zhao
* @version V1.0.0
* @date    2018.06.18
* @brief   flash板级管理包
******************************************************************************
******************************************************************************/

#ifndef __BSP_FLASH_H__
#define __BSP_FLASH_H__

//#include "stm32f10x.h"
#include <string.h>
#include <stdio.h>
#include "stm32f10x_flash.h"


int32_t bsp_flash_write     (uint32_t addr,uint8_t* pbuf,int32_t len);
int32_t bsp_flash_read		  (uint32_t addr,uint8_t* p_dest,int32_t len);
int32_t bsp_flash_erase		  (uint32_t address,uint32_t len);

int32_t demo_flash(void);

#endif //__BSP_FLASH_H__

