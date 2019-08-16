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
#include "stm32f0xx_exti.h"
#include "stm32f0xx_syscfg.h"
#include "stm32f0xx_misc.h"

struct IR_DATA_ST ir_dat;

/************************************************
函数名称 ： signal_motor_gpio_init
功    能 ： 单项电机gpio初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void signal_motor_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	EXTI_InitTypeDef  EXTI_InitStructure;
    NVIC_InitTypeDef NVIC_InitStructure;
	
    //使能电机所在GPIO的时钟
    RCC_AHBPeriphClockCmd(MOTOR_DATA_CLK|INFRA_RX_DATA_CLK, ENABLE);

    //初始化借伞、还伞、开门MOTOR的GPIO PB7/PB8/PB9 控制开关
    GPIO_InitStructure.GPIO_Pin = BOOK_MOTOR_PIN|RETURN_MOTOR_PIN|DOOR_MOTOR_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(MOTOR_DATA_PORT, &GPIO_InitStructure);
	
	//初始化开门开关GPIO PB0 确认是否动作IO
    GPIO_InitStructure.GPIO_Pin = DOOR_STAT_CHECK_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;	
    GPIO_Init(MOTOR_DATA_PORT, &GPIO_InitStructure);
	
	//初始化红外检测的GPIO PA4/PB5 确认是否动作IO
    GPIO_InitStructure.GPIO_Pin = BOOK_INFRA_RX_PIN|RETURN_INFRA_RX_PIN;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(INFRA_RX_POART, &GPIO_InitStructure);
		

	//初始化红外检测的GPIO PA4/PB5 确认是否动作IO,配置为外部中断
	NVIC_InitStructure.NVIC_IRQChannel = EXTI4_15_IRQn;                 //外部中断
	NVIC_InitStructure.NVIC_IRQChannelPriority = 0;
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
	NVIC_Init(&NVIC_InitStructure);
	
	/* EXTI配置*/
	EXTI_InitStructure.EXTI_Line = EXTI_Line4;                         //外部中断线
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;                //中断请求（请求模式）
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;            //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource4);      //EXIT中断
	
	EXTI_InitStructure.EXTI_Line = EXTI_Line5;                         //外部中断线
	EXTI_InitStructure.EXTI_Mode = EXTI_Mode_Interrupt;                //中断请求（请求模式）
	EXTI_InitStructure.EXTI_Trigger = EXTI_Trigger_Falling;            //下降沿触发
	EXTI_InitStructure.EXTI_LineCmd = ENABLE;
	EXTI_Init(&EXTI_InitStructure);
	SYSCFG_EXTILineConfig(EXTI_PortSourceGPIOA, EXTI_PinSource5);      //EXIT中断
	
	ir_dat.ir_tx1 = 0;
	ir_dat.ir_tx2 = 0;
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
函数名称 ： unlocking_ctrl
功    能 ： 开锁控制
参    数 ： LOCK_TYPE 		 --  开锁类型
			LOCK_ON_OFF_TYPE --  控制方式
返 回 值 ： 无
*************************************************/
void unlocking_ctrl(LOCK_TYPE etpye,LOCK_ON_OFF_TYPE CTRL_CMD)
{	
	if(CTRL_CMD==LOCK_ON)
	{
		make_buzzer_sound();
	}

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
函数名称 ： EXTI4_15_IRQHandler
功    能 ： 开锁控制
参    数 ： 
返 回 值 ： 无
*************************************************/
void EXTI4_15_IRQHandler(void)
{
	OSIntEnter();
	if((EXTI->PR & EXTI_Line4) != (uint32_t)RESET)
	{
		EXTI->PR = EXTI_Line4;                       //清除标志位
		printf("recv EXTI_Line4\n");
		ir_dat.ir_tx1 = 1;
		EXTI->PR = 0xFFF0;                           //清除EXTI4_15所有标志位
	}
	else if((EXTI->PR & EXTI_Line5) != (uint32_t)RESET)
	{
		EXTI->PR = EXTI_Line5;                       //清除标志位
		printf("recv EXTI_Line5\n");
		ir_dat.ir_tx2 = 1;
		EXTI->PR = 0xFFF0;                           //清除EXTI4_15所有标志位
	}
	else
	{
		EXTI->PR = 0xFFF0;                           //清除EXTI4_15所有标志位
	}
	OSIntExit();
}


