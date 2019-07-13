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
#ifndef _RF_SI4332B_H_
#define _RF_SI4332B_H_

/* Includes ------------------------------------------------------------------*/
#include "stm32f10x.h"

	 
//��������
#define SI4332_MAX_SIZE_FIFO 128


	
	
	
	
	
/*���²�����ѡ���޸�*/
//�޸���ʼ========>>
#define FHCHB     0x00	 //433����ģ����ƵƵ��ѡ�� 0~43
#define FHSB      0x00	 //433����ģ����Ƶ����     0~43
//�޸Ľ���========<<


int SI4332B_Init(void);
int SI4332B_send_msg(const uint8_t *msg, uint8_t len, int timeout);
int SI4332B_read_msg(uint8_t *msg, uint8_t len, int timeout);
	 

#endif /* _CAN_MCP2515_H_ */
#ifdef __cplusplus
}
#endif

