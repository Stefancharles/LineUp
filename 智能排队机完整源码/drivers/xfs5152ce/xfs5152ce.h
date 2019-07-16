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

#ifndef __XFS_5152_CE_H
#define __XFS_5152_CE_H

/* Includes ------------------------------------------------------------------*/
/* Public typedef ------------------------------------------------------------*/
/* 中文编码格式 */
typedef enum {
	XFS_FORMAT_GB2312 = 0x00,
	XFS_FORMAT_GBK = 0x01,
	XFS_FORMAT_BIG5 = 0x02,
	XFS_FORMAT_UNICODE = 0x03,
}xfsEnodingFormatEnum;

/* 响应状态 */
typedef enum {
	XFS_ACK_INIT_SUCCESS = 0x4A,
	XFS_ACK_CMD_RIGHT = 0x41,
	XFS_ACK_CMD_ERROR = 0x45,
	XFS_ACK_BUSY = 0x4E,
	XFS_ACK_IDEL = 0x4F,
}xfsAckStatusEnum;

/* Public define -------------------------------------------------------------*/
/* Public macro --------------------------------------------------------------*/
/* Public variables ----------------------------------------------------------*/
extern const char *cmd_word_list[];
/* Public function prototypes ------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
int xfs_init(void);
int xfs_get_status(void);
int xfs_sleep(void);
int xfs_wake_up(void);
int xfs_voice_start(unsigned char *data, unsigned char format, int wait_finish);
int xfs_voice_stop(void);
int xfs_voice_pause(void);
int xfs_voice_restore(void);
int xfs_send_to_cache(unsigned char *data, unsigned char cache_num);
int xfs_paly_from_cache(unsigned char num, unsigned char format);
int xfs_recognize(unsigned char *result, unsigned int waittime);
void xfs_delete_number(void);
void xfs_add_number(void);
void xfs_save_id(void);
void xfs_recognize_test(void);




#endif /* __XFS_5152_CE_H */
#ifdef __cplusplus
}
#endif


