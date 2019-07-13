/**
  ******************************************************************************
  * @file    enc28j60.c
  * @author  
  * @version 
  * @date    
  * @brief   ENC28J60����ģ��   
  ******************************************************************************
  * @attention
  *
  *
  ******************************************************************************
  */ 
/*
********************************************************************************
*                                   
* ģ������ : ENC28J60����ģ��   
* �ļ����� : enc28j60.c
* ��    �� : V0.1
* ˵    �� : 
* �޸ļ�¼ :
*   �汾��  ����        ����      ˵��
*   V0.1    2013-07-12  �쿭      �Ż�����
*   V0.2    2013-07-14  �쿭      ������ӦLwIP����������
*
********************************************************************************
*/
/* Includes ------------------------------------------------------------------*/
#include  "ethernet\enc28j60.h"
#include  "ethernet\bsp_spi1.h"

/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
// ��Ҫ����Ŀ����޸�����
#define 	ENC28J60_CSL()		GPIO_ResetBits(GPIOA,GPIO_Pin_4)
#define 	ENC28J60_CSH()		GPIO_SetBits(GPIOA,GPIO_Pin_4)


/*INT GPIO EXTI*/
#define ENC28J60_INT_GPIO               GPIOA
#define ENC28J60_INT_RCC_APB2Periph     RCC_APB2Periph_GPIOA
#define ENC28J60_INT_PIN                GPIO_Pin_2
#define ENC28J60_INT_EXTI_LINE          EXTI_Line2
#define ENC28J60_INT_PORT_SOURCE        GPIO_PortSourceGPIOA
#define ENC28J60_INT_PIN_SOURCE         GPIO_PinSource2
#define ENC28J60_INT_NVIC_IRQ           EXTI2_IRQn
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* �洢�����  ENC28J60 ����Bank0��Bank3 4���洢�� ��Ҫͨ��ECON1�Ĵ���ѡ�� */
static unsigned char enc28j60_bank;
/* ��һ�����ݰ�ָ�룬��������ֲ�P43 ͼ7-3 */
static unsigned int next_pkt;
/* �жϻص����� */
enc28j60_irq_callback_fn enc28j60_irq_cb = (void *)0;
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  �ж���������
  * @param  None
  * @retval None
  */
static void enc28j60_gpio_configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    RCC_APB2PeriphClockCmd(ENC28J60_INT_RCC_APB2Periph | RCC_APB2Periph_AFIO, ENABLE);
    GPIO_InitStructure.GPIO_Pin =  ENC28J60_INT_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
    GPIO_Init(ENC28J60_INT_GPIO, &GPIO_InitStructure);
}



/**
  * @brief  �ж�����NVIC����
  * @param  None
  * @retval None
  */
static  void enc28j60_nviv_configuration(void)
{
    NVIC_InitTypeDef NVIC_InitStructure;

    NVIC_InitStructure.NVIC_IRQChannel = ENC28J60_INT_NVIC_IRQ;
    NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0;
    NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
    NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
    NVIC_Init(&NVIC_InitStructure);
}

/**
  * @brief  �ⲿ�ж�����
  * @param  None
  * @retval None
  */
static  void enc28j60_exit_configuration(void)
{
    EXTI_InitTypeDef EXTI_InitStructure;

    GPIO_EXTILineConfig(ENC28J60_INT_PORT_SOURCE, ENC28J60_INT_PIN_SOURCE);
    EXTI_InitStructure.EXTI_Line = ENC28J60_INT_EXTI_LINE;
    EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;
    EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling ; /*��?��y??�䣤����*/
    EXTI_InitStructure.EXTI_LineCmd = ENABLE;
    EXTI_Init(&EXTI_InitStructure);
    EXTI_GenerateSWInterrupt(ENC28J60_INT_EXTI_LINE);	/*?D??1����? EXTI_Line0  ??*/
}


/**
  * @brief  int pin status
  * @param  None
  * @retval None
  */
int enc28j60_int_status(void)
{
    return GPIO_ReadOutputDataBit(ENC28J60_INT_GPIO, ENC28J60_INT_PIN);
}
/**
  * @brief  irq init
  * @param  None
  * @retval None
  */
void enc28j60_irq_init(void)
{
    enc28j60_gpio_configuration();
    enc28j60_exit_configuration();
    enc28j60_nviv_configuration();
}
/**
  * @brief  EXTI0_IRQHandler	
						This function handles External lines 0 interrupt request.
  * @param  None
  * @retval None
  */
void EXTI2_IRQHandler(void)
{
    if ( EXTI_GetITStatus(ENC28J60_INT_EXTI_LINE) != RESET )  
    {  
			if(enc28j60_irq_cb)
				enc28j60_irq_cb(1);
			EXTI_ClearITPendingBit(ENC28J60_INT_EXTI_LINE);		
    }
    
}
/**
  * @brief  ����˵��: ���Ĵ�������
						ʹ��˵��: �ú���֧�ֵĲ�����ֻ�����ƼĴ��� ��������
  * @param  unsigned char op        ������
            unsigned char address   �Ĵ�����ַ
  * @retval ��������
  */
unsigned char enc28j60_readop(unsigned char op, unsigned char address)
{
	unsigned char dat = 0;
	
	/* CS���� ʹ��ENC28J60 */
	ENC28J60_CSL();
	/* ������͵�ַ */
	dat = op | (address & ADDR_MASK);
    /* ͨ��SPIд����*/
	bsp_spi_read_write_byte(dat);
    /* ͨ��SPI�������� */
	dat = bsp_spi_read_write_byte(0xFF);
	
    /* �����MAC��MII�Ĵ�������һ����ȡ���ֽ���Ч������Ϣ�����ڵ�ַ�����λ*/
	if(address & 0x80)
	{
		/* �ٴ�ͨ��SPI��ȡ���� */
        dat = bsp_spi_read_write_byte(0xFF);
	}
	
    /* CS���� ��ֹENC28J60 */
	ENC28J60_CSH();
    
    /* �������� */
	return dat;
}
/**
  * @brief  ����˵��: д�Ĵ�������
						ʹ��˵��: �ú���֧�ֵĲ�������: д���ƼĴ��� λ������ λ����1
  * @param  unsigned char op        ������
	*         unsigned char address   �Ĵ�����ַ
	*         unsigned char data      д������
  * @retval None
  */
void enc28j60_writeop(unsigned char op, unsigned char address, unsigned char data)
{
	unsigned char dat = 0;
    /* ʹ��ENC28J60 */							  	  
	ENC28J60_CSL();	     
    /* ͨ��SPI���� ������ͼĴ�����ַ */                 		
	dat = op | (address & ADDR_MASK);
    /* ͨ��SPI1�������� */
	bsp_spi_read_write_byte(dat);
    /* ׼���Ĵ�����ֵ */				  
	dat = data;
    /* ͨ��SPI�������� */
	bsp_spi_read_write_byte(dat);
    /* ��ֹENC28J60 */				 
	ENC28J60_CSH();	
}
/**
  * @brief  ����˵��: ��������
						ʹ��˵��: ������uIP��LwIP
	*         					LwIP�У���low_level_input�е���
	*      		ʹ�÷���: ���read data into( q->payload, q->len );
  * @param  unsigned int len        ��ȡ����
	*         unsigned char* data     ��ȡָ��
  * @retval None
  */
void enc28j60_readbuf(char* pdata,int len)
{
    /* ʹ��ENC28J60 */
    ENC28J60_CSL();
	/* ͨ��SPI���Ͷ�ȡ����������*/
	bsp_spi_read_write_byte(ENC28J60_READ_BUF_MEM);
    
    /* ѭ����ȡ */
	while(len)
	{
        len--;
        /* ��ȡ���� */
        *pdata = (unsigned char)bsp_spi_read_write_byte(0);
        /* ��ַָ���ۼ� */
        pdata++;
	}
    
    /* ��ֹENC28J60 */
	ENC28J60_CSH();
}
/**
  * @brief  ����˵��: д������
						ʹ��˵��: ��������LwIP����low_level_output�е���
	*         					���send data from(q->payload, q->len);
  * @param  unsigned int len        ��ȡ����
	*         unsigned char* data     ��ȡָ��
  * @retval None
  */
void enc28j60_writebuf(char* pdata,int len )
{
    /* ʹ��ENC28J60 */
    ENC28J60_CSL();
	/* ͨ��SPI����дȡ���������� */
	bsp_spi_read_write_byte(ENC28J60_WRITE_BUF_MEM);
	
    /* ѭ������ */
	while(len)
	{
		len--;
        /* �������� */
		bsp_spi_read_write_byte(*pdata);
        /* ��ַָ���ۼ� */
		pdata++;
	}
    
    /* ��ֹENC28J60 */
	ENC28J60_CSH();
}
/**
  * @brief  �趨�Ĵ����洢����
  * @param  unsigned char address   �Ĵ�����ַ
  * @retval None
  */
void enc28j60_setbank(unsigned char address)
{
	/* ���㱾�μĴ�����ַ�ڴ�ȡ�����λ�� */
	if((address & BANK_MASK) != enc28j60_bank)
	{
        /* ���ECON1��BSEL1 BSEL0 ��������ֲ�15ҳ */
        enc28j60_writeop(ENC28J60_BIT_FIELD_CLR, ECON1, (ECON1_BSEL1|ECON1_BSEL0));
        /* ��ע��Ĵ�����ַ�ĺ궨�壬bit6 bit5����Ĵ����洢����λ�� */
        enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON1, (address & BANK_MASK)>>5);
        /* ����ȷ����ǰ�Ĵ����洢���� */
        enc28j60_bank = (address & BANK_MASK);
	}
}
/**
  * @brief  ��ȡ�Ĵ���ֵ
  * @param  unsigned char address   �Ĵ�����ַ
  * @retval None
  */
unsigned char enc28j60_read(unsigned char address)
{
	/* �趨�Ĵ�����ַ���� */
	enc28j60_setbank(address);
	/* ��ȡ�Ĵ���ֵ ���Ͷ��Ĵ�������͵�ַ */
	return enc28j60_readop(ENC28J60_READ_CTRL_REG, address);
}
/**
  * @brief  д�Ĵ���
  * @param  unsigned char address   �Ĵ�����ַ
	*         unsigned char data      �Ĵ�����ֵ
  * @retval None
  */
void enc28j60_write(unsigned char address, unsigned char data)
{
	/* �趨�Ĵ�����ַ���� */
	enc28j60_setbank(address);
	/* д�Ĵ���ֵ ����д�Ĵ�������͵�ַ */
	enc28j60_writeop(ENC28J60_WRITE_CTRL_REG, address, data);
}

/**
  * @brief  ����˵��: д����Ĵ���
						ʹ��˵��: PHY�Ĵ�������ͨ��SPI����ֱ�ӷ��ʣ�����ͨ��һ������ļĴ���������
	*         					��������ֲ�19ҳ
  * @param  unsigned char address   ����Ĵ�����ַ
	*         unsigned int data       ����Ĵ�����ֵ ����Ĵ�����Ϊ16λ��
  * @retval None
  */
void enc28j60_writephy(unsigned char address, unsigned int data)
{
	/* ��MIREGADRд���ַ ��������ֲ�19ҳ*/
	enc28j60_write(MIREGADR, address);
	/* д���8λ���� */
	enc28j60_write (MIWRL, data);
    /* д���8λ���� */
	enc28j60_write(MIWRH, data>>8);
	/* �ȴ�PHY�Ĵ���д����� */
	while( enc28j60_read(MISTAT) & MISTAT_BUSY );
}

/**
  * @brief  ����˵��: д����Ĵ���
						ʹ��˵��: PHY�Ĵ�������ͨ��SPI����ֱ�ӷ��ʣ�����ͨ��һ������ļĴ���������
	*         					��������ֲ�19ҳ
  * @param  unsigned char address   ����Ĵ�����ַ
	*					unsigned int data       ����Ĵ�����ֵ ����Ĵ�����Ϊ16λ��
  * @retval None
  */
unsigned int enc28j60_readhy(unsigned char address)
{
	unsigned int data = 0;
	/* ��MIREGADRд���ַ ��������ֲ�19ҳ*/
	enc28j60_write(MIREGADR, address);
		/* �趨�Ĵ�����ַ���� */
	enc28j60_setbank(MICMD);
	/* ����MICMD.MIIRD,��ʼ������ */
	enc28j60_writeop(ENC28J60_BIT_FIELD_SET, MICMD, MICMD_MIIRD);
	/* �ȴ�PHY�Ĵ�������� */
	while( enc28j60_read(MISTAT) & MISTAT_BUSY );	
		/* �趨�Ĵ�����ַ���� */
	enc28j60_setbank(MICMD);
	/* ����MICMD.MIIRD,��ʼ������ */
	enc28j60_writeop(ENC28J60_BIT_FIELD_CLR, MICMD, MICMD_MIIRD);
	/* �����8λ���� */
	data = 0x00FF&enc28j60_read(MIRDL);
    /* �����8λ���� */
	data |= 0xFF00&((unsigned int)enc28j60_read(MIRDH) << 8);
	
	return data;
}


/**
  * @brief  �����·״̬
  * @param  1��link up��0��link down
  * @retval None
  */
int enc28j60_get_link_status(void)
{
	/* ��phystat1�Ĵ��� */
	if(enc28j60_readhy(PHSTAT1) & PHSTAT1_LLSTAT)
		return 1;
	else
		return 0;
}

/**
  * @brief  ��ʼ��enc28j60
  * @param  unsigned char* mac_addr  mac��ַ����ָ��
  * @retval None
  */
void enc28j60_init(unsigned char* mac_addr, enc28j60_irq_callback_fn cb_fn)
{
	GPIO_InitTypeDef GPIO_InitStructure;  
	
	/* ��GPIOAʱ�� */
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA , ENABLE);
	
	/* enc28j60 CS @GPIOA.4 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_4;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	bsp_spi_init();
	
    /* ��ֹENC28J60 */
    ENC28J60_CSH();
	/* ENC28J60�����λ �ú������ԸĽ� */
	enc28j60_writeop(ENC28J60_SOFT_RESET, 0, ENC28J60_SOFT_RESET); 
    /* ��ѯESTAT.CLKRDYλ */
	while(!(enc28j60_read(ESTAT) & ESTAT_CLKRDY));
    
	/* ���ý��ջ�������ʼ��ַ �ñ�������ÿ�ζ�ȡ������ʱ������һ�������׵�ַ */
	next_pkt = RXSTART_INIT;
    
    /* ���ý��ջ����� ��ʼָ�� */
	enc28j60_write(ERXSTL, RXSTART_INIT & 0xFF);
	enc28j60_write(ERXSTH, RXSTART_INIT >> 8);
    
    /* ���ý��ջ����� ��ָ�� */ 
	enc28j60_write(ERXRDPTL, RXSTART_INIT&0xFF);
	enc28j60_write(ERXRDPTH, RXSTART_INIT>>8);
    
    /* ���ý��ջ����� ����ָ�� */
	enc28j60_write(ERXNDL, RXSTOP_INIT&0xFF);
	enc28j60_write(ERXNDH, RXSTOP_INIT>>8);
    
	/* ���÷��ͻ����� ��ʼָ�� */
	enc28j60_write(ETXSTL, TXSTART_INIT&0xFF);
	enc28j60_write(ETXSTH, TXSTART_INIT>>8);
	/* ���÷��ͻ����� ����ָ�� */
	enc28j60_write(ETXNDL, TXSTOP_INIT&0xFF);
	enc28j60_write(ETXNDH, TXSTOP_INIT>>8);
    
    /* ʹ�ܵ������� ʹ��CRCУ�� ʹ�� ��ʽƥ���Զ�����*/
	enc28j60_write(ERXFCON, ERXFCON_UCEN | ERXFCON_CRCEN | ERXFCON_BCEN);
//	enc28j60_write(ERXFCON, ERXFCON_UCEN|ERXFCON_CRCEN|ERXFCON_PMEN);
	enc28j60_write(EPMM0,   0x3f);
	enc28j60_write(EPMM1,   0x30);
	enc28j60_write(EPMCSL,  0xf9);
	enc28j60_write(EPMCSH,  0xf7);
    
    /* ʹ��MAC���� ����MAC������ͣ����֡ �����յ���ͣ����֡ʱֹͣ����*/
    /* �����ֲ�34ҳ */
	enc28j60_write(MACON1, MACON1_MARXEN|MACON1_TXPAUS|MACON1_RXPAUS);
    
    /* �˳���λ״̬ */
	enc28j60_write(MACON2, 0x00);
    
    /* ��0������ж�֡��60�ֽڳ� ��׷��һ��CRC ����CRCʹ�� ֡����У��ʹ�� MACȫ˫��ʹ��*/
	/* ��ʾ ����ENC28J60��֧��802.3���Զ�Э�̻��ƣ����ԶԶ˵����翨��Ҫǿ������Ϊȫ˫�� */
	enc28j60_writeop( ENC28J60_BIT_FIELD_SET, MACON3, 
                     MACON3_PADCFG0|MACON3_TXCRCEN|MACON3_FRMLNEN|MACON3_FULDPX );
    
    /* ����Ĭ��ֵ */
	enc28j60_write(MAIPGL, 0x12);
    /* ����Ĭ��ֵ */
	enc28j60_write(MAIPGH, 0x0C);
    /* ����Ĭ��ֵ */
	enc28j60_write(MABBIPG, 0x15);
    
    /* ���֡���� */
	enc28j60_write(MAMXFLL, MAX_FRAMELEN & 0xFF);	
	enc28j60_write(MAMXFLH, MAX_FRAMELEN >> 8);
    
    /* д��MAC��ַ */
	enc28j60_write(MAADR5, mac_addr[0]);	
	enc28j60_write(MAADR4, mac_addr[1]);
	enc28j60_write(MAADR3, mac_addr[2]);
	enc28j60_write(MAADR2, mac_addr[3]);
	enc28j60_write(MAADR1, mac_addr[4]);
	enc28j60_write(MAADR0, mac_addr[5]);
    
	/* ����PHYΪȫ˫��  LEDBΪ������ */
	enc28j60_writephy(PHCON1, PHCON1_PDPXMD);
    
    /* LED״̬ */
    enc28j60_writephy(PHLCON,0x0476);	
    
    /* ��˫���ػ���ֹ */
	enc28j60_writephy(PHCON2, PHCON2_HDLDIS);
    
    /* ����BANK0 */	
	enc28j60_setbank(ECON1);
    
    /* ʹ���ж� ȫ���ж� �����ж� ���մ����ж� */
	enc28j60_writeop(ENC28J60_BIT_FIELD_SET, EIE, EIE_INTIE|EIE_PKTIE|EIE_RXERIE);
    
    /* ����ʹ��λ */
	enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_RXEN);
  
	/* �жϳ�ʼ�� */
	if(cb_fn){
		enc28j60_irq_cb = cb_fn;
		enc28j60_irq_init();
	}
}


/**
  * @brief  ����˵��: ͨ��enc28j60�������ݰ�
						ʹ��˵��: ��������uIP
  * @param  int len       ���ݰ�����
	* 	      char* packet  ���ݰ�ָ��
  * @retval None
  */
void enc28j60_packet_send(char* packet,int len )
{
	/* ��ѯ�����߼���λλ */
	while((enc28j60_read(ECON1) & ECON1_TXRTS)!= 0);
    
    /* ���÷��ͻ�������ʼ��ַ */    
	enc28j60_write(EWRPTL, TXSTART_INIT & 0xFF);
	enc28j60_write(EWRPTH, TXSTART_INIT >> 8);
    
	/* ���÷��ͻ�����������ַ ��ֵ��Ӧ�������ݰ����� */   
	enc28j60_write(ETXNDL, (TXSTART_INIT + len) & 0xFF);
	enc28j60_write(ETXNDH, (TXSTART_INIT + len) >>8);
    
	/* ���Ϳ����ֽ� �����ֽ�Ϊ0x00,��ʾʹ��macon3���� */
    enc28j60_writeop(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
    
	/* ͨ��ENC28J60�������ݰ� */
	enc28j60_writebuf( packet,len );
    
	/* �������� */
	enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
    
    /* ��λ�����߼������⡣�μ� Rev. B4 Silicon Errata point 12. */
	if( (enc28j60_read(EIR) & EIR_TXERIF) )
	{
		enc28j60_setbank(ECON1);
        enc28j60_writeop(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
    }
}


/**
  * @brief  ����˵��: ���ݰ����ͺ�����1�� �޸ķ��ͻ������������Ϳ����ֽ�.
	*         ʹ��˵��: �����ֽ�Ϊ0������macon3�������Զ����0��ʹ��CRCУ��
											������LwIP����low_level_out��ʹ�ã����initiate transfer();
						���÷���:enc28j60_init_send(p->tot_len); // ��ʾpbuf���ݰ����峤��
  * @param  int len       ���ݰ�����
  * @retval None
  */
void enc28j60_init_send(int len)
{
	/* ��ѯ�����߼���λλ */
	while((enc28j60_read(ECON1) & ECON1_TXRTS)!= 0);
    
    /* ���÷��ͻ�������ʼ��ַ */    
	enc28j60_write(EWRPTL, TXSTART_INIT & 0xFF);
	enc28j60_write(EWRPTH, TXSTART_INIT >> 8);
    
	/* ���÷��ͻ�����������ַ ��ֵ��Ӧ�������ݰ����� */   
	enc28j60_write(ETXNDL, (TXSTART_INIT + len) & 0xFF);
	enc28j60_write(ETXNDH, (TXSTART_INIT + len) >>8);
    
	/* ���Ϳ����ֽ� �����ֽ�Ϊ0x00,��ʾʹ��macon3���� */
    enc28j60_writeop(ENC28J60_WRITE_BUF_MEM, 0, 0x00);
}


/**
  * @brief  ����˵��: ���ݰ����ͺ�����3�� ��������
						ʹ��˵��: ������LwIP����low_level_out��ʹ��
	*         ���signal that packet should be sent();
						���÷���: enc28j60_start_send
  * @param  None
  * @retval None
  */
void enc28j60_start_send( void )
{
	uint8_t try_count = 0;
	
	/* �������� */
	start_tx:
	enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRTS);
    
    /* ��λ�����߼������⡣�μ� Rev. B4 Silicon Errata point 12. */
	if( (enc28j60_read(EIR) & EIR_TXERIF) )
	{
		//ֹͣ����
		enc28j60_writeop(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRTS);
		//��λ�����߼�
		enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON1, ECON1_TXRST);
		enc28j60_writeop(ENC28J60_BIT_FIELD_CLR, ECON1, ECON1_TXRST);
		//�����־λ
		enc28j60_writeop(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXERIF);
		enc28j60_writeop(ENC28J60_BIT_FIELD_CLR, EIR, EIR_TXIF);
		try_count++;
		if(try_count < 15)
			goto start_tx;
  }
}


/**
  * @brief  ����˵��: ͨ��enc28j60�������ݰ�
						ʹ��˵��: ��������uIP
  * @param  unsigned int maxlen       �������ݰ���󳤶�ֵ
	*         unsigned char* packet     ���ݰ�ָ��
  * @retval �������ݰ�����
  */
int enc28j60_packet_receive(char* packet,int maxlen )
{
	int rxstat;
	int len;
    
	/* �Ƿ��յ���̫�����ݰ� */
	if( enc28j60_read(EPKTCNT) == 0 )
	{
		return 0;
  }
    
	/* ���ý��ջ�������ָ�� */
	enc28j60_write( ERDPTL, (next_pkt) );
	enc28j60_write( ERDPTH, (next_pkt)>>8 );
    
    /* �������ݰ��ṹʾ�� �����ֲ�43ҳ */
    
	/* ����һ������ָ�� */
	next_pkt  = enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 );
	next_pkt |= enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 )<<8;
    
	/* �����ĳ��� */
	len  = enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 );
	len |= enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 )<<8;
    
    /* ɾ��CRC���� */
    len -= 4; 	
    
	/* ��ȡ����״̬ */
	rxstat  = enc28j60_readop(ENC28J60_READ_BUF_MEM, 0);
	rxstat |= enc28j60_readop(ENC28J60_READ_BUF_MEM, 0) << 8;
    
	/* ���Ƽ����ĳ���	*/  
    if (len > maxlen - 1)
	{
        len = maxlen - 1;
    }
    /* ���CRC�ͷ��Ŵ��� */
    /* ERXFCON.CRCEN��Ĭ�����á�ͨ�����ǲ���Ҫ��� */
    if ((rxstat & 0x80)==0)
	{
        len = 0;
	}
	else
	{
        /* �ӽ��ջ������и������ݰ� */
        enc28j60_readbuf( packet , len );
    }
    
    /* �ƶ����ջ����� ��ָ��*/
	enc28j60_write(ERXRDPTL, (next_pkt));
	enc28j60_write(ERXRDPTH, (next_pkt)>>8);
    
	/* ���ݰ��ݼ� */
	enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
    
    /* ���س��� */
	return len;
}


/**
  * @brief  ����˵��: ���enc28j60���ݰ�����
						ʹ��˵��: ������uIP����LwIP���ɶ�ʱѭ�����ø�����
	*         ��LwIPϵͳ���ɵ���ethernetif_input();
  * @param  None
  * @retval ���ݰ�����
  */
int enc28j60_packet_getcount( void )
{
    return (int)enc28j60_read(EPKTCNT);
}


/**
  * @brief  ����˵��: ���enc28j60���ݰ�����
	*					ʹ��˵��: ������LwIP��low_level_input�е��ã����len = ;
	*         ������ݰ����ȣ����ŷ���pbuf
	*					���÷���: len = enc28j60_packet_getlen();
  * @param  None
  * @retval ���ݰ�����
  */
int enc28j60_packet_getlen(void)
{
	/* ���ݰ��ܳ��� */
    int len = 0;
    int rxstat;
    
    /* ���ý��ջ�������ָ�� */
	enc28j60_write( ERDPTL, (next_pkt) );
	enc28j60_write( ERDPTH, (next_pkt)>>8 );
    
    /* �������ݰ��ṹʾ�� �����ֲ�43ҳ */
	/* ����һ������ָ�� */
	next_pkt  = enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 );
	next_pkt |= enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 )<<8;
    
	/* �����ĳ��� */
	len  = enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 );
	len |= enc28j60_readop( ENC28J60_READ_BUF_MEM, 0 )<<8;
    
    /* ɾ��CRC���� */
    len -= 4; 	
    
	/* ��ȡ����״̬ */
	rxstat  = enc28j60_readop(ENC28J60_READ_BUF_MEM, 0);
	rxstat |= enc28j60_readop(ENC28J60_READ_BUF_MEM, 0) << 8;
    
    /* ע��ȡ����CRCУ���鲿�� */
    /* ���ؽ������ݰ����� */
    return len;
}

/**
  * @brief  ����˵��: ������ȡ���ջ��������̣��ƶ����ջ�����ָ��
	* 				ʹ��˵��: ������LwIP��low_level_input�е���
	*         					acknowledge that packet has been read(); ��
	*         					drop packet();
	* 				���÷���: enc28j60_finish_receive()
  * @param  None
  * @retval None
  */
void enc28j60_finish_receive( void )
{
    /* �ƶ����ջ����� ��ָ��*/
	enc28j60_write(ERXRDPTL, (next_pkt));
	enc28j60_write(ERXRDPTH, (next_pkt)>>8);
    
	/* ���ݰ��ݼ� */
	enc28j60_writeop(ENC28J60_BIT_FIELD_SET, ECON2, ECON2_PKTDEC);
}

/********************************* END OF FILE *********************************/

