/***************************************************************************//**
  * @file    sec_app.c
  * @author  
  * @version V1.0.0
  * @date    2019��5��6��
  * @brief   ������
*******************************************************************************/
#include "stm32f0xx.h"
#include "ucos_ii.h"
#include "led_app.h"

/************************************************
�������� �� led_ctrl_func
��    �� �� led���ƺ���
��    �� �� �����͡���������
�� �� ֵ �� ��
*************************************************/
void led_ctrl_func(uint16_t LED,uint8_t cmd)
{
	if(cmd)
	{
		LED_On(LED);
	}
	else
	{
		LED_Off(LED);
	}
}
/************************************************
�������� �� LED_Configuration
��    �� �� led��ʼ��
��    �� �� 
�� �� ֵ �� ��
*************************************************/
void LED_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    //ʹ��LED����GPIO��ʱ��
    RCC_AHBPeriphClockCmd(LED_GPIO_CLK, ENABLE);

    //��ʼ��LED��GPIO PA11\PA12
    GPIO_InitStructure.GPIO_Pin = SYS_LED | ALM_LED;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LED_PORT, &GPIO_InitStructure);
    
    GPIO_ResetBits(LED_PORT,SYS_LED | ALM_LED);  //Ϩ��LED3-4
}

/************************************************
�������� �� LED_On
��    �� �� led��
��    �� �� 
�� �� ֵ �� ��
*************************************************/
void LED_On(uint16_t LEDx)
{
	GPIO_ResetBits (LED_PORT, LEDx);
}

/************************************************
�������� �� LED_On
��    �� �� led��
��    �� �� 
�� �� ֵ �� ��
*************************************************/
void LED_Off(uint16_t LEDx)
{
	GPIO_SetBits (LED_PORT, LEDx);
}
/************************************************
�������� �� delay_us
��    �� �� us��ʱ
��    �� �� 
�� �� ֵ �� ��
*************************************************/
void delay_us(uint16_t ucont)
{
	int i,j;
    for (i=0; i<ucont; i++)
		for (j=0; j<8; j++)
			;
}
/************************************************
�������� �� delay_us
��    �� �� ms��ʱ
��    �� �� 
�� �� ֵ �� ��
*************************************************/
void delay_ms(uint16_t ucont)
{
	int i,j;
    for (i=0; i<ucont; i++)
		for (j=0; j<8000; j++)
			;
}


