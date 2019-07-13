#ifndef __USART3_H
#define __USART3_H	 
#include "sys.h"  
#include "kfifo.h"

#define USART3_MAX_RECV_LEN		600					//最大接收缓存字节数
#define USART3_MAX_SEND_LEN		600					//最大发送缓存字节数
#define USART3_RX_EN 			    1				   	//0,不接收;1,接收.
extern kfifo_t usart3_fifo;
extern u8  USART3_RX_BUF[USART3_MAX_RECV_LEN]; 		//接收缓冲,最大USART3_MAX_RECV_LEN字节
extern u8  USART3_TX_BUF[USART3_MAX_SEND_LEN]; 		//发送缓冲,最大USART3_MAX_SEND_LEN字节
extern vu16 USART3_RX_STA;   						//接收数据状态

void usart3_init(u32 bound);				//串口2初始化 
int wait_response(uint8_t result, uint16_t waittime);

void u3_printf(char* fmt,...);
void test_uart3(u8 cmd);
void uart3_send_buff(u8 *buf,u8 len)  ;

#endif













