/***************************************************************************//**
  * @file    main.c
  * @author  
  * @version V1.0.0
  * @date    
  * @brief   主程序
*******************************************************************************/

/* 包含的头文件---------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_iwdg.h"
#include "ucos_ii.h"
#include "led_app.h"

/* 私有变量 ------------------------------------------------------------------*/
/* 变量 ----------------------------------------------------------------------*/
static  OS_STK   App_Task_LED1_Stk[APP_TASK_LED1_STK_SIZE];
static  OS_STK   App_Task_LED2_Stk[APP_TASK_LED2_STK_SIZE];
static  OS_STK   App_Task_SEC_Stk[APP_TASK_SEC_STK_SIZE];
static  OS_STK   App_Task_4G_Stk[APP_TASK_4G_STK_SIZE];

/* 任务函数 ------------------------------------------------------------------*/
static  void  App_Task_LED1(void* p_arg);
static  void  App_Task_LED2(void* p_arg);
extern  void  App_Task_SEC(void* p_arg);
extern  void  App_Task_4G(void* p_arg);

/* 私有函数 ------------------------------------------------------------------*/
void Delay(__IO uint32_t nCount);
void iwdg_init(void);

/***************************************************************************//**
  * @brief  主函数，硬件初始化，实现LED1-LED4闪烁
  * @note   无
  * @param  无
  * @retval 无
*******************************************************************************/
int main(void)
{
    INT8U os_err;
    OSInit();
    OS_CPU_SysTickInit();
	iwdg_init();
	LED_Configuration ();
	USART_Initializes();
    
//    //创建LED1闪烁的任务
//    os_err = OSTaskCreate( App_Task_LED1,
//                          (void *) 0,
//                          (OS_STK *) &App_Task_LED1_Stk[APP_TASK_LED1_STK_SIZE - 1],
//                          (INT8U) APP_TASK_LED1_PRIO);

    //创建4g通信模块任务
//    os_err = OSTaskCreate( App_Task_LED2,
//                          (void*) 0,
//                          (OS_STK*) &App_Task_LED2_Stk[APP_TASK_LED2_STK_SIZE - 1],
//                          (INT8U ) APP_TASK_LED2_PRIO);
													
	//创建sec秒程序的任务
    os_err = OSTaskCreate( App_Task_SEC,
                          (void*) 0,
                          (OS_STK*) &App_Task_SEC_Stk[APP_TASK_SEC_STK_SIZE - 1],
                          (INT8U ) APP_TASK_SEC_PRIO);	

	//创建4G程序的任务
    os_err = OSTaskCreate( App_Task_4G,
                          (void*) 0,
                          (OS_STK*) &App_Task_4G_Stk[APP_TASK_4G_STK_SIZE - 1],
                          (INT8U ) APP_TASK_4G_PRIO);							
    
    os_err = os_err;//仅仅是清除这个变量未使用的编译警告
    
    //启动uSOS 操作系统
    OSStart ();
}
/*******************************************************************************
  * @函数名称	App_Task_LED1
  * @函数说明   LED任务1
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void App_Task_LED1(void* pdata)
{
    pdata = pdata;

    for (;;)
    {
        LED_On(ALM_LED);
        OSTimeDlyHMSM(0, 0, 0, 1000);
        LED_Off(ALM_LED);
        OSTimeDlyHMSM(0, 0, 0, 1000);
    }
}

/*******************************************************************************
  * @函数名称	App_Task_LED2
  * @函数说明   LED任务2
  * @输入参数   无
  * @输出参数   无
  * @返回参数   无
*******************************************************************************/
void App_Task_LED2(void* pdata)
{
    pdata = pdata;

    for (;;)
    {
        LED_On(ALM_LED);
        OSTimeDly(1000);
        LED_Off(ALM_LED);
        OSTimeDly(1000);
    }
}


/***************************************************************************//**
  * @brief  插入一段延时时间
  * @note   无
  * @param  nCount：指定延时的时间长度
  * @retval 无
*******************************************************************************/
void Delay(__IO uint32_t nCount)
{
    int i,j;
    //利用循环来延时一定的时间
    for (i=0; i<nCount; i++)
        for (j=0; j<5000; j++)
            ;
}

/************************************************
函数名称 ： iwdg_init
功    能 ： 看门狗初始化
参    数 ： prer ---- 预分频值，rlr --- 自动重装值
返 回 值 ： 无	12s
*************************************************/
void iwdg_init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);			//使能写操作
	IWDG_SetPrescaler(IWDG_Prescaler_256);					//加入预分频值
	IWDG_SetReload(3200);									//加入自动重装值
	IWDG_ReloadCounter();
	IWDG_Enable();											//使能看门狗		((4*2^prer)*rlr)/40
}
/************************************************
函数名称 ： iwdg_init
功    能 ： 看门狗初始化
参    数 ： prer ---- 预分频值，rlr --- 自动重装值
返 回 值 ： 无	10s
*************************************************/
void iwdg_feed(void)
{
	IWDG_ReloadCounter();
}

#ifdef  USE_FULL_ASSERT
/***************************************************************************//**
  * @brief  报告在检查参数发生错误时的源文件名和错误行数
  * @param  file: 指向错误文件的源文件名
  * @param  line: 错误的源代码所在行数
  * @retval 无
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
    /* 用户可以增加自己的代码用于报告错误的文件名和所在行数,
       例如：printf("错误参数值: 文件名 %s 在 %d行\r\n", file, line) */

    /* 死循环 */
    while (1)
    {
    }
}
#endif

