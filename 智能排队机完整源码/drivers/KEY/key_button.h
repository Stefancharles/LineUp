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
#ifdef __cplusplus
extern "C" {
#endif

#ifndef __KEY_BUTTON_H
#define __KEY_BUTTON_H

/* Includes ------------------------------------------------------------------*/
//#include <stdio.h>
#include "hal_core.h"
#include "string.h"
#include "debug_printf.h"
#include <stdint.h>
//#include "debug_printf.h"
/* Public typedef ------------------------------------------------------------*/
typedef enum 
{  
  BUTTON_1 = 0,
  BUTTON_2,
} Button_TypeDef;

typedef enum 
{  
  BUTTON_MODE_GPIO = 0,
  BUTTON_MODE_EXTI = 1
} ButtonMode_TypeDef;

/* Public define -------------------------------------------------------------*/
/** @addtogroup STM3210E_EVAL_LOW_LEVEL_BUTTON
  * @{
  */  
#define BUTTONn                          2

/**
 * @brief Key push-button-1
 */
#define KEY_BUTTON_1_PIN                   GPIO_Pin_13
#define KEY_BUTTON_1_GPIO_PORT             GPIOC
#define KEY_BUTTON_1_GPIO_CLK              RCC_APB2Periph_GPIOC
#define KEY_BUTTON_1_EXTI_LINE             EXTI_Line13
#define KEY_BUTTON_1_EXTI_PORT_SOURCE      GPIO_PortSourceGPIOC
#define KEY_BUTTON_1_EXTI_PIN_SOURCE       GPIO_PinSource13
#define KEY_BUTTON_1_EXTI_IRQn             EXTI15_10_IRQn
/**
 * @brief Key push-button-2
 */
#define KEY_BUTTON_2_PIN                   GPIO_Pin_13
#define KEY_BUTTON_2_GPIO_PORT             GPIOD
#define KEY_BUTTON_2_GPIO_CLK              RCC_APB2Periph_GPIOD
#define KEY_BUTTON_2_EXTI_LINE             EXTI_Line13
#define KEY_BUTTON_2_EXTI_PORT_SOURCE      GPIO_PortSourceGPIOD
#define KEY_BUTTON_2_EXTI_PIN_SOURCE       GPIO_PinSource13
#define KEY_BUTTON_2_EXTI_IRQn             EXTI15_10_IRQn


typedef void (*k_action_p)	 (void);
/**
  * @}
  */ 

/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
/* Public function prototypes ------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
uint32_t key_button_get_status(Button_TypeDef Button);
void key_button_init(Button_TypeDef Button, ButtonMode_TypeDef Button_Mode);

void key_irq(Button_TypeDef ch);

void bsp_key_init(Button_TypeDef ch,k_action_p p_short);
void bsp_key_init1(void);
void key_poll(void);
void key_task(void);

#endif /* __KEY_BUTTON_H */
#ifdef __cplusplus
}
#endif


