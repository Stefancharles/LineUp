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
#ifndef _RF_SI4332A_H_
#define _RF_SI4332A_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

	 
//��������
#define SI4332A_MAX_SIZE_FIFO 128

	
/*���²�����ѡ���޸�*/
//�޸���ʼ========>>			
#define FHCHA      0xFF	  //433����ģ����ƵƵ��ѡ��
#define FHSA       0xFF   //433����ģ����Ƶ����
//�޸Ľ���========<<

int SI4332A_Init(void);
int SI4332A_send_msg(const uint8_t *msg, uint8_t len, int timeout);
int SI4332A_read_msg(uint8_t *msg, uint8_t len, int timeout);
	 

#endif /* _CAN_MCP2515_H_ */
#ifdef __cplusplus
}
#endif

