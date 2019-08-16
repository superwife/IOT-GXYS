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
#include "stm32f0xx_misc.h"
#include "stm32f0xx_dma.h"
#include "stm32f0xx.h"
#include <string.h>
#include "ptu_app.h"

extern struct PTU_DATA_ST ptu_dat;

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
	if(ptu_dat.ptu_mode_flag==0)
	{
		while((USART1->ISR&0X40)==0);  
		USART1->TDR = (uint8_t) ch;      
		return ch;
	}
	else
	{
		return 0;
	}
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
	NVIC_InitTypeDef NVIC_InitStructure;
  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );

	/******************************************************************
	USART参数初始化:  波特率     传输位数   停止位数  校验位数
					115200         8         1        0(NO)
	*******************************************************************/
	USART_InitStructure.USART_BaudRate = 9600;                       //设定传输速率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //设定传输数据位数
	USART_InitStructure.USART_StopBits = USART_StopBits_1;             //设定停止位个数
	USART_InitStructure.USART_Parity = USART_Parity_No ;               //不用校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不用流量控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //使用接收和发送功能
	USART_Init(USART1, &USART_InitStructure);                          //初始化USART1

	/* 外设中断 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                  //IRQ通道:串口1
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;                    //优先级 :1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    //使能IRQ通道
	NVIC_Init(&NVIC_InitStructure);
	
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                     //使能USART1接收中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);					 //使能接收空闲中断

	USART_Cmd(USART1, ENABLE);                                         //使能USART1
}
/************************************************
函数名称 ： usart1_init
功    能 ： 串口引脚初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void usart1_config(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );	
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
	
	/******************************************************************
	USART参数初始化:  波特率     传输位数   停止位数  校验位数
	    115200         8           1          0        (NO)
	*******************************************************************/
	USART_InitStructure.USART_BaudRate = BaudRate;                     	//设定传输速率
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;        	//设定传输数据位数
	USART_InitStructure.USART_StopBits = USART_StopBits_1;             	//设定停止位个数
	USART_InitStructure.USART_Parity = USART_Parity_No ;               	//不用校验位
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//不用流量控制
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    	//使用接收和发送功能
	USART_Init(USART1, &USART_InitStructure);                          	//初始化USART2                                      
		
	/* 外设中断 */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                  //IRQ通道:串口1
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;                    //优先级 :1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    //使能IRQ通道
	NVIC_Init(&NVIC_InitStructure);	
	
	//配置串口，禁止发送中断，开启空闲中断
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);						//开启空闲接收中断
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);					    //使能接收
	USART_ClearFlag(USART1, USART_FLAG_IDLE);							//清除空闲中断标志 USART2->ICR |= 1<<4;
	USART_Cmd(USART1, ENABLE);	
	
	DMA_DeInit(DMA1_Channel3);                                         //复位DMA1_Channel3
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->RDR));   //外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ptu_dat.rx_buf; 		//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                 //传输方向:外设 -> 内存
	DMA_InitStructure.DMA_BufferSize = USART1_DMA_LEN;                     //传输长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //外设递增:NO
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //内存递增:YES
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //数据宽度:Byte
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                      //循环模式:NO
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            //优先级:高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                       //内存-内存:NO	
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	DMA_ClearFlag(DMA1_FLAG_TC3);
	
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TE, ENABLE);
	DMA_Cmd(DMA1_Channel3, ENABLE);                                    //使能DMA(开始传输)
}

/************************************************
函数名称 ： USART1_IRQHandler
功    能 ： 配置USART
参    数 ： 无
返 回 值 ： 无
*************************************************/
void USART1_IRQHandler(void)
{
//	static uint8_t recv_cnt = 0;
//	static uint8_t recv_buf[64]={0};
//  
//	if(USART_GetITStatus(USART1, USART_IT_RXNE) != RESET) 
//	{
//		USART_ClearFlag(USART1,USART_IT_RXNE);
//		recv_buf[recv_cnt++] = USART_ReceiveData(USART1); 
//		
//		//recv_buf[recv_cnt++] = (uint8_t)(USART1->RDR);      
//		if(recv_cnt==64)
//		{
//			recv_cnt = 0;
//		}
//	}
//	else if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)
//	{
//		USART1->RDR;                                        
//		USART1->TDR;                                        
//		ptu_data_deal(recv_buf,recv_cnt);         
//		recv_cnt = 0;
//		memset(recv_buf,0,sizeof(recv_buf));  
//		USART_ClearFlag(USART1, USART_FLAG_IDLE);	
//	}
	uint8_t i=0;
	if(USART_GetITStatus(USART1, USART_IT_IDLE) != RESET)  
	{
		uint32_t dma_len = 0,i=0; 			
		USART_ClearFlag(USART1, USART_FLAG_IDLE);							//清除空闲中断标志 USART2->ICR |= 1<<4;
		/* close DMA  */
		DMA_Cmd(DMA1_Channel3,DISABLE); 
		/* dma_len=dma_buff_size-idle_size*/
		dma_len = (uint16_t)(USART1_DMA_LEN - DMA_GetCurrDataCounter(DMA1_Channel3)); 	
//		for(i=0;i<dma_len;i++)
//		{
//			printf("%x ",DMA_Rx_Buf[i]);
//		}
//		printf("\r\n");	
		ptu_data_deal(ptu_dat.rx_buf,dma_len); 
		memset(ptu_dat.rx_buf,0,sizeof(ptu_dat.rx_buf));
		/* set dma send_len ,the dma_receive buff will be clear automatically*/
		DMA_SetCurrDataCounter(DMA1_Channel3,USART1_DMA_LEN);  						
		DMA_Cmd(DMA1_Channel3,ENABLE);
	} 
}


/************************************************
函数名称 ： USART_Initializes
功    能 ： 串口初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void USART_Initializes(void)
{
//	USART_GPIO_Configuration();
//	USART_Configuration();
	usart1_config(115200);
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

