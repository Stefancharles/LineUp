/******************************************************************************
******************************************************************************
* @file    app/app_cloud.c 
* @author  zhao
* @version V1.0.0
* @date    2018.07.27
* @update  2018.07.27 
* @brief   云服务代码
******************************************************************************
******************************************************************************/

#include "WiFiToCloud.h"
#include "debug_printf.h"
#include "app_cloud.h"

uint8_t IpData[128];
/*******************************************************************
*函数：int8_t ESP8266_SendSensor(char *ApiTag, 
									uint32_t sensor, char *TimeStr)
*功能：发送传感数据到服务器
*输入：
	char *ApiTag0 实体对象标签名称
	uint32_t sensor0, 传感数值
	char *ApiTag1 实体对象标签名称
	uint32_t sensor1, 传感数值
	char *ApiTag2 实体对象标签名称
	uint32_t sensor2, 传感数值
	char *TimeStr，采集数据时的时间，须是yyyy-mm-dd hh:mm:ss格式
*输出：
		return = 0 ,sucess
		return < 0 ,error
*特殊说明：
*******************************************************************/
int8_t SE_SendSensor(char *ApiTag0, uint32_t Value0, char *TimeStr)
{
	uint8_t TryGo = 0;
	int8_t error = 0;
	uint8_t TrySend = 0;
	uint8_t TxetBuf[MAX_AT_TX_LEN];
	memset(TxetBuf,0x00,MAX_AT_TX_LEN);//清空缓存
	sprintf((char *)TxetBuf,"{\"t\":3,\"datatype\":2,\"datas\":{\"%s\":{\"%s\":%d}},\"msgid\":001}",
													ApiTag0,TimeStr,Value0);
	se_send_sensor:		
	//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
	if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
	{//发送失败
		error=-1;
	
	}
	else
	{//发送成功
		for(TryGo = 0; TryGo<50; TryGo++)//最多等待时间50*10=500ms
		{
			if(strstr((const char *)AT_RX_BUF, (const char *)"\"status\":0") == NULL)//检查响应状态是否为"上报成功"
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
//	    DBG_B_INFO("重发传感器数据 %d ",TrySend+1);
	 	TrySend++;
		goto se_send_sensor;
	}
	return error;
}

/*******************************************************************
*函数：char *USER_GetJsonValue(char *cJson, char *Tag)
*功能：json为字符串序列，将json格式中的目标对象Tag对应的值字符串提取出来
*输入：
		char *cJson json字符串
		char *Tag 要操作的对象标签
*输出：返回数值的字符串形式的启始地址
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
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
		//printf("空字符！\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<10; i++, target++)//数值超过10个位为非法，由于2^32=4294967296
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
	//printf("数值=%s\r\n",temp);
	return (char *)temp;
}

/*******************************************************************
*函数：char *USER_GetSteeringEngineJsonValue(char *cJson, char *Tag)
*功能：json为字符串序列，将json格式中的目标对象Tag对应的值字符串转换为数值
*输入：
		char *cJson json字符串
		char *Tag 要操作的对象标签
*输出：返回数值
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
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
		printf("空字符！\r\n");
		return NULL;
	}
	i=strlen((const char *)temp);
	target=target+i;
	memset(temp, 0x00, 128);
	for(i=0; i<10; i++, target++)//数值超过10个位为非法，由于2^32=4294967296
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
// printf("数值str=%s\r\n",temp);
//	printf("数值str=%s\r\n",target);
// len=strlen((const char *)target);
//	for(i=0; i<len;i++)
//    {
//	 printf("数值str[%d]=0x%x  \r\n",i,*(target+i));
//	}
	len=strlen((const char *)temp);
	switch(len)
	{
		case(1):value=temp[0]-'0';break;
		case(2):value=temp[1]-'0'+(temp[0]-'0')*10;break;
		case(3):value=temp[2]-'0'+(temp[0]-'0')*100+(temp[1]-'0')*10;break;
		default:break;
	}
	//printf("数值value=%d\r\n",value);
	return value;
}


/*******************************************************************
*函数：void USER_DataAnalysisProcess(char *RxBuf)
*功能：解析服务器数据
*输入：char *RxBuf 服务器下发数据
*输出：无
*特殊说明：用户可以在此基础上改造和扩展该函数，这里只是个简单的DEMO
*******************************************************************/
void USER_DataAnalysisProcess(char *RxBuf)
{
	char *cmdid = NULL;
	uint8_t TxetBuf[128];
	uint8_t number_down=0;
	if(strstr((const char *)RxBuf, (const char *)PING_REQ) != NULL)//心跳请求？
	{
		if(ESP8266_IpSend((char *)PING_RSP, strlen((const char *)PING_RSP)) < 0)//响应心跳
		{//发送失败
			DBG_B_INFO("发送心跳包失败！\r\n");
		}
		else
		{
			DBG_B_INFO("心跳包！\r\n");
		}
	}
	else if(strstr((const char *)RxBuf, (const char *)"\"t\":5") != NULL)//命令请求？
	{
		if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"bool_work\"") != NULL)//控制取号
		{
			memset(TxetBuf,0x00,128);//清空缓存
			if((strstr((const char *)RxBuf, (const char *)"\"data\":1") != NULL))//开灯
			{
				DBG_B_INFO("开始取号");
			    PWD_RUN();
			
				cmdid = USER_GetJsonValue((char *)RxBuf, (char *)"cmdid");
				sprintf((char *)TxetBuf,"{\"t\":6,\"cmdid\":%s,\"status\":0,\"data\":1}",cmdid);
				//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
				if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
				{//发送失败
					DBG_B_INFO("发送响应失败！");
				}
			}
			else if((strstr((const char *)RxBuf, (const char *)"\"data\":0") != NULL))//关灯
			{
				DBG_B_INFO("暂停取号");
				PWD_IDLE();
			
				cmdid = USER_GetJsonValue((char *)RxBuf, (char *)"cmdid");
				sprintf((char *)TxetBuf,"{\"t\":6,\"cmdid\":%s,\"status\":0,\"data\":0}",cmdid);
				//printf("%s\r\n",TxetBuf);////////////////////////////////////////////////////////////
				if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
				{//发送失败
					DBG_B_INFO("发送响应失败！");
				}
			}
		}
		else if(strstr((const char *)RxBuf, (const char *)"\"apitag\":\"number_down\"") != NULL)//叫号按钮
		{
			memset(TxetBuf,0x00,128);//清空缓存
			number_down=USER_GetSteeringEngineJsonValue((char *)RxBuf, (char *)"data");
			
		    cmdid = USER_GetJsonValue((char *)RxBuf, (char *)"cmdid");
			sprintf((char *)TxetBuf,"{\"t\":6,\"cmdid\":%s,\"status\":0,\"data\":1}",cmdid);
			xfs_delete_number();
			if(ESP8266_IpSend((char *)TxetBuf, strlen((char *)TxetBuf)) < 0)
			{//发送失败
				DBG_B_INFO("发送响应失败！");
			}
			
			DBG_B_INFO("设置减员人数：%d！",number_down);
		}


	}
}


/*******************************************************************
*函数：void USER_Datatoplayer(char *RxBuf)
*功能：解析服务器数据 判断是否为语音播报执行器
*输入：char *RxBuf 服务器下发数据
*输出：无
*特殊说明：考生在该代码上补充。需获取需要语音播报额数据
           例如：char *RxBuf= {"apitag":"string_play","cmdid":73166,"data":"你好","t":5}       p_play="你好"      播报"你好"。
                 char *RxBuf= {"apitag":"string_play","cmdid":73166,"data":"谢谢","t":5}    p_play="谢谢"      播报"谢谢"。

*******************************************************************/
void USER_Datatoplayer(char *RxBuf)
{
    char *p_play=NULL;
	/*以下部分由选手补充*/
	//空白部分起始========>>
	
	
	
	
	
		
	//空白部分结束========<<


	
	if(p_play!=NULL)
	{
	 printf("%s\r\n",p_play);
	 xfs_voice_start((unsigned char *)p_play, XFS_FORMAT_GBK, 1);
	}
 ;
}



/*
  * @brief  cloud_task 云服务进程
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
   if(F_AT_RX_FINISH)//串口4接收到数据
	 {	  // 接收到数据包
		 ESP8266_GetIpData((uint8_t *)AT_RX_BUF, (char *)IpData);
		 USER_DataAnalysisProcess((char *)IpData);
		 USER_Datatoplayer((char*)IpData);
		 memset(IpData, 0x00, 128);
		 ClrAtRxBuf();
	 }
	 if((uint32_t)(SYSTICK_get_time()-lastTime>=3000))//3S发送一次数据,备注：该时间不准确，大约3s
	 {
	    lastTime=SYSTICK_get_time();

	  	RTC_Get();				 //更新时间   
	  	sprintf(str,"%d-%d-%d %d:%d:%d",calendar.w_year,calendar.w_month,calendar.w_date,calendar.hour,calendar.min,calendar.sec);//输出闹铃时间  
	   	error=SE_SendSensor((char *)"number_up", number_show, (char *)str);
	 	  ClrAtRxBuf();
	   // DBG_B_INFO("SE_SendSensor %d",error);
		 //判断是否和服务器失去连接
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
		if(erroCount > 3)//通讯失败次数超过6次 视为设备离线
		{
			erroCount=0;
			DBG_B_INFO("断线重连");
			ESP8266_init();
		}
	}
	
	

}



