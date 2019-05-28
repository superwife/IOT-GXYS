/***************************************************************************//**
  * @file    wtn6170.c
  * @author  
  * @version V1.0.0
  * @date    2019��5��8��
  * @brief   ������
*******************************************************************************/
#include "wtn6170.h"
#include "led_app.h"

/************************************************
�������� �� wtn6170_gpio_init
��    �� �� ��ʼ������оƬ����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void wtn6170_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
    RCC_AHBPeriphClockCmd(WTN_BUSY_CLK|WTN_DATA_CLK, ENABLE);
	//PB1
	GPIO_InitStructure.GPIO_Pin = WTN_BUSY_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(WTN_BUSY_PORT, &GPIO_InitStructure);
	
	//PB0
	GPIO_InitStructure.GPIO_Pin = WTN_DATA_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(WTN_DATA_PORT, &GPIO_InitStructure);
	
	//PA7
	GPIO_InitStructure.GPIO_Pin = WTN_CLK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(WTN_CLK_PORT, &GPIO_InitStructure);
	
	WTN_DATA_L;
	WTN_DATA_H;
}
/************************************************
�������� �� wtn_send_data
��    �� �� ͨ��data����������оƬ��������
��    �� �� ��Ҫ���͵�����
�� �� ֵ �� ��
*************************************************/
void wtn_send_data(uint8_t data)
{
	uint8_t i = 0;
	uint8_t send_data = 0;
	
	WTN_DATA_H;
	WTN_DATA_L;
	delay_ms(5);
	
	send_data = data;
	for(i=0;i<8;i++)
	{
		if(send_data&0x01)
		{
			WTN_DATA_H;
			delay_us(600);
			WTN_DATA_L;
			delay_us(200);
		}	
		else
		{
			WTN_DATA_H;
			delay_us(200);
			WTN_DATA_L;
			delay_us(600);
		}
		send_data = send_data>>1;		
	}
	
	WTN_DATA_H;
}




