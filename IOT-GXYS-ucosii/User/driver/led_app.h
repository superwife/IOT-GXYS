#ifndef _LED_APP_H_
#define _LED_APP_H_
#include <stdint.h>
/* 私有类型定义---------------------------------------------------------------*/
/* 私有定义 ------------------------------------------------------------------*/
/* 私有宏定义 ----------------------------------------------------------------*/
#define SYS_LED         GPIO_Pin_12
#define ALM_LED         GPIO_Pin_11

#define LED_PORT        GPIOA
#define LED_GPIO_CLK    RCC_AHBPeriph_GPIOA

void LED_Configuration(void);
void LED_On(uint16_t LEDx);
void LED_Off(uint16_t LEDx);
void led_ctrl_func(uint16_t LED,uint8_t cmd);
void delay_us(uint16_t ucont);
void delay_ms(uint16_t ucont);

#endif
