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
// ������ 
typedef enum {
	XFS_CMD_STATUS = 0x21,//��ѯ״̬
	XFS_CMD_SLEEP = 0x88,//����͹���ģʽ
	XFS_CMD_WAKE_UP = 0xFF,//�ӵ͹��ķ�������ģʽ
	XFS_CMD_VOICE_START = 0x01,//��ʼ�����ϳ�
	XFS_CMD_VOICE_STOP = 0x02,//ֹͣ�����ϳ�
	XFS_CMD_VOICE_PAUSE = 0x03,//��ͣ�����ϳ�
	XFS_CMD_VOICE_RESTORE = 0x04,//�ָ������ϳ�
	XFS_CMD_SEND_TO_CACHE = 0x31,//�����ı���������
	XFS_CMD_PLAY_FROM_CACHE = 0x32,//�ӻ����������ı�
	XFS_CMD_RECOGNIZE_START = 0x10,
	XFS_CMD_RECOGNIZE_STOP = 0x1F
}xfsCommandEnum;


#define XFS_RESPONSE_TIMEOUT 300


// ����ʶ������� 
const char *cmd_word_list[] = {
	"���ڳԷ�",  //0
	"�����޳�",  //1
	"���ڼ���",  //2
	"������Ϣ",//3
	"ͬ��",//4
	"��ͬ��",//5
	"��ȥ",//6
	"���ڼ���",//7
	"���켸��",//8
	"����Ϣ",//9
	"��ʼ��",//10
	"�����Ķ�",//11
	"�򿪹㲥",//12
	"�ص��㲥",//13
	"������",//14
	"�ص�����",//15
	"����һ��",//16
	"�ٶ�һ��",//17
	"������",//18
	"С����",//19
	"������",//20
	"��Ԥ����Ϣ",//21
	"������������",//22
	"����Ԥ����Ϣ",//23
	"��ʼ",//24
	"ֹͣ",//25
	"��ͣ",//26
	"������",//27
	"ȷ����ʼ",//28
	"ȡ��"//29
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
  * @brief  ��������
	* @param  cmd: ����
  * @retval 1���ɹ�
  * @retval 0��ʧ��
  */
static int xfs_send_cmd(uint8_t cmd)
{
	unsigned  char frame_info[8]; //������ı�����
	uint16_t frame_len = 0;
	 
/*****************֡�̶�������Ϣ**************************************/           
	frame_info[frame_len++] = 0xFD ; 			//����֡ͷFD
	frame_info[frame_len++] = 0x00 ; 			//�������������ȵĸ��ֽ�
	frame_info[frame_len++] = 0x01; 		//�������������ȵĵ��ֽ�
	frame_info[frame_len++] = cmd ; 			//���������� 
 
/*******************����֡��Ϣ***************************************/

	uart3_send_buff(frame_info, frame_len);
  
	return wait_response(XFS_ACK_CMD_RIGHT, XFS_RESPONSE_TIMEOUT);
}


/**
  * @brief  ���״̬
  * @param  
  * @retval ״̬��
  */
int xfs_get_status(void)
{
	unsigned char rec_buf[8];
	unsigned  char frame_info[8]; //������ı�����
	uint16_t frame_len = 0;
	int timeout = XFS_RESPONSE_TIMEOUT;
	
	memset(rec_buf, 0, sizeof(rec_buf));
/*****************֡�̶�������Ϣ**************************************/           
	frame_info[frame_len++] = 0xFD ; 			//����֡ͷFD
	frame_info[frame_len++] = 0x00 ; 			//�������������ȵĸ��ֽ�
	frame_info[frame_len++] = 0x01; 		//�������������ȵĵ��ֽ�
	frame_info[frame_len++] = XFS_CMD_STATUS ; 			//���������� 
 
/*******************����֡��Ϣ***************************************/
	uart3_send_buff(frame_info, frame_len);

/*******************�ȴ�����***************************************/

	while (--timeout)
	{
		delay_ms(1);

     if(USART_GetFlagStatus(USART3,USART_FLAG_RXNE)!=RESET)//ȷ�����ǲ��Ǵ���1�����ж�
     {
	     rec_buf[0] =USART_ReceiveData(USART3);	   
	      return  rec_buf[0];     
     }
	}
	return -1;

	
}
/**
  * @brief  ����˯��ģʽ
  * @param  
  * @retval 1���ɹ�
  * @retval 0��ʧ��
  */
int xfs_sleep(void)
{
	return xfs_send_cmd(XFS_CMD_SLEEP);
}
/**
  * @brief  ����
  * @param  
  * @retval 1���ɹ�
  * @retval 0��ʧ��
  */
int xfs_wake_up(void)
{
	return xfs_send_cmd(XFS_CMD_WAKE_UP);
}

/**
  * @brief  ��ʼ��������
  * @param  data�������ı�
  * @param  format:�ı������ʽ 0-GB2312,1-GBK,2-BIG,3-UNICODE
  * @param  wait_finish���Ƿ�ȴ�оƬ����
  * @retval 1���ɹ�
  * @retval 0��ʧ��
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
		
	data_length =strlen((char*)data); 			//��Ҫ�����ı��ĳ���
	frame_info = (unsigned char *)calloc((unsigned int)data_length, sizeof(unsigned char));
	if(frame_info == NULL)
		return -1;

/*****************֡�̶�������Ϣ**************************************/           
	frame_info[frame_offset++] = 0xFD ; 			//����֡ͷFD
	frame_info[frame_offset++] = (data_length>>8) & 0xFF; 			//�������������ȵĸ��ֽ�
	frame_info[frame_offset++] = (data_length & 0xFF) + 2; 	  	//�������������ȵĵ��ֽ�
	frame_info[frame_offset++] = XFS_CMD_VOICE_START ; 		    	//���������֣��ϳɲ�������		 		 
	frame_info[frame_offset++] = format;                        //�ı������ʽ��GBK 
 
/*******************����֡��Ϣ***************************************/		  
	memcpy(&frame_info[5], data, data_length);
	frame_offset += data_length;
//	ptrUSART->Send(frame_info, frame_offset);//����֡����
	uart3_send_buff(frame_info, frame_offset);
//	while (ptrUSART->GetTxCount() < frame_offset);
	ack = wait_response(XFS_ACK_CMD_RIGHT, XFS_RESPONSE_TIMEOUT);
	
	if(frame_info)
		free(frame_info);
	
	return ack;
}
/**
  * @brief  ֹͣ����
  * @param  
  * @retval 1���ɹ�
  * @retval 0��ʧ��
  */
int xfs_voice_stop(void)
{
	return xfs_send_cmd(XFS_CMD_VOICE_STOP);
}
/**
  * @brief  ��ͣ����
  * @param  
  * @retval 1���ɹ�
  * @retval 0��ʧ��
  */
int xfs_voice_pause(void)
{
	return xfs_send_cmd(XFS_CMD_VOICE_PAUSE);
}
/**
  * @brief  ��������
  * @param  
  * @retval 1���ɹ�
  * @retval 0��ʧ��
  */
int xfs_voice_restore(void)
{
	return xfs_send_cmd(XFS_CMD_VOICE_RESTORE);
}



/**
  * @brief  �����ı���������
  * @param  data���ı�����
  * @param  cache_num (0~15)
  * @retval 1���ɹ�
  * @retval 0��ʧ��
  * @note  ����ռ��ܹ�4K,����16��,ÿ����256�ֽ�
					�豾����ʼ��������=X(0<=X<=15),�򱾴η��͵��ı����Ȳ��ܴ���(16-X)*256�ֽڣ�������ı�������
					�û��ڲ��Ż����ı�ǰ���ɶ�η��ͻ���������мǺ������͵��ı����ܲ��ָ��ǻ�ȫ������֮ǰ���ı���
					���򲻱�֤���ŵ���ȷ��
  */
int xfs_send_to_cache(unsigned char *data, unsigned char cache_num)
{
	unsigned char *frame_info = NULL; //������ı�����
	unsigned short data_length, frame_offset = 0;  
	unsigned char ack = 0;
	
	if(data == NULL)
		return -1;
	if(cache_num >= 16)
		return -1;
	data_length = strlen((char*)data); 			//��Ҫ�����ı��ĳ���
	if(data_length > (16-cache_num)*256)
		return -1;
	frame_info = (unsigned char *)calloc((unsigned int)data_length, sizeof(unsigned char));
	if(frame_info == NULL)
		return -1;
/*****************֡�̶�������Ϣ**************************************/           
	frame_info[frame_offset++] = 0xFD ; 			//����֡ͷFD
	frame_info[frame_offset++] = (data_length>>8) & 0xFF; 			//�������������ȵĸ��ֽ�
	frame_info[frame_offset++] = (data_length & 0xFF) + 2; 		//�������������ȵĵ��ֽ�
	frame_info[frame_offset++] = XFS_CMD_SEND_TO_CACHE ; 			//����������	 
	frame_info[frame_offset++] = cache_num;       //�ı������ʽ��GBK 
 
/*******************����֡��Ϣ***************************************/		  
	memcpy(&frame_info[frame_offset], data, data_length);
	frame_offset += data_length;
//	ptrUSART->Send(frame_info,frame_offset);//����֡����
//	while (ptrUSART->GetTxCount() < frame_offset);
	uart3_send_buff(frame_info, frame_offset);
	ack = wait_response(XFS_ACK_CMD_RIGHT, XFS_RESPONSE_TIMEOUT);
	
	if(frame_info)
		free(frame_info);
	return ack;

}

/**
  * @brief  ���Ż�������
  * @param  num(1~15)
  * @param  format:0-GB2312,1-GBK,2-BIG,3-UNICODE
  * @retval 1���ɹ�
  * @retval 0��ʧ��
  */
int xfs_paly_from_cache(unsigned char num, unsigned char format)
{
	unsigned  char frame_info[8];
	unsigned short frame_len = 0;
	
	if(format > XFS_FORMAT_UNICODE || num > 15)
	return -1;

/*****************֡�̶�������Ϣ**************************************/           
	frame_info[frame_len++] = 0xFD ; 			//����֡ͷFD
	frame_info[frame_len++] = 0x00 ; 			//�������������ȵĸ��ֽ�
	frame_info[frame_len++] = 0x02; 		//�������������ȵĵ��ֽ�
	frame_info[frame_len++] = XFS_CMD_PLAY_FROM_CACHE ; 			//���������� 
 	frame_info[frame_len++] = (num << 4 & 0xF0) | (format & 0x0F); 			//����������λ-�ظ����Ŵ���������λ-�ı������ʽ

/*******************����֡��Ϣ***************************************/
	uart3_send_buff(frame_info, frame_len);
	return wait_response(XFS_ACK_CMD_RIGHT, XFS_RESPONSE_TIMEOUT);

}
/*
  * @brief  ��ӡ����
  * @param  buf    ��ӡ����
            len    ��ӡ����
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
  * @brief  ����ʶ��
  * @param  result[out]: ʶ����
  * @param  waittime[in]: ��ʱʱ��
  * @retval >0��   �ɹ�
  * @retval 0 ��  ʧ��
  * @note �� �����ڴ˺���ʵ������ʶ����  ָ��*result���� ʶ�����Ĺؼ�����     
                                           waittime    ����ȴ�ʱ��  ������0.5����ʶ������   waittime=500
                                
             xfs5152ceģ���շ�ʹ�õĴ���Ϊ  USART3

             ����ʶ���������:
                       MCU���ڷ�������       FD 00 01 10  ��xfs5152
                             |
                             |
                             |
                       ������˷�˵��
						     |
							 |
							 |                            
                          �ȴ�����      FC 00 2A cmd    
                             |
                             |
                             |
                           �ؼ���      ��cmd_word_list[]��һһ��Ӧ
                         
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

	/*���²�����ѡ�ֲ���*/
	//�հײ�����ʼ========>>

	
	

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
	

	
	 
	 
	 
	
	//�հײ��ֽ���========<<

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
	  sprintf(str_call,"��%d�ŵ����ڣ�ID��β�ź���λΪ%x ,%x ,%x ",number_read,num_id[number_read-1][5],num_id[number_read-1][6],num_id[number_read-1][7]);
	  DBG_B_INFO("[ %x %x %x]",num_id[number_read-1][5],num_id[number_read-1][6],num_id[number_read-1][7]);

	  xfs_voice_start((unsigned char *)str_call, XFS_FORMAT_GBK, 1);

	  p_state1=p_delete;
}

void xfs_add_number(void)
{
	xfs_voice_start((unsigned char *)"ˢ���ɹ�", XFS_FORMAT_GBK, 1);
}


void xfs_save_id(void)
{
	
	xfs_voice_start((unsigned char *)"���ڴ洢�������ƿ��ſ�", XFS_FORMAT_GBK, 1);
	xfs_voice_start((unsigned char *)"�洢��ɣ�", XFS_FORMAT_GBK, 1);
	xfs_voice_start((unsigned char *)"�ſ��Ѵ���", XFS_FORMAT_GBK, 1);
	xfs_voice_start((unsigned char *)"�ռ�����", XFS_FORMAT_GBK, 1);
}


void xfs_recognize_test(void)
{  

	
	uint8_t result=30;
	if(xfs_recognize(&result, 1000) == 1)
	{
		DBG_B_INFO("recognition failed!");
		xfs_voice_start((unsigned char *)"����ʶ��ʧ��", XFS_FORMAT_GBK, 1);
		return;
	}

	xfs_voice_start((unsigned char *)cmd_word_list[result], XFS_FORMAT_GBK, 1);

	
	 if(result==24)
		  lcd_open();		 
	 else if(result==26)
		 lcd_close();

}

/********************************* END OF FILE *********************************/




