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
*������void ClrAtRxBuf(void)
*���ܣ����AT���ڽ��ջ�����ؼĴ���
*���룺��
*�������
*����˵������
*******************************************************************/
void ClrAtRxBuf(void)
{
	memset(AT_RX_BUF, 0x00, MAX_AT_RX_LEN);//��ջ���
	AT_RX_COUNT = 0;
	F_AT_RX_FINISH = 0;
}
/*******************************************************************
*������void SendAtCmd(uint8_t *string)
*���ܣ�����ATָ���ַ���
*���룺
		uint8_t *string �����͵�ATָ��
		uint8_t len ����
*�������
*����˵������
*******************************************************************/
void SendAtCmd(uint8_t *string, uint8_t len)
{
	UART4_SendStrlen(string, len);
	UART4_SendStrlen((uint8_t *)"\r\n", 2);
}

/*******************************************************************
*������void SendStrLen(uint8_t *string, uint8_t len)
*���ܣ������ַ�����ATָ���
*���룺
		uint8_t *string �����͵��ַ���
		uint8_t len ����

*�������
*����˵������
*******************************************************************/
void SendStrLen(uint8_t *string, uint8_t len)
{
	UART4_SendStrlen(string, len);
}



void ESP8266_init(void)
{
	uint8_t TryCount=0;
	int8_t temp=0;
	//���ӷ�����
	for(TryCount=0; TryCount<4; TryCount++)
	{
		temp=ConnectToServer((char *)MY_DEVICE_ID, (char *)MY_SECRET_KEY);
		if(temp != 0)
		{
			DBG_R_E("Connect To Server ERROR %d ,  ",temp);
			
		}
		else
		{
		 
		  xfs_voice_start((unsigned char *)"������ƽ̨�ɹ�", XFS_FORMAT_GBK, 1);
		  DBG_R_E("Connect To Server OK");
		  break;
		}
	}
	
	
}

