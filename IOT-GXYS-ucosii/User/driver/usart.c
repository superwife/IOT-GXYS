/**
  ********************************  STM32F0xx  *********************************
  * @文件名     ： usart.c
  * @作者       ： 
  * @库版本     ： V1.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2019年05月06日
  * @摘要       ： USART源文件
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ----------------------------------------------------------------------------*/
/* 包含的头文件 --------------------------------------------------------------*/
#include "usart.h"
#include "stm32f0xx_usart.h"

#pragma import(__use_no_semihosting)             
//??????????                 
struct __FILE 
{ 
	int handle; 
}; 
typedef struct __FILE FILE;

FILE __stdout;       
//??_sys_exit()??????????    
int _sys_exit(int x) 
{ 
	x = x; 
} 

int fputc(int ch, FILE *f)
{      
	while((USART1->ISR&0X40)==0);  
    USART1->TDR = (uint8_t) ch;      
	return ch;
}


/************************************************
函数名称 ： USART_GPIO_Configuration
功    能 ： USART所使用管脚输出输入定义
参    数 ： 无
返 回 值 ： 无
*************************************************/
void USART_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

  /* 定义USART TX_RX 引脚为复用输出 */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;            //引脚
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                       //复用模式
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                  //高速
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                     //推完输出
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                       //上拉
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}

/************************************************
函数名称 ： USART_Configuration
功    能 ： 配置USART
参    数 ： 无
返 回 值 ： 无
*************************************************/
void USART_Configuration(void)
{
  USART_InitTypeDef USART_InitStructure;
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );

  /******************************************************************
  USART参数初始化:  波特率     传输位数   停止位数  校验位数
                    115200         8         1        0(NO)
  *******************************************************************/
  USART_InitStructure.USART_BaudRate = 115200;                       //设定传输速率
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //设定传输数据位数
  USART_InitStructure.USART_StopBits = USART_StopBits_1;             //设定停止位个数
  USART_InitStructure.USART_Parity = USART_Parity_No ;               //不用校验位
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不用流量控制
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //使用接收和发送功能
  USART_Init(USART1, &USART_InitStructure);                          //初始化USART1

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                     //使能USART1接收中断

  USART_Cmd(USART1, ENABLE);                                         //使能USART1
}

/************************************************
函数名称 ： USART_Initializes
功    能 ： 串口初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void USART_Initializes(void)
{
  USART_GPIO_Configuration();
  USART_Configuration();
}

/************************************************
函数名称 ： USART1_SendByte
功    能 ： 串口1发送一字节数据
参    数 ： Data --- 数据
返 回 值 ： 无
*************************************************/
void USART1_SendByte(uint8_t Data)
{
  while((USART1->ISR & USART_FLAG_TXE) == RESET);
  USART1->TDR = (Data & (uint16_t)0x01FF);
}

/************************************************
函数名称 ： USART1_SendNByte
功    能 ： 串口1发送N个字符
参    数 ： pData ----- 字符串
            Length --- 长度
返 回 值 ： 无
*************************************************/
void USART1_SendNByte(uint8_t *pData, uint16_t Length)
{
  while(Length--)
  {
    USART1_SendByte(*pData);
    pData++;
  }
}

/************************************************
函数名称 ： USART1_Printf
功    能 ： 串口1打印输出
参    数 ： String --- 字符串
返 回 值 ： 无
*************************************************/
void USART1_Printf(uint8_t *String)
{
  while((*String) != '\0')
  {
    USART1_SendByte(*String);
    String++;
  }
  USART1_SendByte(0x0A);
}


