/******************************************************************************
******************************************************************************
* @file    app/potocol.c 
* @author  zhao
* @version V1.0.0
* @date    2018.07.17
* @update  2018.07.17 
* @brief   si4432协议处理
******************************************************************************
******************************************************************************/

/**************include*************************/
#include "potocol.h"
#include "debug_printf.h"
#include "stdbool.h"
#include "string.h"
#include "stdlib.h"
#include "kfifo.h"
#include "delay.h"

/**************参数*************************/
#define PAYLOAD_SIZE 8
static uint8_t payload[PAYLOAD_SIZE];//测试数据
static uint8_t payload_rx[PAYLOAD_SIZE];//测试数据
extern volatile char number_write;
extern volatile char number_read;
extern volatile char number_show;
extern volatile char number_get;



/*
  * @brief  static bool compare(const uint8_t *buf_1, const uint8_t *buf_2, uint16_t len)
  * @param  buf_1   比较参数1
            buf_2   比较参数2
            len     比较长度
  * @note     
  * @Date:  2017.7.10
  * @updatge:2017.7.10
  * @author:zhao
  * @return:bool
*/
static bool compare(const uint8_t *buf_1, const uint8_t *buf_2, uint16_t len){
	int i;
	for(i=0;i<len;i++){
		if(buf_1[i] != buf_2[i])
			return false;
	}
	return true;
} 



/*
  * @brief  打印数据
  * @param  buf    打印数组
            len    打印长度
  * @note     
  * @Date:  2017.7.10
  * @updatge:2017.7.10
  * @author:zhao
  * @return:bool
*/
static void dump(const uint8_t *buf, int len)
{
	int i;
	for(i=0;i<len;i++)
	{
		printf("%x ", buf[i]);
	}
	printf("\r\n");
} 



/*
  * @brief  无线数据处理
  * @param  buf    数据指针
            len    数据长度
  * @note     
  * @Date:  2017.7.10
  * @updatge:2017.7.10
  * @author:zhao
  * @return:bool
*/
bool si4432_rx_handle(const uint8_t *buf, int len)
{
	printf("ISM Get data: ");
	dump(buf, len);
	return	compare(payload, buf, PAYLOAD_SIZE);
}

/****************************************************************************
FUNCTION		: Crc16Calc
DESCRIPTION		: UART发送
INPUT			: None
OUTPUT			: None
NOTICE			: 
DATE			: 2016/06/24
 *****************************************************************************/
static uint16_t Crc16Calc(uint8_t *data_arr, uint8_t data_len) {
      uint16_t crc16 = 0;
    uint8_t i;
    for (i = 2; i < (data_len + 2); i++) {
        crc16 = (uint16_t) ((crc16 >> 8) | (crc16 << 8));
        crc16 ^= data_arr[i];
        crc16 ^= (uint16_t) ((crc16 & 0xFF) >> 4);
        crc16 ^= (uint16_t) ((crc16 << 8) << 4);
        crc16 ^= (uint16_t) (((crc16 & 0xFF) << 4) << 1);
    }
    return crc16;
}


/*
  * @brief  Tx_protocol 发送程序
  * @param 
  * @note   FE FE cmd length data crch crcl BB  
  * @Date:  2017.7.10
  * @updatge:2017.7.10
  * @author:zhao
  * @return:void
*/
void Tx_protocol(void)
{
	uint16_t crc16 = 0;

	uint8_t tx_buf[8];
	uint8_t tx_cnt=0;
	tx_buf[tx_cnt++]=0xFE;
	tx_buf[tx_cnt++]=0xFE;
	tx_buf[tx_cnt++]=0x00;  //cmd
    tx_buf[tx_cnt++]=0x01;  //length
	tx_buf[tx_cnt++]=number_show;
	crc16 = Crc16Calc(tx_buf, 3);
	tx_buf[tx_cnt++]=crc16>>8;
	tx_buf[tx_cnt++]=crc16;
	tx_buf[tx_cnt++]=0xBB;
	SI4332A_send_msg((const uint8_t *)tx_buf,sizeof (tx_buf), 0);

}

/*
  * @brief  Tx_protocol 接收程序
  * @param 
  * @note   FE FE cmd length data crch crcl BB  
  * @Date:  2017.7.10
  * @updatge:2017.7.10
  * @author:zhao
  * @return:void
*/
void Rx_protocol(void)
{
	uint16_t crc16 = 0;

	uint8_t rx_buf[8];
	uint8_t rx_cnt=0;
	
	rx_cnt = SI4332B_read_msg((uint8_t *)&rx_buf, 8, 0);
	
	if ((rx_buf[0] == 0xfe)&&(rx_buf[1] == 0xfe))
	{
     // DBG_B_INFO("SI4332: Receive Success");
	    crc16 = Crc16Calc(rx_buf, 3);

      if ((rx_buf[8 - 1] == 0xbb)&&(rx_buf[8 - 2] == (uint8_t) crc16)&&(rx_buf[8 - 3] == (uint8_t) (crc16 >> 8)))
	    {
	     
		  switch(rx_buf[2])
		  {
		  	 case 0x00:number_get=rx_buf[4];break;
			  default:break;
		  	}
		  	DBG_B_INFO("SI4332: Receive Success  number[%d]",number_get);
	 
      }
	
	}
	
	  memset(rx_buf,0,sizeof(rx_buf));

}



/*
  * @brief  si4432通信程序
  * @param 
  * @note   
  * @Date:  2017.7.10
  * @updatge:2017.7.10
  * @author:zhao
  * @return:void
*/
void protocol_task(void)
{
  DBG_B_INFO("protocol_task");
	Tx_protocol();
	delay_ms(500);
	Rx_protocol();
	
}


/*
  * @brief  si4432通信演示程序
  * @param 
  * @note   
  * @Date:  2017.7.10
  * @updatge:2017.7.10
  * @author:zhao
  * @return:void
*/
void si4432_demo(void)
{
	uint8_t i;
	int16_t si_rx_len=0;

	for(i=0; i<PAYLOAD_SIZE; i++)
	{
  	  payload[i] = 0xAA;
		
	}

//	DBG_B_INFO("-------------- SI4432A --------------");
//	DBG_B_INFO("SI4332: Send Data");
//	dump(payload,8);
	SI4332A_send_msg((const uint8_t *)payload, PAYLOAD_SIZE, 0);
	delay_ms(100);


//	DBG_B_INFO("-------------- SI4432B clean up --------------");
//	memset(payload_rx, 0, PAYLOAD_SIZE);
//	dump(payload_rx,8);
//	DBG_B_INFO("-------------- SI4432B --------------");
	//Rx_protocol();
	si_rx_len = SI4332B_read_msg((uint8_t *)&payload_rx, PAYLOAD_SIZE, 0);

	if(si_rx_len > 0)
	{
		if(si4432_rx_handle(payload_rx, PAYLOAD_SIZE))
		{
			DBG_B_INFO("SI4332: Receive Success");
		}
	}
	else if(si_rx_len <= 0)
	{
	  	DBG_B_INFO("SI4332: Receive Error ");
	}

}




