#ifndef _USART_4G_H_
#define _USART_4G_H_
#include "stdint.h"
#include "stm32f0xx_usart.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx_misc.h"

#define USART_4G_STATUS_STAT    GPIO_ReadInputDataBit(GPIOA, GPIO_Pin_1)
#define DMA_DATA_LEN	64

#define POWER_4G_PORT 			GPIOA
#define POWER_4G_PIN			GPIO_Pin_0
#define POWER_4G_CLK            RCC_AHBPeriph_GPIOA 

#define POWER_4G_ON				GPIO_SetBits(POWER_4G_PORT, POWER_4G_PIN)	
#define POWER_4G_OFF			GPIO_ResetBits(POWER_4G_PORT, POWER_4G_PIN)

/*------- public para ---------*/
struct sim7600_flag_st{
	uint8_t config_sim7600_flag;
	uint8_t sim7600_work_flag;
	uint8_t sim7600_status_flag;
	uint8_t sim7600_register_flag;
};

/*------- public func ---------*/
void usart_4g_init(void);
void usart_4g_gpio_init(void);
void usart_4g_config(uint32_t BaudRate);
void Usart2_SendByte(uint8_t Data);
void Usart2_SendNByte(uint8_t *pData, uint16_t Length);
void usart_4g_send_data(uint8_t *data,uint32_t len);
void sim7600ce_config(void);
void usart_dma_init(void);
void NVIC_Configuration(void);
void sim7600_flag_init(void);

/*------- AT 4G CMD ---------*/
void AT_TEST(void);
void AT_CSQ(void);
void AT_CGDCONT(void);
void AT_CIPMODE(void);
void AT_NETCLOSE(void);
void AT_NETOPEN(void);
void AT_CIPRXGET(void);
void AT_CIPOPEN(void);
void AT_CHECK_CIPOPEN(void);
void AT_ATE(void);
void AT_ATO(void);


#endif
