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

/* Includes ------------------------------------------------------------------*/
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "stm32f10x.h"
#include "xfs5152ce.h"
#include "delay.h"
#include "debug_printf.h"
#include "usart3.h"
#include "app_pwd.h"
/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
// 命令字 
typedef enum {
	XFS_CMD_STATUS = 0x21,//查询状态
	XFS_CMD_SLEEP = 0x88,//进入低功耗模式
	XFS_CMD_WAKE_UP = 0xFF,//从低功耗返回正常模式
	XFS_CMD_VOICE_START = 0x01,//开始语音合成
	XFS_CMD_VOICE_STOP = 0x02,//停止语音合成
	XFS_CMD_VOICE_PAUSE = 0x03,//暂停语音合成
	XFS_CMD_VOICE_RESTORE = 0x04,//恢复语音合成
	XFS_CMD_SEND_TO_CACHE = 0x31,//发送文本到缓存区
	XFS_CMD_PLAY_FROM_CACHE = 0x32,//从缓存区播放文本
	XFS_CMD_RECOGNIZE_START = 0x10,
	XFS_CMD_RECOGNIZE_STOP = 0x1F
}xfsCommandEnum;


#define XFS_RESPONSE_TIMEOUT 300


// 语音识别命令词 
const char *cmd_word_list[] = {
	"我在吃饭",  //0
	"我在修车",  //1
	"我在加油",  //2
	"正在休息",//3
	"同意",//4
	"不同意",//5
	"我去",//6
	"现在几点",//7
	"今天几号",//8
	"读信息",//9
	"开始读",//10
	"这是哪儿",//11
	"打开广播",//12
	"关掉广播",//13
	"打开音乐",//14
	"关掉音乐",//15
	"再听一次",//16
	"再读一遍",//17
	"大声点",//18
	"小声点",//19
	"读短信",//20
	"读预警信息",//21
	"明天天气怎样",//22
	"紧急预警信息",//23
	"开始",//24
	"停止",//25
	"暂停",//26
	"继续读",//27
	"确定开始",//28
	"取消"//29
};

/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/

/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/



/**
  * @brief  
  * @param  None
  * @retval None
  */
int xfs_init(void)
{
  usart3_init(115200);
  return 0;
}




/**
  * @brief  发送命令
	* @param  cmd: 命令
  * @retval 1：成功
  * @retval 0：失败
  */
static int xfs_send_cmd(uint8_t cmd)
{
	unsigned  char frame_info[8]; //定义的文本长度
	uint16_t frame_len = 0;
	 
/*****************帧固定配置信息**************************************/           
	frame_info[frame_len++] = 0xFD ; 			//构造帧头FD
	frame_info[frame_len++] = 0x00 ; 			//构造数据区长度的高字节
	frame_info[frame_len++] = 0x01; 		//构造数据区长度的低字节
	frame_info[frame_len++] = cmd ; 			//构造命令字 
 
/*******************发送帧信息***************************************/

	uart3_send_buff(frame_info, frame_len);
  
	return wait_response(XFS_ACK_CMD_RIGHT, XFS_RESPONSE_TIMEOUT);
}


/**
  * @brief  获得状态
  * @param  
  * @retval 状态码
  */
int xfs_get_status(void)
{
	unsigned char rec_buf[8];
	unsigned  char frame_info[8]; //定义的文本长度
	uint16_t frame_len = 0;
	int timeout = XFS_RESPONSE_TIMEOUT;
	
	memset(rec_buf, 0, sizeof(rec_buf));
/*****************帧固定配置信息**************************************/           
	frame_info[frame_len++] = 0xFD ; 			//构造帧头FD
	frame_info[frame_len++] = 0x00 ; 			//构造数据区长度的高字节
	frame_info[frame_len++] = 0x01; 		//构造数据区长度的低字节
	frame_info[frame_len++] = XFS_CMD_STATUS ; 			//构造命令字 
 
/*******************发送帧信息***************************************/
	uart3_send_buff(frame_info, frame_len);

/*******************等待接收***************************************/

	while (--timeout)
	{
		delay_ms(1);

     if(USART_GetFlagStatus(USART3,USART_FLAG_RXNE)!=RESET)//确认下是不是串口1接收中断
     {
	     rec_buf[0] =USART_ReceiveData(USART3);	   
	      return  rec_buf[0];     
     }
	}
	return -1;

	
}
/**
  * @brief  进入睡眠模式
  * @param  
  * @retval 1：成功
  * @retval 0：失败
  */
int xfs_sleep(void)
{
	return xfs_send_cmd(XFS_CMD_SLEEP);
}
/**
  * @brief  唤醒
  * @param  
  * @retval 1：成功
  * @retval 0：失败
  */
int xfs_wake_up(void)
{
	return xfs_send_cmd(XFS_CMD_WAKE_UP);
}

/**
  * @brief  开始语音播报
  * @param  data：语音文本
  * @param  format:文本编码格式 0-GB2312,1-GBK,2-BIG,3-UNICODE
  * @param  wait_finish：是否等待芯片空闲
  * @retval 1：成功
  * @retval 0：失败
  */
int xfs_voice_start(unsigned char *data, unsigned char format, int wait_finish)
{
	unsigned  char *frame_info;
	unsigned  int  data_length, frame_offset=0;  
	int ack = 0;
	
	if(data == NULL)
		return -1;
	if(format > XFS_FORMAT_UNICODE)
		return -1;

	//if(wait_finish)
	//	while(xfs_get_status() != XFS_ACK_IDEL)
	//		delay_ms(10);
		
	data_length =strlen((char*)data); 			//需要发送文本的长度
	frame_info = (unsigned char *)calloc((unsigned int)data_length, sizeof(unsigned char));
	if(frame_info == NULL)
		return -1;

/*****************帧固定配置信息**************************************/           
	frame_info[frame_offset++] = 0xFD ; 			//构造帧头FD
	frame_info[frame_offset++] = (data_length>>8) & 0xFF; 			//构造数据区长度的高字节
	frame_info[frame_offset++] = (data_length & 0xFF) + 2; 	  	//构造数据区长度的低字节
	frame_info[frame_offset++] = XFS_CMD_VOICE_START ; 		    	//构造命令字：合成播放命令		 		 
	frame_info[frame_offset++] = format;                        //文本编码格式：GBK 
 
/*******************发送帧信息***************************************/		  
	memcpy(&frame_info[5], data, data_length);
	frame_offset += data_length;
//	ptrUSART->Send(frame_info, frame_offset);//发送帧配置
	uart3_send_buff(frame_info, frame_offset);
//	while (ptrUSART->GetTxCount() < frame_offset);
	ack = wait_response(XFS_ACK_CMD_RIGHT, XFS_RESPONSE_TIMEOUT);
	
	if(frame_info)
		free(frame_info);
	
	return ack;
}
/**
  * @brief  停止播报
  * @param  
  * @retval 1：成功
  * @retval 0：失败
  */
int xfs_voice_stop(void)
{
	return xfs_send_cmd(XFS_CMD_VOICE_STOP);
}
/**
  * @brief  暂停播报
  * @param  
  * @retval 1：成功
  * @retval 0：失败
  */
int xfs_voice_pause(void)
{
	return xfs_send_cmd(XFS_CMD_VOICE_PAUSE);
}
/**
  * @brief  继续播报
  * @param  
  * @retval 1：成功
  * @retval 0：失败
  */
int xfs_voice_restore(void)
{
	return xfs_send_cmd(XFS_CMD_VOICE_RESTORE);
}



/**
  * @brief  发送文本到缓存区
  * @param  data：文本数据
  * @param  cache_num (0~15)
  * @retval 1：成功
  * @retval 0：失败
  * @note  缓存空间总共4K,共分16区,每个区256字节
					设本次起始缓存区段=X(0<=X<=15),则本次发送的文本长度不能大于(16-X)*256字节，多余的文本将丢弃
					用户在播放缓存文本前，可多次发送缓存命令，但切记后来发送的文本不能部分覆盖或全部覆盖之前的文本，
					否则不保证播放的正确性
  */
int xfs_send_to_cache(unsigned char *data, unsigned char cache_num)
{
	unsigned char *frame_info = NULL; //定义的文本长度
	unsigned short data_length, frame_offset = 0;  
	unsigned char ack = 0;
	
	if(data == NULL)
		return -1;
	if(cache_num >= 16)
		return -1;
	data_length = strlen((char*)data); 			//需要发送文本的长度
	if(data_length > (16-cache_num)*256)
		return -1;
	frame_info = (unsigned char *)calloc((unsigned int)data_length, sizeof(unsigned char));
	if(frame_info == NULL)
		return -1;
/*****************帧固定配置信息**************************************/           
	frame_info[frame_offset++] = 0xFD ; 			//构造帧头FD
	frame_info[frame_offset++] = (data_length>>8) & 0xFF; 			//构造数据区长度的高字节
	frame_info[frame_offset++] = (data_length & 0xFF) + 2; 		//构造数据区长度的低字节
	frame_info[frame_offset++] = XFS_CMD_SEND_TO_CACHE ; 			//构造命令字	 
	frame_info[frame_offset++] = cache_num;       //文本编码格式：GBK 
 
/*******************发送帧信息***************************************/		  
	memcpy(&frame_info[frame_offset], data, data_length);
	frame_offset += data_length;
//	ptrUSART->Send(frame_info,frame_offset);//发送帧配置
//	while (ptrUSART->GetTxCount() < frame_offset);
	uart3_send_buff(frame_info, frame_offset);
	ack = wait_response(XFS_ACK_CMD_RIGHT, XFS_RESPONSE_TIMEOUT);
	
	if(frame_info)
		free(frame_info);
	return ack;

}

/**
  * @brief  播放缓存内容
  * @param  num(1~15)
  * @param  format:0-GB2312,1-GBK,2-BIG,3-UNICODE
  * @retval 1：成功
  * @retval 0：失败
  */
int xfs_paly_from_cache(unsigned char num, unsigned char format)
{
	unsigned  char frame_info[8];
	unsigned short frame_len = 0;
	
	if(format > XFS_FORMAT_UNICODE || num > 15)
	return -1;

/*****************帧固定配置信息**************************************/           
	frame_info[frame_len++] = 0xFD ; 			//构造帧头FD
	frame_info[frame_len++] = 0x00 ; 			//构造数据区长度的高字节
	frame_info[frame_len++] = 0x02; 		//构造数据区长度的低字节
	frame_info[frame_len++] = XFS_CMD_PLAY_FROM_CACHE ; 			//构造命令字 
 	frame_info[frame_len++] = (num << 4 & 0xF0) | (format & 0x0F); 			//参数：高四位-重复播放次数，低四位-文本编码格式

/*******************发送帧信息***************************************/
	uart3_send_buff(frame_info, frame_len);
	return wait_response(XFS_ACK_CMD_RIGHT, XFS_RESPONSE_TIMEOUT);

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


/**
  * @brief  语音识别
  * @param  result[out]: 识别结果
  * @param  waittime[in]: 超时时间
  * @retval >0：   成功
  * @retval 0 ：  失败
  * @note ： 考生在此函数实现语音识别功能  指针*result返回 识别结果的关键代码     
                                           waittime    传入等待时间  ，比如0.5秒内识别语音   waittime=500
                                
             xfs5152ce模块收发使用的串口为  USART3

             语音识别过程如下:
                       MCU串口发送命令       FD 00 01 10  给xfs5152
                             |
                             |
                             |
                       对着麦克风说话
						     |
							 |
							 |                            
                          等待接收      FC 00 2A cmd    
                             |
                             |
                             |
                           关键码      与cmd_word_list[]表一一对应
                         
  **/
int xfs_recognize(unsigned char *result, unsigned int waittime)
{
	unsigned  char rx_frame_info[8];
	int err = 1;
	
	if(result == NULL){
		err = 0;
		return err;
	}
	DBG_B_INFO("recognize start...");
	if(xfs_send_cmd(XFS_CMD_RECOGNIZE_START) != 1)
	{
			err = 0;
			DBG_R_E("Error: XFS_CMD_RECOGNIZE_START");
			goto ERROR_OUT;
   }
	DBG_R_E("Succeed: XFS_CMD_RECOGNIZE_START");

	/*以下部分由选手补充*/
	//空白部分起始========>>

	
	

	while (--waittime){
		 SYSTICK_Delay10ms(1);
		 kfifo_pull_out(&usart3_fifo,rx_frame_info,1);	
	
			if(rx_frame_info[0]==0xFC)
			break;
	}

	DBG_R_E("Succeed: kfifo_pull_out");
	
    kfifo_pull_out(&usart3_fifo,rx_frame_info+1,4);	
	
	dump(rx_frame_info,10);
	
	if((waittime > 0) && (rx_frame_info[0] == 0xFC)){
		switch (rx_frame_info[3]){
			case 0x01:
				err = 0x01;
				*result = rx_frame_info[4];
				break;
			case 0x02:
				err = 0x02;
				DBG_R_E("Error: Time out\r\n");
				break;
			case 0x03:
				err = 0x03;
				DBG_R_E("Error: Refuse\r\n");
				break;
			case 0x04:
				err = 0x04;
				DBG_R_E("Error: Internal error\r\n");
				break;					
			default:
				err = 0;
				DBG_R_E("Error: Unknow cmd\r\n");
				break;
		}
	}
	if(waittime == 0)
		err = 0;
	

	
	 
	 
	 
	
	//空白部分结束========<<

	ERROR_OUT:
	if(xfs_send_cmd(XFS_CMD_RECOGNIZE_STOP)  != 1){
		err = 0;
		DBG_R_E("Error: XFS_CMD_RECOGNIZE_STOP");

	}
	 DBG_R_E("Succeed: XFS_CMD_RECOGNIZE_STOP");
	return err;
	
	
}




extern volatile char number_write;
extern volatile char number_read;

extern p_state p_state1;

void xfs_delete_number(void)
{
	 char str_call[50]="";
	  num_fifo_out();
	  sprintf(str_call,"请%d号到窗口，ID卡尾号后三位为%x ,%x ,%x ",number_read,num_id[number_read-1][5],num_id[number_read-1][6],num_id[number_read-1][7]);
	  DBG_B_INFO("[ %x %x %x]",num_id[number_read-1][5],num_id[number_read-1][6],num_id[number_read-1][7]);

	  xfs_voice_start((unsigned char *)str_call, XFS_FORMAT_GBK, 1);

	  p_state1=p_delete;
}

void xfs_add_number(void)
{
	xfs_voice_start((unsigned char *)"刷卡成功", XFS_FORMAT_GBK, 1);
}


void xfs_save_id(void)
{
	
	xfs_voice_start((unsigned char *)"正在存储，请勿移开磁卡", XFS_FORMAT_GBK, 1);
	xfs_voice_start((unsigned char *)"存储完成！", XFS_FORMAT_GBK, 1);
	xfs_voice_start((unsigned char *)"磁卡已存在", XFS_FORMAT_GBK, 1);
	xfs_voice_start((unsigned char *)"空间已满", XFS_FORMAT_GBK, 1);
}


void xfs_recognize_test(void)
{  

	
	uint8_t result=30;
	if(xfs_recognize(&result, 1000) == 1)
	{
		DBG_B_INFO("recognition failed!");
		xfs_voice_start((unsigned char *)"语音识别失败", XFS_FORMAT_GBK, 1);
		return;
	}

	xfs_voice_start((unsigned char *)cmd_word_list[result], XFS_FORMAT_GBK, 1);

	
	 if(result==24)
		  lcd_open();		 
	 else if(result==26)
		 lcd_close();

}

/********************************* END OF FILE *********************************/




