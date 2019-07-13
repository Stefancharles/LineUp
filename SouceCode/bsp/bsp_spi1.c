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
#include "stm32f10x_spi.h"
#include "bsp_spi1.h"
//#include "Driver_SPI.h"

/* Public variables ----------------------------------------------------------*/
/* Public functions ----------------------------------------------------------*/
/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
/* Private variables ---------------------------------------------------------*/
/* Private function prototypes -----------------------------------------------*/
/* Private functions ---------------------------------------------------------*/


/**
  * @brief  ����spi
  * @param  None
  * @retval None
  */
void bsp_spi_init(void)
{
  /* GPIO�ṹ�� */
  GPIO_InitTypeDef  GPIO_InitStructure;
  /* SPI�ṹ�� */
  SPI_InitTypeDef   SPI_InitStructure; 
   
  /* ʹ��APB2�����ʱ�� */
  /* ʹ��SPIʱ�ӣ�ʹ��GPIOAʱ�� */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 |\
                         RCC_APB2Periph_GPIOA ,ENABLE );
	
  /* SPI1 SCK@GPIOA.5 SPI1 MOSI@GPIOA.7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  |  GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  /* ����������� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  
  /* SPI1 MISO@GPIOA.6 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  /* �������� */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  

  /* ˫��˫��ȫ˫�� */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	/* ����ģʽ */
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
  /* 8λ֡�ṹ */
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
  /* ʱ�ӿ���ʱΪ�� */
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;        
  /* ��1�������ز������� */
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;      
  /* MSS �˿�������� */
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;         
  /* SPIʱ�� 72Mhz / 8 = 9M */ 
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; 
  /* ���ݴ����λ��ǰ */
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
  
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  /* ��ʼ��SPI1 */
  SPI_Init(SPI1, &SPI_InitStructure);
  
	/* ��ʹ��SPI�ڵ�SS������� GPIOA.4 */
//	SPI_SSOutputCmd(SPI1,ENABLE);
  /* ʹ��SPI1 */
  SPI_Cmd(SPI1, ENABLE); 
}
/**
  * @brief  Spi��дһ���ֽڣ�����Spi����дһ�ֽڵ�ͬʱҲ����һ�ֽ�
  * @param  None
  * @retval None
  */
uint8_t bsp_spi_read_write_byte(uint8_t dat)
{
  /* �ȴ����ͻ���Ĵ���Ϊ�� */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  /* �������� */
  SPI_I2S_SendData(SPI1, dat);		
  
  /* �ȴ����ջ���Ĵ���Ϊ�ǿ� */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  
  return SPI_I2S_ReceiveData(SPI1);
}

/********************************* END OF FILE *********************************/


