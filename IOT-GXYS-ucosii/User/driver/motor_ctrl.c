/***************************************************************************//**
  * @file    
  * @author  
  * @version V1.0.0
  * @date    2019年5月9号
  * @brief   主程序
*******************************************************************************/
#include "stm32f0xx.h"
#include "motor_ctrl.h"
#include "led_app.h"

/************************************************
函数名称 ： signal_motro_gpio_init
功    能 ： 单项电机gpio初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void signal_motro_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
    
    //使能单项电机所在GPIO的时钟
    RCC_AHBPeriphClockCmd(SINGLE_MOTOR_CLK, ENABLE);

    //初始化MOTOR的GPIO PB8 控制开关
    GPIO_InitStructure.GPIO_Pin = SINGLE_MOTOR_EN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	
    GPIO_Init(SINGLE_MOTOR_PORT, &GPIO_InitStructure);
	
	//初始化MOTOR的GPIO PB9 确认是否动作IO
    GPIO_InitStructure.GPIO_Pin = SINGLE_MOTOR_CHECK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
		
    GPIO_Init(SINGLE_MOTOR_PORT, &GPIO_InitStructure);
}
/************************************************
函数名称 ： single_motor_test
功    能 ： 单项电机开关测试
参    数 ： 无
返 回 值 ： 无
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
函数名称 ： unlocking
功    能 ： 单项电机开
参    数 ： 无
返 回 值 ： 无
*************************************************/
void unlocking(void)
{
	SINGLE_MOTOR_RUN_CMD;
	delay_ms(1000);
	SINGLE_MOTOR_STOP_CMD;
}



