/******************************************************************************
*
*
*
*
*
*
*
*
*
*
******************************************************************************/
/**********************************************************
	Modify History: qiuzhb make, 2016-8-21
**********************************************************/
#include <string.h>
#include <stdint.h>
#include "debug_printf.h"
#include "delay.h"
#include "bsp_uart4.h"
#include "WiFiToCloud.h"
#include "HAL_WiFiToCloud.h"
#include "xfs5152ce.h"


/*******************************************************************
*函数：void ClrAtRxBuf(void)
*功能：清空AT串口接收缓存相关寄存器
*输入：无
*输出：无
*特殊说明：无
*******************************************************************/
void ClrAtRxBuf(void)
{
	memset(AT_RX_BUF, 0x00, MAX_AT_RX_LEN);//清空缓存
	AT_RX_COUNT = 0;
	F_AT_RX_FINISH = 0;
}
/*******************************************************************
*函数：void SendAtCmd(uint8_t *string)
*功能：发送AT指令字符串
*输入：
		uint8_t *string 待发送的AT指令
		uint8_t len 长度
*输出：无
*特殊说明：无
*******************************************************************/
void SendAtCmd(uint8_t *string, uint8_t len)
{
	UART4_SendStrlen(string, len);
	UART4_SendStrlen((uint8_t *)"\r\n", 2);
}

/*******************************************************************
*函数：void SendStrLen(uint8_t *string, uint8_t len)
*功能：发送字符串到AT指令串口
*输入：
		uint8_t *string 待发送的字符串
		uint8_t len 长度

*输出：无
*特殊说明：无
*******************************************************************/
void SendStrLen(uint8_t *string, uint8_t len)
{
	UART4_SendStrlen(string, len);
}



void ESP8266_init(void)
{
	uint8_t TryCount=0;
	int8_t temp=0;
	//连接服务器
	for(TryCount=0; TryCount<4; TryCount++)
	{
		temp=ConnectToServer((char *)MY_DEVICE_ID, (char *)MY_SECRET_KEY);
		if(temp != 0)
		{
			DBG_R_E("Connect To Server ERROR %d ,  ",temp);
			
		}
		else
		{
		 
		  xfs_voice_start((unsigned char *)"连接云平台成功", XFS_FORMAT_GBK, 1);
		  DBG_R_E("Connect To Server OK");
		  break;
		}
	}
	
	
}

