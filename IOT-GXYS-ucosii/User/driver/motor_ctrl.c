/***************************************************************************//**
  * @file    
  * @author  
  * @version V1.0.0
  * @date    2019��5��9��
  * @brief   ������
*******************************************************************************/
#include "stm32f0xx.h"
#include "motor_ctrl.h"
#include "led_app.h"

/************************************************
�������� �� signal_motro_gpio_init
��    �� �� ������gpio��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void signal_motro_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
    //ʹ�ܵ���������GPIO��ʱ��
    RCC_AHBPeriphClockCmd(SINGLE_MOTOR_CLK, ENABLE);

    //��ʼ��MOTOR��GPIO PB8 ���ƿ���
    GPIO_InitStructure.GPIO_Pin = SINGLE_MOTOR_EN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
    GPIO_Init(SINGLE_MOTOR_PORT, &GPIO_InitStructure);
	
	//��ʼ��MOTOR��GPIO PB9 ȷ���Ƿ���IO
    GPIO_InitStructure.GPIO_Pin = SINGLE_MOTOR_CHECK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		
    GPIO_Init(SINGLE_MOTOR_PORT, &GPIO_InitStructure);
}
/************************************************
�������� �� single_motor_test
��    �� �� ���������ز���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void single_motor_test(void)
{
	SINGLE_MOTOR_RUN_CMD;
	if(SINGLE_MOTOR_RUN_CHECK==1)
	{
		led_ctrl_func(ALM_LED,1);
	}
	else
	{
		led_ctrl_func(ALM_LED,0);
	}
	delay_ms(2000);
	SINGLE_MOTOR_STOP_CMD;
}
/************************************************
�������� �� unlocking
��    �� �� ��������
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void unlocking(void)
{
	SINGLE_MOTOR_RUN_CMD;
	delay_ms(1000);
	SINGLE_MOTOR_STOP_CMD;
}



