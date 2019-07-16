/**************************************************************************************************************
*
*
*
*
*
*
*
**************************************************************************************************************/
#include "bsp_timer3.h"
#include "stdio.h"
#include "ledNixietube.h"
static uint8_t Scom;
bool led_flag=true; //led ��־λ  
uint8_t volatile	 Ledbuf[8];
extern uint8_t tick_1ms	;
extern void hc595_out_for_int(uint8_t dH, uint8_t dL);
/*************LED*************************************/
const uint8_t ledseg_tab[]  =
{
        0xC0,/*0*/
        0xF9,/*1*/
        0xA4,/*2*/
        0xB0,/*3*/
        0x99,/*4*/
        0x92,/*5*/
        0x82,/*6*/
        0xF8,/*7*/
        0x80,/*8*/
        0x90,/*9*/
        0x88,/*A*/
        0x83,/*b*/
        0xA7,/*c*/
        0xA1,/*d*/
        0x86,/*E*/
        0x8E,/*F*/
        0x89,/*H 10*/
        0x8B,/*h 11*/
        0xC7,/*L 12*/
        0xAB,/*n 13*/
        0xC8,/*N 14*/
        0xA3,/*o 15*/
        0x8C,/*P 16*/
        0x98,/*q 17*/
        0xAF,/*r 18*/
        0x87,/*t 19 */
        0xC1,/*U 1a*/
        0x91,/*y 1b*/
        0xBF,/*- 1c*/
        0xFF, /* 1D*/
        0xFF,
        0x00, /*  1F*/
 }	  ;
const uint8_t com_tab[8]  ={1,2,4,8,0x10,0x20,0x40,0x80};
///***************************************************
//�� �� ����
//�������ܣ�
//��    ����loca: 0~7��ʾ���ҿ�ʼ�ĵ�1λ����8λ
//�� �� ֵ����
//****************************************************/
///****************************************************************************************/
//void led_out(uint8_t dat, uint8_t loca)
//{
//	uint8_t seg,com;
//	P1 =loca;
//	if((dat&0x7f)>MAX_LEDCH)
//		dat =MAX_LEDCH;
//	seg=ledseg_tab[dat&0x7f];
//	if(dat&0x80)
//		seg &=SEG_H;
//	com=0x01<<(loca&0x07); 
//	hc595_out(com,seg);
//}
/***************************************************
�� �� ����
�������ܣ�led��̬��ʾ
��    ������
�� �� ֵ����
****************************************************/
/****************************************************************************************/
void led_scan_run(void)
{	/*
	static uint8_t Scoms;
	if(tick_1ms)
	{
		tick_1ms=0;
		Scoms++;
		Scoms &=7;
		led_out(Ledbuf[Scoms],Scoms);	
	}	*/
}	

/***************************************************
�� �� ����
�������ܣ�led��̬��ʾ
			�жϺ�������ã�С�ı�д��������
��    ������
�� �� ֵ����
****************************************************/
//#pragma NOAREGS   //ָ����������Ҫ�Ըú���ʹ�þ��ԼĴ���Ѱַ��ʽ
void led_scan(void)  
{
	
  uint8_t segi,dati;
	Scom++;
	if(Scom>=7)
	{
	  Scom &=7;
    Scom |=5;
	}

	dati=Ledbuf[Scom];
	if((dati&0x7f)>MAX_LEDCH)
		dati=MAX_LEDCH;
	segi=ledseg_tab[dati&0x7f];
	if(dati&0x80)
		segi &=SEG_H;
 	hc595_out_for_int(com_tab[Scom],segi);


}
//#pragma AREGS 


/***************************************************
�� �� ����
�������ܣ�������ʾ����
��ڲ�����uint8_t *buf, uint8_t pos,uint8_t len
		  ��ʾ����    ��ʼλ��     ����  	
���ڲ���������
�� �� ֵ����
****************************************************/
void set_led_dispbuf(uint8_t *buf, uint8_t pos,uint8_t len)
{
	uint8_t	i=0;
	while( (pos<8) && (len!=0) )
	{
		Ledbuf[pos]=buf[i];
		i++;
		pos++;
		len--;
	}
}


/***************************************************
�� �� ����clr_led_dispbuf
�������ܣ����������ʾLED��ʾ����
��ڲ����� none 	
���ڲ���������
�� �� ֵ����
****************************************************/
void clr_led_dispbuf(void)
{
	Ledbuf[0] =LED_SPACE;
	Ledbuf[1] =LED_SPACE;
	Ledbuf[2] =LED_SPACE;
	Ledbuf[3] =LED_SPACE;
	Ledbuf[4] =LED_SPACE;
	Ledbuf[5] =LED_SPACE;
	Ledbuf[6] =LED_SPACE;
	Ledbuf[7] =LED_SPACE;
}
/***************************************************
�� �� ����Bcd2disp
�������ܣ�
��ڲ�����  	
���ڲ���������
�� �� ֵ����
****************************************************/
void Bcd2disp(uint8_t *bcd, uint8_t *buf,uint8_t len)
{
	if(len>4)
		len=4;
	while(len--)
	{
		*buf= *bcd >>4;
		buf++;
		*buf= *bcd & 0x0f;
		buf++;
		bcd++;		
	}	

}


/*
  * @brief  LED_show led��ʾ���� ��������
  * @param  none
  * @note        
  * @Date:  2017.7.10
  * @updatge:2017.7.10
  * @author:zhao
  * @return:void
*/
void LED_show(void)
{
	uint8_t buf_led[8]={0};	
	//�������ʾ
    buf_led[7]=number_get%10;
	buf_led[6]=number_get/10%10;
	buf_led[5]=number_get/100%10;
	buf_led[4]=0x1D;
	buf_led[3]=0x1D;
	buf_led[2]=0x1D;
	buf_led[1]=0x1D;
	buf_led[0]=0x1D;	
	set_led_dispbuf(buf_led,0,8); //��ʾ����

}







/*
  * @brief  led_disable �ر�LED��ʾ
  * @param  none
  * @note        
  * @Date:  2017.7.10
  * @updatge:2017.7.10
  * @author:zhao
  * @return:void
*/
void led_disable(void)
{
    clr_led_dispbuf();

	while(led_flag==true)
	{
	  //  DBG_B_INFO("�ȴ�led �ر�  led_flag %d",led_flag);	
	   // TIM_ITConfig(TIM3,TIM_IT_CC1,DISABLE);
	   //  led2s=0;
       //     break;
	 }


	TIM_ITConfig(TIM3,TIM_IT_CC1,DISABLE);

}

/*
  * @brief  led_enable ����LED��ʾ
  * @param  none
  * @note        
  * @Date:  2017.7.10
  * @updatge:2017.7.10
  * @author:zhao
  * @return:void
*/
void led_enable(void)
{
  if(led_flag==false)
	{
	 TIM_ITConfig(TIM3,TIM_IT_CC1,ENABLE);
	 led2s=0;
	 led_flag=true;
	
	}	

}
/******************* (C) COPYRIGHT xxx *****END OF FILE****/


