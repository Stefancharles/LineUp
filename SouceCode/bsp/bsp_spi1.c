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
  * @brief  配置spi
  * @param  None
  * @retval None
  */
void bsp_spi_init(void)
{
  /* GPIO结构体 */
  GPIO_InitTypeDef  GPIO_InitStructure;
  /* SPI结构体 */
  SPI_InitTypeDef   SPI_InitStructure; 
   
  /* 使能APB2上相关时钟 */
  /* 使能SPI时钟，使能GPIOA时钟 */
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_SPI1 |\
                         RCC_APB2Periph_GPIOA ,ENABLE );
	
  /* SPI1 SCK@GPIOA.5 SPI1 MOSI@GPIOA.7 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_5  |  GPIO_Pin_7; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  /* 复用推挽输出 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  
  /* SPI1 MISO@GPIOA.6 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_6; 
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz; 
  /* 浮动输入 */
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOA, &GPIO_InitStructure); 
  

  /* 双线双向全双工 */
  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; 
	/* 主机模式 */
  SPI_InitStructure.SPI_Mode = SPI_Mode_Master; 
  /* 8位帧结构 */
  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b; 
  /* 时钟空闲时为低 */
  SPI_InitStructure.SPI_CPOL = SPI_CPOL_Low;        
  /* 第1个上升沿捕获数据 */
  SPI_InitStructure.SPI_CPHA = SPI_CPHA_1Edge;      
  /* MSS 端口软件控制 */
  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;         
  /* SPI时钟 72Mhz / 8 = 9M */ 
  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_8; 
  /* 数据传输高位在前 */
  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB; 
  
  SPI_InitStructure.SPI_CRCPolynomial = 7;
  /* 初始化SPI1 */
  SPI_Init(SPI1, &SPI_InitStructure);
  
	/* 把使能SPI口的SS输出功能 GPIOA.4 */
//	SPI_SSOutputCmd(SPI1,ENABLE);
  /* 使能SPI1 */
  SPI_Cmd(SPI1, ENABLE); 
}
/**
  * @brief  Spi读写一个字节，在向Spi总线写一字节的同时也读回一字节
  * @param  None
  * @retval None
  */
uint8_t bsp_spi_read_write_byte(uint8_t dat)
{
  /* 等待发送缓冲寄存器为空 */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_TXE) == RESET);
  /* 发送数据 */
  SPI_I2S_SendData(SPI1, dat);		
  
  /* 等待接收缓冲寄存器为非空 */
  while (SPI_I2S_GetFlagStatus(SPI1, SPI_I2S_FLAG_RXNE) == RESET);
  
  return SPI_I2S_ReceiveData(SPI1);
}

/********************************* END OF FILE *********************************/


