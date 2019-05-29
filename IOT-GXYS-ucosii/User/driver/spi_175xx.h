/**
  ********************************  STM32F0xx  *********************************
  * @文件名     ： spi_175xx.h
  * @作者       ： 
  * @库版本     ： V1.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2019年05月04日
  * @摘要       ： spi_175xx头文件
  ******************************************************************************/

/* 定义防止递归包含 ----------------------------------------------------------*/
#ifndef _SPI_175XX_H
#define _SPI_175XX_H

/* 包含的头文件 --------------------------------------------------------------*/
#include "stm32f0xx.h"

//主芯片SPI GPIO定义
#define SPI1_NFC_IQR_PIN                    GPIO_Pin_15 
#define SPI1_NFC_IQR_PORT                   GPIOB
#define SPI1_NFC_IQR_CLK                    RCC_AHBPeriph_GPIOB 

#define SPI1_NFC_RST_PIN                    GPIO_Pin_6
#define SPI1_NFC_RST_PORT                   GPIOB
#define SPI1_NFC_RST_CLK                    RCC_AHBPeriph_GPIOB  

#define SPI1_NFC_MOSI_PIN                   GPIO_Pin_5  
#define SPI1_NFC_MOSI_PORT                  GPIOB
#define SPI1_NFC_MOSI_CLK                   RCC_AHBPeriph_GPIOB 
  
#define SPI1_NFC_MISO_PIN                   GPIO_Pin_4
#define SPI1_NFC_MISO_PORT                  GPIOB
#define SPI1_NFC_MISO_CLK                   RCC_AHBPeriph_GPIOB  

#define SPI1_NFC_SCK_PIN                    GPIO_Pin_3  
#define SPI1_NFC_SCK_PORT                   GPIOB
#define SPI1_NFC_SCK_CLK                    RCC_AHBPeriph_GPIOB 

#define SPI1_NFC_NSS_PIN                    GPIO_Pin_15 
#define SPI1_NFC_NSS_PORT                   GPIOA
#define SPI1_NFC_NSS_CLK                    RCC_AHBPeriph_GPIOA 


//控制模式命令
#define RST_H                            GPIO_SetBits(SPI1_NFC_RST_PORT, SPI1_NFC_RST_PIN)
#define RST_L                            GPIO_ResetBits(SPI1_NFC_RST_PORT, SPI1_NFC_RST_PIN)

#define MOSI_H                           GPIO_SetBits(SPI1_NFC_MOSI_PORT, SPI1_NFC_MOSI_PIN)
#define MOSI_L                           GPIO_ResetBits(SPI1_NFC_MOSI_PORT, SPI1_NFC_MOSI_PIN)

#define SCK_H                            GPIO_SetBits(SPI1_NFC_SCK_PORT, SPI1_NFC_SCK_PIN)
#define SCK_L                            GPIO_ResetBits(SPI1_NFC_SCK_PORT, SPI1_NFC_SCK_PIN)

#define NSS_H                            GPIO_SetBits(SPI1_NFC_NSS_PORT, SPI1_NFC_NSS_PIN)
#define NSS_L                            GPIO_ResetBits(SPI1_NFC_NSS_PORT, SPI1_NFC_NSS_PIN)

#define READ_MISO                        GPIO_ReadInputDataBit(SPI1_NFC_MISO_PORT, SPI1_NFC_MISO_PIN)



void spi_nfc_gpio_init(void);
void spi_nfc_mode_config(void);
void spi_nfc_init(void);
void InitializeSystem(void);
void spi_nfc_read_func(void);



#endif
