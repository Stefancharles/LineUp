/******************************************************************************
******************************************************************************
* @file    app/usmart/usmart_config.c 
* @author  zhao
* @version V1.0.0
* @date    2018.07.17
* @update  2018.07.17 
* @brief   usmart��� �����ļ�
******************************************************************************
******************************************************************************/

#include "usmart.h"
#include "usmart_str.h"	   
////////////////////////////�û�������///////////////////////////////////////////////
//������Ҫ�������õ��ĺ�����������ͷ�ļ�(�û��Լ����) 
#include "systick.h"
#include "usart.h"		
#include "sys.h"
#include "usart3.h"
#include "xfs5152ce.h"
#include "LCD12864.h"
#include "lf125k.h"
#include "app_pwd.h"
#include "potocol.h"
//�������б��ʼ��(�û��Լ����)
//�û�ֱ������������Ҫִ�еĺ�����������Ҵ�
struct _m_usmart_nametab usmart_nametab[]=
{


#if USMART_USE_WRFUNS==1 	//���ʹ���˶�д����
	(void*)read_addr,"u32 read_addr(u32 addr)",
	(void*)write_addr,"void write_addr(u32 addr,u32 val)",	 
#endif

    (void*)LF125K_card_init,"int8_t LF125K_card_init(void)",
	(void*)xfs_recognize_test,"void xfs_recognize_test(void)",
	(void*)save_id_key,"void save_id_key(void)",
	(void*)Time_Adjust,"void Time_Adjust(uint16_t syear,uint8_t smon, uint8_t sday, uint8_t hour, uint8_t minute, uint8_t second)",

};		
	
///////////////////////////////////END///////////////////////////////////////////////
/////////////////////////////////////////////////////////////////////////////////////
//�������ƹ�������ʼ��
//�õ������ܿغ���������
//�õ�����������
struct _m_usmart_dev usmart_dev=
{
	usmart_nametab,
	usmart_init,
	usmart_cmd_rec,
	usmart_exe,
	usmart_scan,
	sizeof(usmart_nametab)/sizeof(struct _m_usmart_nametab),//��������
	0,	  	//��������
	0,	 	//����ID
	1,		//������ʾ����,0,10����;1,16����
	0,		//��������.bitx:,0,����;1,�ַ���	    
	0,	  	//ÿ�������ĳ����ݴ��,��ҪMAX_PARM��0��ʼ��
	0,		//�����Ĳ���,��ҪPARM_LEN��0��ʼ��
};   



















