
/******************************************************************************
******************************************************************************
* @file    app/main.h 
* @author  zhao
* @version V1.0.0
* @date    2018.07.10
* @update  2018.07.17 
* @brief   主程序
******************************************************************************
******************************************************************************/



/**************include*************************/
#include "hal_core.h"
#include "string.h"
#include "usmart.h"	
#include "debug_printf.h"
#include "WiFiToCloud.h"
#include "app_pwd.h"
/**************code*************************/


/*
  * @brief  main
  * @param  none
  * @note   int main(void)          
  * @Date:  2017.7.10
  * @updatge:2017.7.10
  * @author:zhao
  * @return:int
*/
int main(void)
{

  hal_init();
  
  xfs_voice_start((unsigned char *)"欢迎使用智能排队机", XFS_FORMAT_GBK, 1);
  
  show_logo();

  param_Init();
 
  read_IdCardPwd();
  
  TIM3_Init();
  
  change_pwd_flag(PWD_IDLE_FLAG);
  
  ESP8266_init();
 
	while(1)
	{		
     cloud_task(); 
		
	   pwd_task();		 
			
	   lcd_task();		
		
	   key_task();

	  
	}
	
	
}
 


