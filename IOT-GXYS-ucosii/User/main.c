/***************************************************************************//**
  * @file    main.c
  * @author  
  * @version V1.0.0
  * @date    
  * @brief   ������
*******************************************************************************/

/* ������ͷ�ļ�---------------------------------------------------------------*/
#include "stm32f0xx.h"
#include "stm32f0xx_iwdg.h"
#include "ucos_ii.h"
#include "led_app.h"

/* ˽�б��� ------------------------------------------------------------------*/
/* ���� ----------------------------------------------------------------------*/
static  OS_STK   App_Task_LED1_Stk[APP_TASK_LED1_STK_SIZE];
static  OS_STK   App_Task_LED2_Stk[APP_TASK_LED2_STK_SIZE];
static  OS_STK   App_Task_SEC_Stk[APP_TASK_SEC_STK_SIZE];
static  OS_STK   App_Task_4G_Stk[APP_TASK_4G_STK_SIZE];

/* ������ ------------------------------------------------------------------*/
static  void  App_Task_LED1(void* p_arg);
static  void  App_Task_LED2(void* p_arg);
extern  void  App_Task_SEC(void* p_arg);
extern  void  App_Task_4G(void* p_arg);

/* ˽�к��� ------------------------------------------------------------------*/
void Delay(__IO uint32_t nCount);
void iwdg_init(void);

/***************************************************************************//**
  * @brief  ��������Ӳ����ʼ����ʵ��LED1-LED4��˸
  * @note   ��
  * @param  ��
  * @retval ��
*******************************************************************************/
int main(void)
{
    INT8U os_err;
    OSInit();
    OS_CPU_SysTickInit();
	iwdg_init();
	LED_Configuration ();
	USART_Initializes();
    
//    //����LED1��˸������
//    os_err = OSTaskCreate( App_Task_LED1,
//                          (void *) 0,
//                          (OS_STK *) &App_Task_LED1_Stk[APP_TASK_LED1_STK_SIZE - 1],
//                          (INT8U) APP_TASK_LED1_PRIO);

    //����4gͨ��ģ������
//    os_err = OSTaskCreate( App_Task_LED2,
//                          (void*) 0,
//                          (OS_STK*) &App_Task_LED2_Stk[APP_TASK_LED2_STK_SIZE - 1],
//                          (INT8U ) APP_TASK_LED2_PRIO);
													
	//����sec����������
    os_err = OSTaskCreate( App_Task_SEC,
                          (void*) 0,
                          (OS_STK*) &App_Task_SEC_Stk[APP_TASK_SEC_STK_SIZE - 1],
                          (INT8U ) APP_TASK_SEC_PRIO);	

	//����4G���������
    os_err = OSTaskCreate( App_Task_4G,
                          (void*) 0,
                          (OS_STK*) &App_Task_4G_Stk[APP_TASK_4G_STK_SIZE - 1],
                          (INT8U ) APP_TASK_4G_PRIO);							
    
    os_err = os_err;//����������������δʹ�õı��뾯��
    
    //����uSOS ����ϵͳ
    OSStart ();
}
/*******************************************************************************
  * @��������	App_Task_LED1
  * @����˵��   LED����1
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
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
  * @��������	App_Task_LED2
  * @����˵��   LED����2
  * @�������   ��
  * @�������   ��
  * @���ز���   ��
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
  * @brief  ����һ����ʱʱ��
  * @note   ��
  * @param  nCount��ָ����ʱ��ʱ�䳤��
  * @retval ��
*******************************************************************************/
void Delay(__IO uint32_t nCount)
{
    int i,j;
    //����ѭ������ʱһ����ʱ��
    for (i=0; i<nCount; i++)
        for (j=0; j<5000; j++)
            ;
}

/************************************************
�������� �� iwdg_init
��    �� �� ���Ź���ʼ��
��    �� �� prer ---- Ԥ��Ƶֵ��rlr --- �Զ���װֵ
�� �� ֵ �� ��	12s
*************************************************/
void iwdg_init(void)
{
	IWDG_WriteAccessCmd(IWDG_WriteAccess_Enable);			//ʹ��д����
	IWDG_SetPrescaler(IWDG_Prescaler_256);					//����Ԥ��Ƶֵ
	IWDG_SetReload(3200);									//�����Զ���װֵ
	IWDG_ReloadCounter();
	IWDG_Enable();											//ʹ�ܿ��Ź�		((4*2^prer)*rlr)/40
}
/************************************************
�������� �� iwdg_init
��    �� �� ���Ź���ʼ��
��    �� �� prer ---- Ԥ��Ƶֵ��rlr --- �Զ���װֵ
�� �� ֵ �� ��	10s
*************************************************/
void iwdg_feed(void)
{
	IWDG_ReloadCounter();
}

#ifdef  USE_FULL_ASSERT
/***************************************************************************//**
  * @brief  �����ڼ�������������ʱ��Դ�ļ����ʹ�������
  * @param  file: ָ������ļ���Դ�ļ���
  * @param  line: �����Դ������������
  * @retval ��
*******************************************************************************/
void assert_failed(uint8_t* file, uint32_t line)
{
    /* �û����������Լ��Ĵ������ڱ��������ļ�������������,
       ���磺printf("�������ֵ: �ļ��� %s �� %d��\r\n", file, line) */

    /* ��ѭ�� */
    while (1)
    {
    }
}
#endif

