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
    
    //使能电机所在GPIO的时钟
    RCC_AHBPeriphClockCmd(BOOK_DATA_CLK|RETURN_DATA_CLK, ENABLE);

    //初始化借伞MOTOR的GPIO PB8 控制开关
    GPIO_InitStructure.GPIO_Pin = BOOK_MOTOR_EN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(BOOK_DATA_PORT, &GPIO_InitStructure);
	
	//初始化借伞行程开关GPIO PB9 确认是否动作IO
    GPIO_InitStructure.GPIO_Pin = BOOK_TRAVEL_SW_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
    GPIO_Init(BOOK_DATA_PORT, &GPIO_InitStructure);
	
	//初始化还伞MOTOR的GPIO PA4 控制开关
    GPIO_InitStructure.GPIO_Pin = RETURN_MOTOR_EN_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(RETURN_DATA_PORT, &GPIO_InitStructure);
	
	//初始化还伞的行程开关GPIO PA5 确认是否动作IO
    GPIO_InitStructure.GPIO_Pin = RETURN_TRAVEL_SW_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
    GPIO_Init(RETURN_DATA_PORT, &GPIO_InitStructure);
}
/************************************************
函数名称 ： single_motor_test
功    能 ： 单项电机开关测试
参    数 ： 无
返 回 值 ： 无
*************************************************/
void single_motor_test(void)
{
	BOOK_MOTOR_RUN;
	if(BOOK_TRAVEL_SW_CHECK==1)
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
函数名称 ： unlocking_ctrl
功    能 ： 开锁控制
参    数 ： LOCK_TYPE 		 --  开锁类型
			LOCK_ON_OFF_TYPE --  控制方式
返 回 值 ： 无
*************************************************/
void unlocking_ctrl(LOCK_TYPE etpye,LOCK_ON_OFF_TYPE CTRL_CMD)
{
	switch(etpye)
	{
		case BOOK_LOCK:										//借伞锁
			if(CTRL_CMD==LOCK_ON)
			{
				BOOK_MOTOR_RUN;
			}
			else
			{
				BOOK_MOTOR_STOP;
			}
			break;
		case RETURN_LOCK:									//还伞锁
			if(CTRL_CMD==LOCK_ON)
			{
				RETURN_MOTOR_RUN;
			}
			else
			{
				RETURN_MOTOR_STOP;
			}
			break;
		case ADD_LOCK:										//添伞锁
			
			break;
		default:
			break;
	}
}



