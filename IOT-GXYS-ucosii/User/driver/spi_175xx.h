/**
  ********************************  STM32F0xx  *********************************
  * @�ļ���     �� spi_175xx.h
  * @����       �� 
  * @��汾     �� V1.5.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2019��05��04��
  * @ժҪ       �� spi_175xxͷ�ļ�
  ******************************************************************************/

/* �����ֹ�ݹ���� ----------------------------------------------------------*/
#ifndef _SPI_175XX_H
#define _SPI_175XX_H

/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "stm32f0xx.h"

//��оƬSPI GPIO����
#define SPI1_NFC_IQR_PIN                    GPIO_Pin_15 
#define SPI1_NFC_IQR_PORT                   GPIOB
#define SPI1_NFC_IQR_CLK                    RCC_AHBPeriph_GPIOB 

//#define SPI1_NFC_RST_PIN                    GPIO_Pin_6
//#define SPI1_NFC_RST_PORT                   GPIOB
//#define SPI1_NFC_RST_CLK                    RCC_AHBPeriph_GPIOB  

#define SPI1_NFC_MOSI_PIN                   GPIO_Pin_5  
#define SPI1_NFC_MOSI_PORT                  GPIOB
#define SPI1_NFC_MOSI_CLK                   RCC_AHBPeriph_GPIOB 
  
#define SPI1_NFC_MISO_PIN                   GPIO_Pin_4
#define SPI1_NFC_MISO_PORT                  GPIOB
#define SPI1_NFC_MISO_CLK                   RCC_AHBPeriph_GPIOB  

#define SPI1_NFC_SCK_PIN                    GPIO_Pin_3  
#define SPI1_NFC_SCK_PORT                   GPIOB
#define SPI1_NFC_SCK_CLK                    RCC_AHBPeriph_GPIOB 

#define SPI1_NFC_NSS1_PIN                    GPIO_Pin_15 
#define SPI1_NFC_NSS1_PORT                   GPIOA
#define SPI1_NFC_NSS1_CLK                    RCC_AHBPeriph_GPIOA 

#define SPI1_NFC_NSS2_PIN                    GPIO_Pin_6
#define SPI1_NFC_NSS2_PORT                   GPIOB
#define SPI1_NFC_NSS2_CLK                    RCC_AHBPeriph_GPIOB 

//����ģʽ����
//#define RST_H                            GPIO_SetBits(SPI1_NFC_RST_PORT, SPI1_NFC_RST_PIN)
//#define RST_L                            GPIO_ResetBits(SPI1_NFC_RST_PORT, SPI1_NFC_RST_PIN)
#define MOSI_H                           GPIO_SetBits(SPI1_NFC_MOSI_PORT, SPI1_NFC_MOSI_PIN)
#define MOSI_L                           GPIO_ResetBits(SPI1_NFC_MOSI_PORT, SPI1_NFC_MOSI_PIN)
#define SCK_H                            GPIO_SetBits(SPI1_NFC_SCK_PORT, SPI1_NFC_SCK_PIN)
#define SCK_L                            GPIO_ResetBits(SPI1_NFC_SCK_PORT, SPI1_NFC_SCK_PIN)
#define NSS1_H                           GPIO_SetBits(SPI1_NFC_NSS1_PORT, SPI1_NFC_NSS1_PIN)
#define NSS1_L                           GPIO_ResetBits(SPI1_NFC_NSS1_PORT, SPI1_NFC_NSS1_PIN)
#define NSS2_H                           GPIO_SetBits(SPI1_NFC_NSS2_PORT, SPI1_NFC_NSS2_PIN)
#define NSS2_L                           GPIO_ResetBits(SPI1_NFC_NSS2_PORT, SPI1_NFC_NSS2_PIN)
#define READ_MISO                        GPIO_ReadInputDataBit(SPI1_NFC_MISO_PORT, SPI1_NFC_MISO_PIN)

//4G��Դ����
//#define POWER_4G_PORT 					 GPIOA
//#define POWER_4G_PIN					 GPIO_Pin_0
//#define POWER_4G_CLK            		 RCC_AHBPeriph_GPIOA 

//#define POWER_4G_ON						 GPIO_SetBits(POWER_4G_PORT, POWER_4G_PIN)	
//#define POWER_4G_OFF					 GPIO_ResetBits(POWER_4G_PORT, POWER_4G_PIN)


//ȷ��NFC��дģʽ
//#define NFC_WORK_MODE					 0
//#if NFC_WORK_MODE						 	
//	#define NSS_H                 	     NSS1_H	
//	#define NSS_L                        NSS1_L
//#else
//	#define NSS_H                 	     NSS2_H	
//	#define NSS_L                        NSS2_L
//#endif

void spi_nfc_gpio_init(void);
void spi_nfc_mode_config(void);
void spi_nfc_init(void);
void InitializeSystem(void);
void spi_nfc_read_func(void);



#endif
