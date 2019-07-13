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
#include "stm32f10x.h"
#include "systick.h"
#include "key_button.h"
#include "delay.h"
#include "debug_printf.h"
#include "app_pwd.h"
/* Public variables ----------------------------------------------------------*/
#define KEY_SHORT  0x01
#define KEY_LONG   0x02
#define KEY_DOUBLE 0x03

GPIO_TypeDef* BUTTON_PORT[BUTTONn] = {KEY_BUTTON_1_GPIO_PORT, KEY_BUTTON_2_GPIO_PORT}; 

const uint16_t BUTTON_PIN[BUTTONn] = {KEY_BUTTON_1_PIN, KEY_BUTTON_2_PIN}; 

const uint32_t BUTTON_CLK[BUTTONn] = {KEY_BUTTON_1_GPIO_CLK, KEY_BUTTON_2_GPIO_CLK};

const uint16_t BUTTON_EXTI_LINE[BUTTONn] = {KEY_BUTTON_1_EXTI_LINE, KEY_BUTTON_2_EXTI_LINE};

const uint16_t BUTTON_PORT_SOURCE[BUTTONn] = {KEY_BUTTON_1_EXTI_PORT_SOURCE, KEY_BUTTON_2_EXTI_PORT_SOURCE};

const uint16_t BUTTON_PIN_SOURCE[BUTTONn] = {KEY_BUTTON_1_EXTI_PIN_SOURCE, KEY_BUTTON_2_EXTI_PIN_SOURCE};
	
const uint16_t BUTTON_IRQn[BUTTONn] = {KEY_BUTTON_1_EXTI_IRQn, KEY_BUTTON_2_EXTI_IRQn};







/**
  * @brief  Configures Button GPIO and EXTI Line.
  * @param  Button: Specifies the Button to be configured.
  *   This parameter can be one of following parameters:   
  *     @arg BUTTON_1
  *     @arg BUTTON_2  
  * @param  Button_Mode: Specifies Button mode.
  *   This parameter can be one of following parameters:   
  *     @arg BUTTON_MODE_GPIO: Button will be used as simple IO 
  *     @arg BUTTON_MODE_EXTI: Button will be connected to EXTI line with interrupt
  *                     generation capability  
  * @retval None
  */
void key_button_init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode)
{
  GPIO_InitTypeDef GPIO_InitStructure;
  EXTI_InitTypeDef EXTI_InitStructure;
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Enable the BUTTON Clock */
  RCC_APB2PeriphClockCmd(BUTTON_CLK[Button] | RCC_APB2Periph_AFIO, ENABLE);

  /* Configure Button pin as input floating */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_InitStructure.GPIO_Pin = BUTTON_PIN[Button];
  GPIO_Init(BUTTON_PORT[Button], &GPIO_InitStructure);

 
  if (Button_Mode == BUTTON_MODE_EXTI)
  {
    /* Connect Button EXTI Line to Button GPIO Pin */
    GPIO_EXTILineConfig(BUTTON_PORT_SOURCE[Button], BUTTON_PIN_SOURCE[Button]);

    /* Configure Button EXTI line */
    EXTI_InitStructure.EXTI_Line = BUTTON_EXTI_LINE[Button];
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;

    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Rising_Falling;  

    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);

    /* Enable and set Button EXTI Interrupt to the lowest priority */
    NVIC_InitStructure.NVIC_IRQChannel = BUTTON_IRQn[Button];
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x0F;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;

    NVIC_Init(&NVIC_InitStructure); 
  }
}

/**
  * @brief  Returns the selected Button state.
  * @param  Button: Specifies the Button to be checked.
  *   This parameter can be one of following parameters:    
  *     @arg BUTTON_1
  *     @arg BUTTON_2   
  * @retval The Button GPIO pin value.
  */
uint32_t key_button_get_status(Button_TypeDef Button)
{
  return GPIO_ReadInputDataBit(BUTTON_PORT[Button], BUTTON_PIN[Button]);
}

/* Private variables ---------------------------------------------------------*/
enum
{
    key_state_idle = 0,
    key_state_down ,
    key_state_hold ,
    key_state_up,
    key_state_confirm,
    key_state_judge,
    key_action,  
};

/* Public variables ----------------------------------------------------------*/
uint32_t key_test_state[BUTTONn] 	 ={key_state_idle};
uint32_t last_key_test_state[BUTTONn] ={key_state_idle};
uint32_t key_code[BUTTONn] =       {0};
uint8_t  flag_is_double[BUTTONn] = {0};


/*
  * @brief  change_key_state 改变按键状态
  * @param  uint32_t tag_state  :
							        key_state_idle = 0,
								    key_state_down ,
								    key_state_hold ,
								    key_state_up,
								    key_state_confirm,
								    key_state_judge,
								    key_action,  
            Button_TypeDef ch  按键选择
   
  * @note     1.last_key_test_state 上次按键状态      
  			  2.key_test_state 此次状态
  * @Date:2018.7.27
  * @author:zhao
  * @return:none
*/
static void change_key_state(Button_TypeDef ch,uint32_t tag_state)
{
    last_key_test_state[ch] = key_test_state[ch];
    key_test_state[ch] = tag_state;
}


/*
  * @brief  key_irq_handle 中断回调
  * @param  uint32_t io_level  电平信号
            Button_TypeDef ch  按键选择
   
  * @note     1.last_key_test_state 上次按键状态      
  			  2.key_test_state 此次状态
  * @Date:2018.7.27
  * @author:zhao
  * @return:none
*/
void key_irq_handle(Button_TypeDef ch,uint32_t io_level)
{
	if(ch==BUTTON_2)
	{
//		DBG_B_INFO("key_irq_handle level:%d  ms:%ld  state:0x%x  is_double:%d",io_level,SYSTICK_get_time(),key_test_state[ch],flag_is_double[ch]);
		if(io_level==0)
		{
			 change_key_state(ch,key_state_down);
		}
		else 
		{
			 change_key_state(ch,key_state_up); 
		}
		//DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[ch],key_test_state[ch]);
	}

}
/* Private variables ---------------------------------------------------------*/

static uint32_t down_time[BUTTONn] ={0};
static uint32_t up_time[BUTTONn]   = {0};

/* Private typedef -----------------------------------------------------------*/

struct _key_obj_
{
	k_action_p  kp_short;
	k_action_p	kp_long;
	k_action_p	kp_double;
};


/* Public variables ----------------------------------------------------------*/

struct _key_obj_  mykey[BUTTONn];

/*
  * @brief  bsp_key_init 按键初始化
  * @param  k_action_p         函数指针注册  长按 短按 双击
            Button_TypeDef ch  按键选择
  * @note    kp_double   双击函数
  			 kp_long     长按函数
  			 kp_short    短按函数
  * @Date:2018.7.27
  * @author:zhao
  * @return:none
*/
void bsp_key_init(Button_TypeDef ch,k_action_p p_short)
{
	//key_button_init(BUTTON_1, BUTTON_MODE_EXTI);
	  key_button_init(ch, BUTTON_MODE_EXTI);

   // mykey[ch].kp_double=p_double;
   //  mykey[ch].kp_long=p_long;
  	mykey[ch].kp_short=p_short;
}




/*
  * @brief  bsp_key1_init 按键1初始化
  * @param  
  * @note    kp_double   双击函数
  			 kp_long     长按函数
  			 kp_short    短按函数
  * @Date:2018.7.27
  * @author:zhao
  * @return:none
*/
void bsp_key_init1(void)
{
	 //key_button_init(BUTTON_1, BUTTON_MODE_EXTI);
	  key_button_init(BUTTON_1, BUTTON_MODE_GPIO);
   //   mykey[0].kp_double=NULL;
    //  mykey[0].kp_long=NULL;
 	  mykey[0].kp_short=save_id_key;
}


/*
  * @brief  key_action_callback 按键回调函数
  * @param  
  * @note    kp_double   双击函数
  			 kp_long     长按函数
  			 kp_short    短按函数
  * @Date:2018.7.27
  * @author:zhao
  * @return:none
*/
static void key_action_callback(Button_TypeDef ch)
{
	switch (key_code[ch])
	{
	 //case KEY_LONG:mykey[ch].kp_long();break;
	 //case KEY_DOUBLE:mykey[ch].kp_double();break;
	 case KEY_SHORT:mykey[ch].kp_short();break;
	 default:break;
	}
}


/*
  * @brief  按键1轮训方式
  * @param  
  * @note    
  * @Date:2018.7.27
  * @author:zhao
  * @return:none
*/
void key_poll(void)
{
	 uint32_t  io_level;

	 io_level=key_button_get_status(BUTTON_1);
	
	 if((io_level==1)&&(key_test_state[BUTTON_1]==key_state_idle))  //按键没有按下
		{
		 //DBG_B_INFO("key_irq_handle level:%d  ms:%ld  state:0x%x  is_double:%d",io_level,SYSTICK_get_time(),key_test_state[ch],flag_is_double[ch]);
	  	 //change_key_state(ch,key_state_up); 
		}
	else if((io_level==0)&&((key_test_state[BUTTON_1]==key_state_idle)||(key_test_state[BUTTON_1]==key_state_confirm)))
		//按键没有按下
	{
  	 change_key_state(BUTTON_1,key_state_down);
	 DBG_B_INFO("按键按下  ms:%ld  state:0x%x  is_double:%d",SYSTICK_get_time(),key_test_state[BUTTON_1],flag_is_double[BUTTON_1]);
	}
	else if((io_level==1)&&(key_test_state[BUTTON_1]==key_state_hold))
	{
  	 change_key_state(BUTTON_1,key_state_up);
	 DBG_B_INFO("按键弹起  ms:%ld  state:0x%x  is_double:%d",SYSTICK_get_time(),key_test_state[BUTTON_1],flag_is_double[BUTTON_1]);
	}
	
	 switch(key_test_state[BUTTON_1])
    {
        case key_state_idle:     break;
        case key_state_down:     
        {
        if(last_key_test_state[BUTTON_1]==key_state_confirm)  //是不是2次
	      {
		      flag_is_double[BUTTON_1] = 1;
			 // DBG_B_INFO("双击条件使能");
			  DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[BUTTON_1],key_test_state[BUTTON_1]);
		    }
         down_time[BUTTON_1] = SYSTICK_get_time();//记录 down时间
         change_key_state(BUTTON_1,key_state_hold);
        }break;
        case key_state_hold:
		{
			if( (SYSTICK_get_time() - down_time[BUTTON_1]) > 300)
			{
					key_code[BUTTON_1] = KEY_LONG;
		    	//	DBG_B_INFO("长按");
					DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[BUTTON_1],key_test_state[BUTTON_1]);
					change_key_state(BUTTON_1,key_state_judge);
			}
		}break;
      case key_state_up:  
		{
			if(last_key_test_state[BUTTON_1]==key_state_idle)
			{
				change_key_state(BUTTON_1,key_state_idle);
			}
		    else
			{
				 up_time[BUTTON_1] =    SYSTICK_get_time() ;
				 if( (up_time[BUTTON_1] - down_time[BUTTON_1]) > 20)
				 {
					change_key_state(BUTTON_1,key_state_confirm);
				 	//DBG_B_INFO("进入确认单机双击状态");
				    DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[BUTTON_1],key_test_state[BUTTON_1]);
				}   
				else
				{
					DBG_B_INFO("按键恢复，进入等待  up_time :%ld  down_time :%ld",up_time[BUTTON_1],down_time[BUTTON_1]);
					DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[BUTTON_1],key_test_state[BUTTON_1]);
					change_key_state(BUTTON_1,key_state_idle);  
				}               
			}
					 
		} break;
        case key_state_confirm: 
        {
           if( (SYSTICK_get_time() - up_time[BUTTON_1])  > 100)
           {
             if(flag_is_double[BUTTON_1])
			 {
				key_code[BUTTON_1] = KEY_DOUBLE;
				//DBG_B_INFO("确认双击");
				 DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[BUTTON_1],key_test_state[BUTTON_1]);
			 }
             else
			 {
				key_code[BUTTON_1] = KEY_SHORT;
			//	DBG_B_INFO("确认单击");
				 DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[BUTTON_1],key_test_state[BUTTON_1]);
			 }
			  change_key_state(BUTTON_1,key_state_judge);
           }
        } break;
        case key_state_judge:   
        {
            DBG_B_INFO("key jude to action ");
	    	DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[BUTTON_1],key_test_state[BUTTON_1]);
            change_key_state(BUTTON_1,key_action);
        }break;
        case key_action:       
        {
            flag_is_double[BUTTON_1] = 0;
            DBG_B_INFO("key code :%d",key_code[BUTTON_1]);
		    if(key_code[BUTTON_1]==2)
			   while(key_button_get_status(BUTTON_1)==0)
		     {
		    //	DBG_B_INFO("啦啦啦 我死在这里啦");
			   }
			  //DBG_B_INFO("哈哈哈，我逃出来了");
           change_key_state(BUTTON_1,key_state_idle);
           key_action_callback(BUTTON_1);
        } break; 
    
        default: break;
	
}
}


//----中断方式操作 按键-----//
void key_irq(Button_TypeDef ch)
{
    
    switch(key_test_state[ch])
    {
        case key_state_idle:     break;
        case key_state_down:     
        {
        if(last_key_test_state[ch]==key_state_confirm)
	      {
		    	flag_is_double[ch] = 1;
			  //DBG_B_INFO("双击条件使能");
			  //DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[ch],key_test_state[ch]);
		    }
         down_time[ch] = SYSTICK_get_time();
         change_key_state(ch,key_state_hold);
        }break;
        case key_state_hold:
		{
			if( (SYSTICK_get_time() - down_time[ch]) > 400)
			{
					key_code[ch] = KEY_LONG;
//		    		DBG_B_INFO("长按");
//					DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[ch],key_test_state[ch]);
					change_key_state(ch,key_state_judge);
			}
		}break;
       case key_state_up:  
		{
			if(last_key_test_state[ch]==key_state_idle)
			{
				change_key_state(ch,key_state_idle);
			}
		    else
			{
				 up_time[ch] =    SYSTICK_get_time() ;
				 if( (up_time[ch] - down_time[ch]) > 20)
				 {
					change_key_state(ch,key_state_confirm);
				 //	DBG_B_INFO("按键恢复，进入确认");
				    DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[ch],key_test_state[ch]);
				}   
				else
				{
					DBG_B_INFO("按键恢复，进入等待  up_time :%ld  down_time :%ld",up_time[ch],down_time[ch]);
					DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[ch],key_test_state[ch]);
					change_key_state(ch,key_state_idle);  
				}               
			}
					 
		} break;
        case key_state_confirm: 
        {
           if( (SYSTICK_get_time() - up_time[ch])  > 100)
           {
//             if(flag_is_double[ch])
//			 {
//				 key_code[ch] = KEY_DOUBLE;
//				// DBG_B_INFO("确认双击");
//				 DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[ch],key_test_state[ch]);
//			 }
//             else
//			 {
				key_code[ch] = KEY_SHORT;
			
				 DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[ch],key_test_state[ch]);
//			 }
			  change_key_state(ch,key_state_judge);
           }
        } break;
        case key_state_judge:   
        {
//            DBG_B_INFO("key jude to action ");
//			      DBG_B_INFO("last_key_test_state :%d  key_test_state:%d ",last_key_test_state[ch],key_test_state[ch]);
            change_key_state(ch,key_action);
        }break;
        case key_action:       
        {
            flag_is_double[ch] = 0;
//            DBG_B_INFO("key code :%d",key_code[ch]);
            change_key_state(ch,key_state_idle);
            key_action_callback(ch);
        } break; 
    
        default: break;
    }
}


/*
  * @brief  key_task
  * @param  void
  * @note   按键2中断方式
            按键1轮训方式
  * @Date:2018.7.27
  * @author:zhao
  * @return:none
*/
void key_task(void)
{
   key_poll();
   key_irq(BUTTON_2);
}

/*
  * @brief  EXTI15_10_IRQHandler
  * @param  
  * @note    IRQ Line10~Line15
  * @Date:2018.7.27
  * @author:zhao
  * @return:none
*/
void EXTI15_10_IRQHandler(void)
{

	 if(EXTI_GetITStatus(EXTI_Line13) != RESET)
    {	 
      
        key_irq_handle(BUTTON_2,key_button_get_status(BUTTON_2));
			 // key_irq_handle(key_button_get_status(BUTTON_1));
        EXTI_ClearITPendingBit(EXTI_Line13);
    }
}
/********************************* END OF FILE *********************************/

