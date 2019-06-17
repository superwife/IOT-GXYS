/* ∂®“Â∑¿÷πµ›πÈ∞¸∫¨ ----------------------------------------------------------*/
#ifndef _WTN6170_H
#define _WTN6170_H

#include "stm32f0xx.h"
#include "ucos_ii.h"

#define WTN_DATA_PIN             GPIO_Pin_1  
#define WTN_DATA_PORT            GPIOB
#define WTN_DATA_CLK             RCC_AHBPeriph_GPIOB

#define BUZZER_PIN				 GPIO_Pin_6  
#define BUZZER_PORT            	 GPIOA
#define BUZZER_CLK               RCC_AHBPeriph_GPIOA
  
#define WTN_DATA_H              GPIO_SetBits(WTN_DATA_PORT, WTN_DATA_PIN)
#define WTN_DATA_L              GPIO_ResetBits(WTN_DATA_PORT, WTN_DATA_PIN)

#define BUZZER_ON           	GPIO_SetBits(BUZZER_PORT, BUZZER_PIN)
#define BUZZER_OFF              GPIO_ResetBits(BUZZER_PORT, BUZZER_PIN)

//”Ô“Ù√¸¡Ó¿‡–Õ
#define PLEASE_PICK_UP			0x01
#define BOOK_SUCCESS			0x02

void make_buzzer_sound(void);
void wtn6170_gpio_init(void);
void wtn_send_data(uint8_t data);

#endif
