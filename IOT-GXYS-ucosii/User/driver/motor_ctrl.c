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
#include "stm32f0xx_exti.h"
#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_misc.h"

struct IR_DATA_ST ir_dat;

/************************************************
�������� �� signal_motor_gpio_init
��    �� �� ������gpio��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void signal_motor_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
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
		

	//��ʼ���������GPIO PA4/PB5 ȷ���Ƿ���IO,����Ϊ�ⲿ�ж�
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;                 //�ⲿ�ж�
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* EXTI����*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;                         //�ⲿ�ж���
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;                //�ж���������ģʽ��
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;            //�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);      //EXIT�ж�
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;                         //�ⲿ�ж���
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;                //�ж���������ģʽ��
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;            //�½��ش���
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource5);      //EXIT�ж�
	
	ir_dat.ir_tx1 = 0;
	ir_dat.ir_tx2 = 0;
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
	if(CTRL_CMD==LOCK_ON)
	{
		make_buzzer_sound();
	}

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
/************************************************
�������� �� EXTI4_15_IRQHandler
��    �� �� ��������
��    �� �� 
�� �� ֵ �� ��
*************************************************/
void EXTI4_15_IRQHandler(void)
{
	OSIntEnter();
	if((EXTI->PR & EXTI_Line4) != (uint32_t)RESET)
	{
		EXTI->PR = EXTI_Line4;                       //�����־λ
		printf("recv EXTI_Line4\n");
		ir_dat.ir_tx1 = 1;
		EXTI->PR = 0xFFF0;                           //���EXTI4_15���б�־λ
	}
	else if((EXTI->PR & EXTI_Line5) != (uint32_t)RESET)
	{
		EXTI->PR = EXTI_Line5;                       //�����־λ
		printf("recv EXTI_Line5\n");
		ir_dat.ir_tx2 = 1;
		EXTI->PR = 0xFFF0;                           //���EXTI4_15���б�־λ
	}
	else
	{
		EXTI->PR = 0xFFF0;                           //���EXTI4_15���б�־λ
	}
	OSIntExit();
}


