/******************************************************************************
******************************************************************************
* @file    app/app_pwd.c 
* @author  zhao
* @version V1.0.0
* @date    2018.07.27
* @update  2018.07.27 
* @brief   a
******************************************************************************
******************************************************************************/
#include "app_pwd.h"
#include "bsp_flash.h"
#include "kfifo.h"
volatile char number_write=0; //接收到的人数数据
volatile char number_read=0;
volatile char number_show=0;//显示的人数数据 = 接收到的人数数据 -  叫号的数据
volatile char number_get=0;//HC595 显示的人数数据
unsigned char id_card[ID_CARD_NUMBER][ID_CARD_PWD_LEN];
unsigned char idCardPwd_offset = 0;//密码保存在缓存中的位置
unsigned char curIdCardPwd[ID_CARD_PWD_LEN];//当前输入的密码
unsigned char num_id[NUM_MAX][NUM_STR_LENGTH];
unsigned char num_fifo_buffer[NUM_MAX*NUM_STR_LENGTH];

//unsigned char num_id[2][NUM_STR_LENGTH]={{1,2,3,4,5,6,7,8},{6,6,6,6,6,6,5,5}};

volatile __SYS_PWD_FLAGS pwd_flags;




kfifo_t num_fifo = {"numfifo",NUM_MAX*NUM_STR_LENGTH,num_fifo_buffer,0,0};





/*
  * @brief  isRightPwd_IdCard 判断ID卡密码是否正确
  * @param  返回buff的 指针
  * @note           
  * @Date:2018.7.27
  * @author:zhao
  * @return:OK:PWD_SUCCESS  NO:PWD_ERROR\PWD_ERRPR_LF
*/
int isRight_Pwd_IdCard(unsigned char * ret_buff)
{
	int i;

    int ret=0;
    unsigned char buf[8];

	ret = LF125K_read_1_page(buf);
//	DBG_B_INFO("[%x %x %x %x %x %x %x %x ]",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
	if(ret == 0)
	{
		memset(buf, 0, sizeof(buf));
	  ret = LF125K_read_1_page(buf);
//		DBG_B_INFO("[%x %x %x %x %x %x %x %x ]",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
	}
	else
		return PWD_ERRPR_LF;
	if(ret == 0)
	{
		for(i=0;i<ID_CARD_NUMBER;i++)
		{
	//  	DBG_B_INFO("[%x %x %x %x %x %x %x %x ]",buf[0],buf[1],buf[2],buf[3],buf[4],buf[5],buf[6],buf[7]);
			 if(memcmp(buf, id_card[i], ID_CARD_PWD_LEN) == 0)
			 {
			    memcpy(ret_buff,buf, 8);
					return PWD_SUCCESS; 
			 }
		}
	 
    		return PWD_ERROR;
	
	}
	else 
		return PWD_ERRPR_LF;
}



/*
  * @brief  paramInit
  * @param  none
  * @note  1.参数初始化
           2.是否初次上电  第一次上电 重置密码 
  * @Date:2018.6.19
  * @author:zhao
  * @return:none
*/
void param_Init(void)
{
	unsigned char save_flag;
    unsigned short NumByteToRead = 1;
	unsigned short NumByteToWrite;
	int i;

    bsp_flash_read(EEP_SAVE_FLAG_ADDRESS, &save_flag, NumByteToRead);

	DBG_B_INFO("%x", save_flag);
	if(save_flag != 0xA3)
	{
		save_flag = 0xA3;
		NumByteToWrite = 1;

		bsp_flash_erase(EEP_SAVE_FLAG_ADDRESS, NumByteToWrite);
		bsp_flash_write(EEP_SAVE_FLAG_ADDRESS, &save_flag, NumByteToWrite);

		//id card pwd
		for(i=0; i < ID_CARD_NUMBER; i++)
		{
		  memset(id_card[i], 0xEE, ID_CARD_PWD_LEN);
		}
		NumByteToWrite = 1;

		bsp_flash_erase(EEP_ID_CARD_PWD_OFFSET_ADDRESS, NumByteToWrite);
		bsp_flash_write(EEP_ID_CARD_PWD_OFFSET_ADDRESS, &idCardPwd_offset, NumByteToWrite);
		NumByteToWrite = ID_CARD_NUMBER * ID_CARD_PWD_LEN;

		bsp_flash_erase(EEP_ID_CARD_PWD_START_ADDRESS, NumByteToWrite);
		bsp_flash_write(EEP_ID_CARD_PWD_START_ADDRESS, (unsigned char *)id_card[0], NumByteToWrite);


		//-------此卡已经空了，请添加添加--------//
	}
}


/*
  * @brief  read_DigitPwd 读取数字密码
  * @param  none
  * @note  1.参数初始化
           2.分配内存 calloc(128,sizeof(char))
           3.获取当前缓存位置
           4.digitPwd缓存写FF
           5.将digitPwd缓存写入 debug_buff  打印 密码  strcat 串联字符串
           6.free 释放内存
  * @Date:2018.6.20
  * @author:zhao
  * @return:(unsigned char **)digitPwd  指针地址
*/
unsigned char **read_IdCardPwd(void)
{	
	unsigned short data_len = 0;
	data_len = 1;
//	sEE_ReadBuffer(&idCardPwd_offset, EEP_ID_CARD_PWD_OFFSET, &data_len);
	bsp_flash_read(EEP_ID_CARD_PWD_OFFSET_ADDRESS, &idCardPwd_offset, data_len);
	memset(id_card[0], 0xff, ID_CARD_NUMBER * ID_CARD_PWD_LEN);
	data_len = ID_CARD_NUMBER * ID_CARD_PWD_LEN;
//	sEE_ReadBuffer(idCardPwd[0], EEP_ID_CARD_PWD_START, &data_len);	
	bsp_flash_read(EEP_ID_CARD_PWD_START_ADDRESS, id_card[0], data_len);
	return (unsigned char **)id_card;

}


/*
  * @brief  存储id卡密码
  * @param  void
  * @note   存储ID卡程序
               1.成功语音播报存储成功
               2.失败语音播报存储失败
               3.卡片存在播报 此卡已存在
  * @Date:  2017.7.10
  * @updatge:2017.7.10
  * @author:zhao
  * @return:void
*/
void save_id_key(void)
{
	unsigned char buf[ID_CARD_NUMBER][ID_CARD_PWD_LEN];
    uint8_t cnt=0;
	int ret=0;
	unsigned short data_len = 0;
  
	change_pwd_flag(PWD_WRITE_FLAG);
	
	memset(buf, 0, sizeof(buf));

	
	for(cnt=0;cnt<=4;cnt++)
	{
		SYSTICK_Delay10ms(100);
		ret = LF125K_read_1_page(buf[cnt]);
		if(ret != 0)
		{
		  DBG_B_INFO("[%x %x %x %x %x %x %x %x ]cnt:%d",buf[cnt][0],buf[cnt][1],buf[cnt][2],buf[cnt][3],buf[cnt][4],buf[cnt][5],buf[cnt][6],buf[cnt][7],cnt);
		  DBG_B_INFO("false1 ");
          change_pwd_flag(PWD_IDLE_FLAG);
		  xfs_voice_start((unsigned char *)"存储失败", XFS_FORMAT_GBK, 1);
		  return ;		
		}
		if((cnt>=1)&&(memcmp(buf[cnt], buf[cnt-1], 8) != 0))
		{
		    DBG_B_INFO("[%x %x %x %x %x %x %x %x ]cnt-1:%d",buf[cnt-1][0],buf[cnt-1][1],buf[cnt-1][2],buf[cnt-1][3],buf[cnt-1][4],buf[cnt-1][5],buf[cnt-1][6],buf[cnt-1][7],cnt-1);
			DBG_B_INFO("[%x %x %x %x %x %x %x %x ]",buf[cnt][0],buf[cnt][1],buf[cnt][2],buf[cnt][3],buf[cnt][4],buf[cnt][5],buf[cnt][6],buf[cnt][7]);
			DBG_B_INFO("false2 ");
			change_pwd_flag(PWD_IDLE_FLAG);
			xfs_voice_start((unsigned char *)"存储失败", XFS_FORMAT_GBK, 1);

	    return ;
		
		}
   }



    //计算是否存在于 id_card 数组中
	for(cnt=0; cnt< ID_CARD_NUMBER; cnt++)
	{
		if(memcmp(buf[0], &id_card[cnt][0], ID_CARD_PWD_LEN) == 0)
		{
			break;
		}
	}
   if(cnt<ID_CARD_NUMBER)
   {
   	DBG_B_INFO("该卡已经存在");
	change_pwd_flag(PWD_IDLE_FLAG);
	xfs_voice_start((unsigned char *)"此卡已经存在", XFS_FORMAT_GBK, 1);
    idCardPwd_offset=cnt;
	data_len = 1;
	bsp_flash_erase(EEP_ID_CARD_PWD_OFFSET_ADDRESS, data_len);
	bsp_flash_write(EEP_ID_CARD_PWD_OFFSET_ADDRESS,&idCardPwd_offset, data_len);
 	return ;
   }
   else
   {
    if(idCardPwd_offset>= ID_CARD_NUMBER)
	 idCardPwd_offset = 0;
   
    memset(id_card[idCardPwd_offset],0xff, ID_CARD_PWD_LEN);
    memcpy(id_card[idCardPwd_offset],buf[0], ID_CARD_PWD_LEN);
    idCardPwd_offset++;
	data_len=1;
	bsp_flash_erase(EEP_ID_CARD_PWD_OFFSET_ADDRESS, data_len);
	bsp_flash_write(EEP_ID_CARD_PWD_OFFSET_ADDRESS,&idCardPwd_offset, data_len);
 	data_len = ID_CARD_NUMBER * ID_CARD_PWD_LEN;

	bsp_flash_erase(EEP_ID_CARD_PWD_START_ADDRESS, data_len);
	bsp_flash_write(EEP_ID_CARD_PWD_START_ADDRESS,(unsigned char *)&id_card[0][0], data_len);
  
    change_pwd_flag(PWD_IDLE_FLAG);
	xfs_voice_start((unsigned char *)"存储成功", XFS_FORMAT_GBK, 1);
    DBG_B_INFO("OK");
	
    return ;
   }
}


/*
  * @brief  app_pwd_add 判断ID卡密码是否正确
  * @param  none
  * @note           
  * @Date:2018.7.27
  * @author:zhao
  * @return:OK:PWD_SUCCESS  NO:PWD_ERROR\PWD_ERRPR_LF
*/

p_state p_state1=p_hold;

unsigned char read_buff[8];

void app_pwd_add(void)
{
	enum PWD_STATUS status;
	status=(enum PWD_STATUS)isRight_Pwd_IdCard(read_buff);
	
	switch(status)
	{
		case PWD_SUCCESS:
		DBG_B_INFO("刷卡成功");
	    DBG_B_INFO("[%x %x %x %x %x %x %x %x ]",read_buff[0],read_buff[1],read_buff[2],read_buff[3],read_buff[4],read_buff[5],read_buff[6],read_buff[7]);
		buzzerOpen();num_fifo_in();p_state1=p_add;break;
		case PWD_ERRPR_LF:
		//DBG_B_INFO("没有磁卡");
		break;
		case PWD_ERROR:
		//DBG_B_INFO("密码错误");
		xfs_voice_start((unsigned char *)"无效卡", XFS_FORMAT_GBK, 1);
		break;
		default:break;
	}
}



/*
  * @brief  num_fifo_in 人数进队列
  * @param  none
  * @note           
  * @Date:2018.8.22
  * @author:zhao
  * @return
*/

void num_fifo_in(void)
{	
     kfifo_t*   pfifo;
	 
     uint8_t*	pfifo_buf;

//	 static uint16_t i;
//	 uint8_t   cnt;
//	 unsigned char write_buf[NUM_STR_LENGTH]={0};
     

//	 for (cnt = 0; cnt < 8; ++cnt)
//	 {
//		 write_buf[cnt]= ++i; 
//	 }

	 
	 pfifo=&num_fifo;

	 pfifo_buf=(uint8_t*)(pfifo->p_buf+pfifo->write_index);
	 
     kfifo_push_in(pfifo,read_buff, NUM_STR_LENGTH);
	 
  	 DBG_B_INFO("fifo [write %d]",pfifo->write_index);
	 
	 
	 number_write=pfifo->write_index/NUM_STR_LENGTH;
	 
	 memcpy(num_id[number_write-1],pfifo_buf, NUM_STR_LENGTH);
	 
	 number_show=number_write-number_read;
	 
     DBG_B_INFO("[num_id[%d] %x %x %x %x %x %x %x %x ]",number_write,num_id[number_write-1][0],num_id[number_write-1][1],num_id[number_write-1][2],  \
	 num_id[number_write-1][3],num_id[number_write-1][4],num_id[number_write-1][5],num_id[number_write-1][6],num_id[number_write-1][7]);
	

}

/*
  * @brief  num_fifo_out 人数出队列
  * @param  none
  * @note           
  * @Date:2018.8.22
  * @author:zhao
  * @return:OK:none
*/

void num_fifo_out(void)
{	
     kfifo_t*   pfifo;
	 
     uint8_t*	pfifo_buf;
	 
	 unsigned char read_buf[NUM_STR_LENGTH]={0};
  
	 
	 
	 pfifo=&num_fifo;
	 
	 pfifo_buf=(uint8_t*)(pfifo->p_buf+pfifo->read_index);
	 
     kfifo_pull_out(pfifo,read_buf, NUM_STR_LENGTH);
	 
  	 DBG_B_INFO("fifo [read %d]",pfifo->read_index);
	 
	 number_read=pfifo->read_index/NUM_STR_LENGTH;

	 number_show=number_write-number_read;
	 
	 memcpy(num_id[number_read-1],pfifo_buf, NUM_STR_LENGTH);
	 
     DBG_B_INFO("[num_id[%d] %x %x %x %x %x %x %x %x ]",number_read,num_id[number_read-1][0],num_id[number_read-1][1],num_id[number_read-1][2],  \
	 num_id[number_read-1][3],num_id[number_read-1][4],num_id[number_read-1][5],num_id[number_read-1][6],num_id[number_read-1][7]);
	

}




/*
  * @brief  get_pwd_flag 获取刷卡状态
  * @param  none
  * @note   #define PWD_IDLE_FLAG     1
			#define PWD_RUN_FLAG      2
			#define PWD_OVER_FLAG     4
			#define PWD_WRITE_FLAG    8        
  * @Date:2018.7.27
  * @author:zhao
  * @return:pwd_flags.VAL
*/
uint8_t get_pwd_flag(void)
{
	return  pwd_flags.VAL;
}


/*
  * @brief  change_pwd_flag 转换刷卡状态
  * @param  newflag (PWD_IDLE_FLAG    PWD_RUN_FLAG  PWD_OVER_FLAG   PWD_WRITE_FLAG)
  * @note  
             1 PWD_IDLE_FLAG   ————>     PWD_RUN_FLAG
             2 PWD_RUN_FLAG    ————>     PWD_IDLE_FLAG
             3 PWD_IDLE_FLAG   ————>     PWD_WRITE_FLAG
             4 PWD_WRITE_FLAG  ————>     PWD_IDLE_FLAG
  * @Date:2018.7.27
  * @author:zhao
  * @return:
*/
void change_pwd_flag(uint8_t newflag)
{
	static __SYS_PWD_FLAGS last_flags;
	last_flags.VAL=pwd_flags.VAL;
	pwd_flags.VAL=newflag;


    if((newflag==PWD_RUN_FLAG)&&(last_flags.VAL==PWD_IDLE_FLAG))
    {
        DBG_B_INFO("空闲到刷卡 ");
     	led_disable();
    }
    else if((newflag==PWD_IDLE_FLAG)&&(last_flags.VAL==PWD_RUN_FLAG))
    {
        led_enable();
		DBG_B_INFO("刷卡到空闲 ");
    }
	else if((newflag==PWD_WRITE_FLAG)&&(last_flags.VAL==PWD_IDLE_FLAG))
	{
	    DBG_B_INFO("空闲到写新卡 ");
		led_disable();

	}
	else if((newflag==PWD_IDLE_FLAG)&&(last_flags.VAL==PWD_WRITE_FLAG))
	{
      led_enable();
      DBG_B_INFO("写新卡到空闲 ");
		
	}
}


/*
  * @brief  PWD_RUN
  * @param  none
  * @note 
  * @Date:2018.7.27
  * @author:zhao
  * @return:none
*/
void PWD_RUN(void)
{

	xfs_voice_start((unsigned char *)"请刷卡", XFS_FORMAT_GBK, 1);

	change_pwd_flag(PWD_RUN_FLAG);
	


}
/*
  * @brief  PWD_RUN
  * @param  none
  * @note 
  * @Date:2018.7.27
  * @author:zhao
  * @return:none
*/
void PWD_IDLE(void)
{


	xfs_voice_start((unsigned char *)"刷卡结束", XFS_FORMAT_GBK, 1);

	change_pwd_flag(PWD_IDLE_FLAG);

}

/*
  * @brief  pwd_task 密码识别进程
  * @param  none
  * @note           
  * @Date:2018.7.27
  * @author:zhao
  * @return:OK:PWD_SUCCESS  NO:PWD_ERROR\PWD_ERRPR_LF
*/
void pwd_task(void)
{
    //change_pwd_flag(PWD_RUN_FLAG);

  switch(pwd_flags.VAL)
	{
      case PWD_IDLE_FLAG: LED_show();break;
	  case PWD_RUN_FLAG:  app_pwd_add();break;
	  case PWD_OVER_FLAG: break;
	  default:break;
	}
	
}




