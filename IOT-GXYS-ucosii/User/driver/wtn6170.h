/* 定义防止递归包含 ----------------------------------------------------------*/
#ifndef _WTN6170_H
#define _WTN6170_H

#include "stm32f0xx.h"
#include "ucos_ii.h"

#define WTN_BUSY_PIN            GPIO_Pin_1 
#define WTN_BUSY_PORT           GPIOB
#define WTN_BUSY_CLK            RCC_AHBPeriph_GPIOB 

#define WTN_CLK_PIN             GPIO_Pin_7  
#define WTN_CLK_PORT            GPIOA
#define WTN_CLK_CLK             RCC_AHBPeriph_GPIOA

#define WTN_DATA_PIN            GPIO_Pin_0
#define WTN_DATA_PORT           GPIOB
#define WTN_DATA_CLK            RCC_AHBPeriph_GPIOB   

//控制模式命令
#define WTN_CLK_H               GPIO_SetBits(WTN_CLK_PORT, WTN_CLK_PIN)
#define WTN_CLK_L               GPIO_ResetBits(WTN_CLKT_PORT, WTN_CLK_PIN)

#define WTN_DATA_H              GPIO_SetBits(WTN_DATA_PORT, WTN_DATA_PIN)
#define WTN_DATA_L              GPIO_ResetBits(WTN_DATA_PORT, WTN_DATA_PIN)

#define READ_WTN_BUSY           GPIO_ReadInputDataBit(WTN_BUSY_PORT, WTN_BUSY_PIN)

//语音命令类型
#define PLEASE_PICK_UP			0x01
#define BOOK_SUCCESS			0x02



void wtn6170_gpio_init(void);
void wtn_send_data(uint8_t data);

#endif
