/******************************************************************************
******************************************************************************
* @file    app/app_pwd.h 
* @author  zhao
* @version V1.0.0
* @date    2018.07.27
* @update  2018.07.27 
* @brief   
******************************************************************************
******************************************************************************/
#ifndef __APP_PWD_H__
#define __APP_PWD_H__

#ifdef __cplusplus  /* C++支持 */
	extern "C"{
#endif
#include "hal_core.h"
#include "stdbool.h"
#include "string.h"
#include "usmart.h"	
#include "debug_printf.h"

//extern 
enum PWD_STATUS
{
   PWD_SUCCESS=0,
	 PWD_ERROR=1,
	 PWD_ERRPR_LF=2,
};

typedef enum people_state_enum
{
	p_add=0,
	p_delete=1,
	p_hold=2,
}p_state;


#define PWD_IDLE_FLAG     1
#define PWD_RUN_FLAG      2
#define PWD_OVER_FLAG     4
#define PWD_WRITE_FLAG    8



typedef union
{
    struct
    {
        unsigned pwd_idle									                    : 1;
        unsigned pwd_runing              	   					        : 1;
        unsigned pwd_over                                     : 1;
	    unsigned pwd_write_flash                                : 1;
    }val;
    uint8_t VAL;
}__SYS_PWD_FLAGS;


// typedef struct
// {
//	 uint8_t num;
//	 uint8_t num_id[ID_CARD_PWD_LEN];
// }pwd_param;


 
#define NUM_MAX 64
#define NUM_STR_LENGTH 8
extern unsigned char num_id[NUM_MAX][NUM_STR_LENGTH];



 /**
  * EEPROM start address in flash
  * As for STM32F103VE (512KB flash), 0x0807F000 is the begining of the last 2 pages.
  * Check the *.map file (in listing folder) to find out the used ROM size.
  */
#define EEP_SAVE_FLAG_ADDRESS             ((uint32_t)0x0807D800)
#define EEP_ID_CARD_PWD_OFFSET_ADDRESS    ((uint32_t)0x0807F000)
#define EEP_ID_CARD_PWD_START_ADDRESS     ((uint32_t)0x0807F800)




 //LF id card password
#define ID_CARD_NUMBER 5//id card密码长度
#define ID_CARD_PWD_LEN 8//id card密码长度

 extern unsigned char id_card[ID_CARD_NUMBER][ID_CARD_PWD_LEN];//id card密码缓存
 extern unsigned char idCardPwd_offset;//密码保存在缓存中的位置
 extern unsigned char curIdCardPwd[ID_CARD_PWD_LEN];//当前输入的密码

 uint8_t get_pwd_flag(void);
 void change_pwd_flag(uint8_t newflag);
extern volatile char number_write;
extern unsigned char read_buff[NUM_STR_LENGTH];
void app_pwd_add(void);
void pwd_task(void);
void save_id_key(void);
void param_Init(void);


void num_fifo_in(void);
void num_fifo_out(void);


unsigned char **read_IdCardPwd(void);
void PWD_IDLE(void);
void PWD_RUN(void);

void test_fifo(u8 cmd);


/*===========================================================================*/
#ifdef __cplusplus  /* C++支持 */
	}
#endif
/*===========================================================================*/
#endif


