/******************************************************************************
******************************************************************************
* @file    app/app_cloud.c 
* @author  zhao
* @version V1.0.0
* @date    2018.07.27
* @update  2018.07.27 
* @brief   �Ʒ������
******************************************************************************
******************************************************************************/

#include "WiFiToCloud.h"
#include "debug_printf.h"
#include "app_cloud.h"

uint8_t IpData[128];
/*******************************************************************
*������int8_t ESP8266_SendSensor(char *ApiTag, 
									uint32_t sensor, char *TimeStr)
*���ܣ����ʹ������ݵ�������
*���룺
	char *ApiTag0 ʵ������ǩ����
	uint32_t sensor0, ������ֵ
	char *ApiTag1 ʵ������ǩ����
	uint32_t sensor1, ������ֵ
	char *ApiTag2 ʵ������ǩ����
	uint32_t sensor2, ������ֵ
	char *TimeStr���ɼ�����ʱ��ʱ�䣬����yyyy-mm-dd hh:mm:ss��ʽ
*�����
		return = 0 ,sucess
		return < 0 ,error
*����˵����
*******************************************************************/
int8_t SE_SendSensor(char *ApiTag0, uint32_t Value0, char *TimeStr)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TrySend = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//��ջ���
	sprintf((char *)TxetBuf,"{\"t\":3,\"datatype\":2,\"datas\":{\"%s\":{\"%s\":%d}},\"msgid\":001}",
													ApiTag0,TimeStr,Value0);
	se_send_sensor:		
	//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
	{//����ʧ��
		error=-1;
	
	}
	else
	{//���ͳɹ�
		for(TryGo = 0; TryGo<50; TryGo++)//���ȴ�ʱ��50*10=500ms
		{
			if(strstr((const char *)AT_RX_BUF, (const char *)"\"status\":0") == NULL)//�����Ӧ״̬�Ƿ�Ϊ"�ϱ��ɹ�"
			{
				error = -1;
			
			}
			else
			{
				error = 0;
				break;
			}
			delay_ms(1000);
		}
	}
	if((error!=0)&&(TrySend<2))
	{
//	    DBG_B_INFO("�ط����������� %d ",TrySend+1);
	 	TrySend++;
		goto se_send_sensor;
	}
	return error;
}

/*******************************************************************
*������char *USER_GetJsonValue(char *cJson, char *Tag)
*���ܣ�jsonΪ�ַ������У���json��ʽ�е�Ŀ�����Tag��Ӧ��ֵ�ַ�����ȡ����
*���룺
		char *cJson json�ַ���
		char *Tag Ҫ�����Ķ����ǩ
*�����������ֵ���ַ�����ʽ����ʼ��ַ
*����˵�����û������ڴ˻����ϸ������չ�ú���������ֻ�Ǹ��򵥵�DEMO
*******************************************************************/
char *USER_GetJsonValue(char *cJson, char *Tag)
{
	char *target = NULL;
	static char temp[10];
	int8_t i=0;
	
	memset(temp, 0x00, 128);
	sprintf(temp,"\"%s\":",Tag);
	target=strstr((const char *)cJson, (const char *)temp);
	if(target == NULL)
	{
		//printf("���ַ���\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<10; i++, target++)//��ֵ����10��λΪ�Ƿ�������2^32=4294967296
	{
		if((*target<='9')&&(*target>='0'))
		{
			temp[i]=*target;
		}
		else
		{
			break;
		}
	}
	temp[i+1] = '\0';
	//printf("��ֵ=%s\r\n",temp);
	return (char *)temp;
}

/*******************************************************************
*������char *USER_GetSteeringEngineJsonValue(char *cJson, char *Tag)
*���ܣ�jsonΪ�ַ������У���json��ʽ�е�Ŀ�����Tag��Ӧ��ֵ�ַ���ת��Ϊ��ֵ
*���룺
		char *cJson json�ַ���
		char *Tag Ҫ�����Ķ����ǩ
*�����������ֵ
*����˵�����û������ڴ˻����ϸ������չ�ú���������ֻ�Ǹ��򵥵�DEMO
*******************************************************************/
uint8_t USER_GetSteeringEngineJsonValue(char *cJson, char *Tag)
{
	char *target = NULL;
	//char *str = NULL;
	static char temp[10];
	uint8_t len=0;
	uint8_t value=0;
	int8_t i=0;
	
	memset(temp, 0x00, 128);
	sprintf(temp,"\"%s\":\"",Tag);
	target=strstr((const char *)cJson, (const char *)temp);
	if(target == NULL)
	{
		printf("���ַ���\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<10; i++, target++)//��ֵ����10��λΪ�Ƿ�������2^32=4294967296
	{
		if((*target<='9')&&(*target>='0'))
		{
			temp[i]=*target;
		}
		else
		{
			break;
		}
	}
	//str=strstr((const char *)target, (const char *)"\",");
	
	temp[i+1] = '\0';
// printf("��ֵstr=%s\r\n",temp);
//	printf("��ֵstr=%s\r\n",target);
// len=strlen((const char *)target);
//	for(i=0; i<len;i++)
//    {
//	 printf("��ֵstr[%d]=0x%x  \r\n",i,*(target+i));
//	}
	len=strlen((const char *)temp);
	switch(len)
	{
		case(1):value=temp[0]-'0';break;
		case(2):value=temp[1]-'0'+(temp[0]-'0')*10;break;
		case(3):value=temp[2]-'0'+(temp[0]-'0')*100+(temp[1]-'0')*10;break;
		default:break;
	}
	//printf("��ֵvalue=%d\r\n",value);
	return value;
}


/*******************************************************************
*������void USER_DataAnalysisProcess(char *RxBuf)
*���ܣ���������������
*���룺char *RxBuf �������·�����
*�������
*����˵�����û������ڴ˻����ϸ������չ�ú���������ֻ�Ǹ��򵥵�DEMO
*******************************************************************/
void USER_DataAnalysisProcess(char *RxBuf)
{
	char *cmdid = NULL;
	uint8_t TxetBuf[128];
	uint8_t number_down=0;
	if(strstr((const char *)RxBuf, (const char *)PING_REQ) != NULL)//��������
	{
		if(ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP)) < 0)//��Ӧ����
		{//����ʧ��
			DBG_B_INFO("����������ʧ�ܣ�\r\n");
		}
		else
		{
			DBG_B_INFO("��������\r\n");
		}
	}
	else if(strstr((const char *)RxBuf, (const char *)"\"t\":5") != NULL)//��������
	{
		if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"bool_work\"") != NULL)//����ȡ��
		{
			memset(TxetBuf,0x00,128);//��ջ���
			if((strstr((const char *)RxBuf, (const char *)"\"data\":1") != NULL))//����
			{
				DBG_B_INFO("��ʼȡ��");
			    PWD_RUN();
			
				cmdid = USER_GetJsonValue((char *)RxBuf, (char *)"cmdid");
				sprintf((char *)TxetBuf,"{\"t\":6,\"cmdid\":%s,\"status\":0,\"data\":1}",cmdid);
				//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
				if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
				{//����ʧ��
					DBG_B_INFO("������Ӧʧ�ܣ�");
				}
			}
			else if((strstr((const char *)RxBuf, (const char *)"\"data\":0") != NULL))//�ص�
			{
				DBG_B_INFO("��ͣȡ��");
				PWD_IDLE();
			
				cmdid = USER_GetJsonValue((char *)RxBuf, (char *)"cmdid");
				sprintf((char *)TxetBuf,"{\"t\":6,\"cmdid\":%s,\"status\":0,\"data\":0}",cmdid);
				//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
				if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
				{//����ʧ��
					DBG_B_INFO("������Ӧʧ�ܣ�");
				}
			}
		}
		else if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"number_down\"") != NULL)//�кŰ�ť
		{
			memset(TxetBuf,0x00,128);//��ջ���
			number_down=USER_GetSteeringEngineJsonValue((char *)RxBuf, (char *)"data");
			
		    cmdid = USER_GetJsonValue((char *)RxBuf, (char *)"cmdid");
			sprintf((char *)TxetBuf,"{\"t\":6,\"cmdid\":%s,\"status\":0,\"data\":1}",cmdid);
			xfs_delete_number();
			if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
			{//����ʧ��
				DBG_B_INFO("������Ӧʧ�ܣ�");
			}
			
			DBG_B_INFO("���ü�Ա������%d��",number_down);
		}


	}
}


/*******************************************************************
*������void USER_Datatoplayer(char *RxBuf)
*���ܣ��������������� �ж��Ƿ�Ϊ��������ִ����
*���룺char *RxBuf �������·�����
*�������
*����˵���������ڸô����ϲ��䡣���ȡ��Ҫ��������������
           ���磺char *RxBuf= {"apitag":"string_play","cmdid":73166,"data":"���","t":5}       p_play="���"      ����"���"��
                 char *RxBuf= {"apitag":"string_play","cmdid":73166,"data":"лл","t":5}    p_play="лл"      ����"лл"��

*******************************************************************/
void USER_Datatoplayer(char *RxBuf)
{
    char *p_play=NULL;
	/*���²�����ѡ�ֲ���*/
	//�հײ�����ʼ========>>
	
	
	
	
	
		
	//�հײ��ֽ���========<<


	
	if(p_play!=NULL)
	{
	 printf("%s\r\n",p_play);
	 xfs_voice_start((unsigned char *)p_play, XFS_FORMAT_GBK, 1);
	}
 ;
}



/*
  * @brief  cloud_task �Ʒ������
  * @param  none
  * @note           
  * @Date:2018.7.27
  * @author:zhao
  * @return:none
*/
extern volatile char number_write;
extern volatile char number_show;
void cloud_task(void)
{
   static uint32_t lastTime;
   static int8_t erroCount=0;
   int8_t error=0;
   char str[50];
   if(F_AT_RX_FINISH)//����4���յ�����
	 {	  // ���յ����ݰ�
		 ESP8266_GetIpData((uint8_t *)AT_RX_BUF, (char *)IpData);
		 USER_DataAnalysisProcess((char *)IpData);
		 USER_Datatoplayer((char*)IpData);
		 memset(IpData, 0x00, 128);
		 ClrAtRxBuf();
	 }
	 if((uint32_t)(SYSTICK_get_time()-lastTime>=3000))//3S����һ������,��ע����ʱ�䲻׼ȷ����Լ3s
	 {
	    lastTime=SYSTICK_get_time();

	  	RTC_Get();				 //����ʱ��   
	  	sprintf(str,"%d-%d-%d %d:%d:%d",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//�������ʱ��  
	   	error=SE_SendSensor((char *)"number_up", number_show, (char *)str);
	 	  ClrAtRxBuf();
	   // DBG_B_INFO("SE_SendSensor %d",error);
		 //�ж��Ƿ�ͷ�����ʧȥ����
	  if(error != 0)
	   {
	   erroCount++;
	//	 DBG_B_INFO("erroCount++  %d",erroCount);
	   }
		else
		{
			erroCount=0;
	//		DBG_B_INFO("erroCount=0 %d",erroCount);
		}
		if(erroCount > 3)//ͨѶʧ�ܴ�������6�� ��Ϊ�豸����
		{
			erroCount=0;
			DBG_B_INFO("��������");
			ESP8266_init();
		}
	}
	
	

}



