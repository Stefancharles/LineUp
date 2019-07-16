#ifndef __USART3_H
#define __USART3_H	 
#include "sys.h"  
#include "kfifo.h"

#define USART3_MAX_RECV_LEN		600					//�����ջ����ֽ���
#define USART3_MAX_SEND_LEN		600					//����ͻ����ֽ���
#define USART3_RX_EN 			    1				   	//0,������;1,����.
extern kfifo_t usart3_fifo;
extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//���ջ���,���USART3_MAX_RECV_LEN�ֽ�
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//���ͻ���,���USART3_MAX_SEND_LEN�ֽ�
extern vu16 USART3_RX_STA;   						//��������״̬

void usart3_init(u32 bound);				//����2��ʼ�� 
int wait_response(uint8_t result, uint16_t waittime);

void u3_printf(char* fmt,...);
void test_uart3(u8 cmd);
void uart3_send_buff(u8 *buf,u8 len)  ;

#endif













