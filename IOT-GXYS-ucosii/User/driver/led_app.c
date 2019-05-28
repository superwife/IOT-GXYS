/***************************************************************************//**
  * @file    sec_app.c
  * @author  
  * @version V1.0.0
  * @date    2019年5月6号
  * @brief   主程序
*******************************************************************************/
#include "stm32f0xx.h"
#include "ucos_ii.h"
#include "led_app.h"

/************************************************
函数名称 ： led_ctrl_func
功    能 ： led控制函数
参    数 ： 灯类型、命令类型
返 回 值 ： 无
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
函数名称 ： LED_Configuration
功    能 ： led初始化
参    数 ： 
返 回 值 ： 无
*************************************************/
void LED_Configuration(void)
{
    GPIO_InitTypeDef GPIO_InitStructure;
    
    //使能LED所在GPIO的时钟
    RCC_AHBPeriphClockCmd(LED_GPIO_CLK, ENABLE);

    //初始化LED的GPIO PA11\PA12
    GPIO_InitStructure.GPIO_Pin = SYS_LED | ALM_LED;
    GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
    GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
    GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
    GPIO_Init(LED_PORT, &GPIO_InitStructure);
    
    GPIO_ResetBits(LED_PORT,SYS_LED | ALM_LED);  //熄灭LED3-4
}

/************************************************
函数名称 ： LED_On
功    能 ： led亮
参    数 ： 
返 回 值 ： 无
*************************************************/
void LED_On(uint16_t LEDx)
{
	GPIO_ResetBits (LED_PORT, LEDx);
}

/************************************************
函数名称 ： LED_On
功    能 ： led灭
参    数 ： 
返 回 值 ： 无
*************************************************/
void LED_Off(uint16_t LEDx)
{
	GPIO_SetBits (LED_PORT, LEDx);
}
/************************************************
函数名称 ： delay_us
功    能 ： us延时
参    数 ： 
返 回 值 ： 无
*************************************************/
void delay_us(uint16_t ucont)
{
	int i,j;
    for (i=0; i<ucont; i++)
		for (j=0; j<8; j++)
			;
}
/************************************************
函数名称 ： delay_us
功    能 ： ms延时
参    数 ： 
返 回 值 ： 无
*************************************************/
void delay_ms(uint16_t ucont)
{
	int i,j;
    for (i=0; i<ucont; i++)
		for (j=0; j<8000; j++)
			;
}


