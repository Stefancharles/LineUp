/******************************************************************************
******************************************************************************
* @file    app/hal_core.c 
* @author  zhao
* @version V1.0.0
* @date    2018.8.1
* @update  2018.8.2
* @brief   hal_core
******************************************************************************
******************************************************************************/


/**************include*************************/

#include "hal_core.h"
#include "debug_printf.h"


/**************code*************************/


/*
  * @brief  NVIC_Priority_Group_Configuration
  * @param  none
  * @note        
  * @Date:  2017.7.10
  * @updatge:2017.7.10
  * @author:zhao
  * @return:void
*/
void NVIC_Priority_Group_Configuration(void)
{	
	NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
}



/*
  * @brief  rtc_update_notify
  * @param  uint32_t rtc_counter_val  rtc计数器值
  * @note        
  * @Date:   2017.8.5
  * @updatge:2017.8.5
  * @author:zhao
  * @return:void
*/
uint32_t rtc_second, is_rtc_update;
void rtc_update_notify(uint32_t rtc_counter_val)
{
	is_rtc_update = 1;
	rtc_second = rtc_counter_val;
}

/*
  * @brief  hal_init
  * @param  void
  * @note    1.初始化函数    
  * @Date:   2017.8.5
  * @author: zhao
  * @return: int32_t
*/
int32_t	hal_init(void)
{
	NVIC_Priority_Group_Configuration();

  SYSTICK_init();  
	uart_init(115200);	 	//串口初始化256000
 	usmart_dev.init(72);	//初始化USMART
 	delay_init();
  lcd_init();	
	rtc_init(NULL);
	xfs_init();
 	UART4_Init(115200);
	LF125K_init();
     //buzzer
	buzzerInit();
	SI4332A_Init();
	SI4332B_Init();
	bsp_key_init(BUTTON_2,protocol_task);
    bsp_key_init1();
	HC595_GpioInit();
	
	return 0;
}


