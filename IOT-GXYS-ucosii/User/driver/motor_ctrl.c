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
    
    //ʹ�ܵ������GPIO��ʱ��
    RCC_AHBPeriphClockCmd(MOTOR_DATA_CLK|INFRA_RX_DATA_CLK, ENABLE);

    //��ʼ����ɡ����ɡ������MOTOR��GPIO PB7/PB8/PB9 ���ƿ���
    GPIO_InitStructure.GPIO_Pin = BOOK_MOTOR_PIN|RETURN_MOTOR_PIN|DOOR_MOTOR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(MOTOR_DATA_PORT, &GPIO_InitStructure);
	
	//��ʼ�����ſ���GPIO PB0 ȷ���Ƿ���IO
    GPIO_InitStructure.GPIO_Pin = DOOR_STAT_CHECK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
    GPIO_Init(MOTOR_DATA_PORT, &GPIO_InitStructure);
	
	//��ʼ���������GPIO PA4/PB5 ȷ���Ƿ���IO
    GPIO_InitStructure.GPIO_Pin = BOOK_INFRA_RX_PIN|RETURN_INFRA_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(INFRA_RX_POART, &GPIO_InitStructure);
}
/************************************************
�������� �� single_motor_test
��    �� �� ���������ز���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void single_motor_test(void)
{
	BOOK_MOTOR_RUN;
	if(BOOK_INFRA_RX_PIN==1)
	{
		led_ctrl_func(ALM_LED,1);
	}
	else
	{
		led_ctrl_func(ALM_LED,0);
	}
	delay_ms(2000);
	BOOK_MOTOR_STOP;
}
/************************************************
�������� �� unlocking_ctrl
��    �� �� ��������
��    �� �� LOCK_TYPE 		 --  ��������
			LOCK_ON_OFF_TYPE --  ���Ʒ�ʽ
�� �� ֵ �� ��
*************************************************/
void unlocking_ctrl(LOCK_TYPE etpye,LOCK_ON_OFF_TYPE CTRL_CMD)
{
	switch(etpye)
	{
		case BOOK_LOCK:										//��ɡ��
			if(CTRL_CMD==LOCK_ON)
			{
				BOOK_MOTOR_RUN;
			}
			else
			{
				BOOK_MOTOR_STOP;
			}
			break;
		case RETURN_LOCK:									//��ɡ��
			if(CTRL_CMD==LOCK_ON)
			{
				RETURN_MOTOR_RUN;
			}
			else
			{
				RETURN_MOTOR_STOP;
			}
			break;
		case ADD_LOCK:										//��ɡ��
			if(CTRL_CMD==LOCK_ON)
			{
				DOOR_MOTOR_RUN;
			}
			else
			{
				DOOR_MOTOR_STOP;
			}
			break;
		default:
			break;
	}
}



