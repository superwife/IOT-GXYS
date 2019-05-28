/***************************************************************************//**
  * @file    usart_4g.c
  * @author  
  * @version V1.0.0
  * @date    2019年5月6号
  * @brief   主程序
*******************************************************************************/
#include "stm32f0xx.h"
#include <stdio.h>
#include <string.h>
#include "led_app.h"
#include "usart_4g.h"
#include "gprs_protocol_app.h"
/*----------------- public para-----------------------------*/
uint8_t  DMA_Tx_Buf[DMA_DATA_LEN];
uint8_t  DMA_Rx_Buf[DMA_DATA_LEN];

struct sim7600_flag_ST sim7600_flag;


/************************************************
函数名称 ： usart_4g_init
功    能 ： 4g初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void usart_4g_init(void)
{
	usart_4g_gpio_init();
	usart_4g_config(115200);
	sim7600_flaga_init();
}
/************************************************
函数名称 ： sim7600_flaga_init
功    能 ： sim7600状态初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600_flaga_init(void)
{
	sim7600_flag.config_sim7600_flag   = 0;			//4g模块配置标志
	sim7600_flag.sim7600_work_flag 	  = 0;			//4g模块工作标志
	sim7600_flag.sim7600_status_flag   = 0;			//4g模块status状态标志
	sim7600_flag.sim7600_register_flag = 0;			//4g模块注册状态
}
/************************************************
函数名称 ： usart_4g_gpio_init
功    能 ： 4g串口引脚初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void usart_4g_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	/* 定义4G-STATUS 引脚为输入 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;            				//引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                       	//输入模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                  	//高速
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/************************************************
函数名称 ： usart_4g_gpio_init
功    能 ： 4g串口引脚初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void usart_4g_config(uint32_t BaudRate)
{
	USART_InitTypeDef USART_InitStructure;
	GPIO_InitTypeDef GPIO_InitStructure;
	DMA_InitTypeDef DMA_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE );	
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

	GPIO_PinAFConfig(GPIOA, GPIO_PinSource2, GPIO_AF_1);
	GPIO_PinAFConfig(GPIOA, GPIO_PinSource3, GPIO_AF_1);

	/* 定义USART TX_RX 引脚为复用输出 */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;            	//引脚
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                       	//复用模式
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                  	//高速
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                     	//推完输出
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                       	//上拉
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
	USART_Init(USART2, &USART_InitStructure);                          	//初始化USART2                                      
		
	/* 外设中断 */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;                  //IRQ通道:串口1
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;                    //优先级 :1级
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    //使能IRQ通道
	NVIC_Init(&NVIC_InitStructure);	
	
	//配置串口，禁止发送中断，开启空闲中断
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);						//开启空闲接收中断
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);					    //使能接收
	//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);					//禁止接收中断
	USART_ClearFlag(USART2, USART_FLAG_IDLE);							//清除空闲中断标志 USART2->ICR |= 1<<4;
	USART_Cmd(USART2, ENABLE);	
	
	DMA_DeInit(DMA1_Channel5);                                         //复位DMA1_Channel3
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->RDR));   //外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA_Rx_Buf; 		//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                 //传输方向:外设 -> 内存
	DMA_InitStructure.DMA_BufferSize = DMA_DATA_LEN;                     //传输长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //外设递增:NO
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //内存递增:YES
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //数据宽度:Byte
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                      //循环模式:NO
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            //优先级:高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                       //内存-内存:NO	
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	DMA_ClearFlag(DMA1_FLAG_TC5);
	
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TE, ENABLE);
	DMA_Cmd(DMA1_Channel5, ENABLE);                                    //使能DMA(开始传输)
}
/************************************************
函数名称 ： usart_4g_init
功    能 ： 4g初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void usart_dma_init(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//set and enable dma tx
	DMA_DeInit(DMA1_Channel4);                                         	//复位DMA1_Channel2

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->TDR));   //外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA_Tx_Buf; 		//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                 	//传输方向:内存 -> 外设
	DMA_InitStructure.DMA_BufferSize = DMA_DATA_LEN;            		//传输长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	//外设递增:NO
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            	//内存递增:YES
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    	//数据宽度:Byte
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                      	//循环模式:NO
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            	//优先级:高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                       	//内存-内存:NO
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Channel4, DISABLE);                                    //使能DMA(开始传输)
	
	
	//set and enable dma rx
	DMA_DeInit(DMA1_Channel5);                                         //复位DMA1_Channel3

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->RDR));   //外设地址
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA_Rx_Buf; 		//内存地址
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                 //传输方向:外设 -> 内存
	DMA_InitStructure.DMA_BufferSize = DMA_DATA_LEN;                     //传输长度
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //外设递增:NO
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //内存递增:YES
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //数据宽度:Byte
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                      //循环模式:NO
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            //优先级:高
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                       //内存-内存:NO	
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	
	
	DMA_Cmd(DMA1_Channel5, ENABLE);                                    //使能DMA(开始传输)
}
/************************************************
函数名称 ： USART2_SendByte
功    能 ： 串口2发送一字节数据
参    数 ： Data --- 数据
返 回 值 ： 无
*************************************************/
void Usart2_SendByte(uint8_t Data)
{
  while((USART2->ISR & USART_FLAG_TXE) == RESET);
  USART2->TDR = (Data & (uint16_t)0x01FF);
}

/************************************************
函数名称 ： USART2_SendNByte
功    能 ： 串口2发送N个字符
参    数 ： pData ----- 字符串
            Length --- 长度
返 回 值 ： 无
*************************************************/
void Usart2_SendNByte(uint8_t *pData, uint16_t Length)
{
  while(Length--)
  {
    Usart2_SendByte(*pData);
    pData++;
  }
}
/************************************************
函数名称 ： AT_TEST
功    能 ： 4g数据串口发送
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT_TEST(void)
{
	usart_4g_send_data("AT\r\n",sizeof("AT\r\n"));
}
/************************************************
函数名称 ： AT_CSQ
功    能 ： 
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT_CSQ(void)
{
	usart_4g_send_data("AT+CSQ\r\n",sizeof("AT+CSQ\r\n"));
}
/************************************************
函数名称 ： AT_CGDCONT
功    能 ： 设置APN
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT_CGDCONT(void)											
{
	usart_4g_send_data("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n",sizeof("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n"));
}
/************************************************
函数名称 ： AT_CIPMODE
功    能 ： 设置4g数据传输模式，1为透传模式，0为非透传
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT_CIPMODE(void)											
{
	usart_4g_send_data("AT+CIPMODE=1\r\n",sizeof("AT+CIPMODE=1\r\n"));
}
/************************************************
函数名称 ： AT_NETCLOSE
功    能 ： 关闭网络连接
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT_NETCLOSE(void)											
{
	usart_4g_send_data("AT+NETCLOSE\r\n",sizeof("AT+NETCLOSE\r\n"));
}
/************************************************
函数名称 ： AT_NETOPEN
功    能 ： 打开网络连接
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT_NETOPEN(void)											
{
	usart_4g_send_data("AT+NETOPEN\r\n",sizeof("AT+NETOPEN\r\n"));
}
/************************************************
函数名称 ： AT_CIPRXGET
功    能 ： 设置检索数据模式
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT_CIPRXGET(void)											
{
	usart_4g_send_data("AT+CIPRXGET=1\r\n",sizeof("AT+CIPRXGET=1\r\n"));
}
/************************************************
函数名称 ： AT_CIPOPEN
功    能 ： 设置ip
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT_CIPOPEN(void)											
{
	usart_4g_send_data("AT+CIPOPEN=0,\"TCP\",\"111.67.206.112\",8080\r\n",sizeof("AT+CIPOPEN=0,\"TCP\",\"111.67.206.112\",8080\r\n"));
}
/************************************************
函数名称 ： AT_CHECK_CIPOPEN
功    能 ： 检查连接
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT_CHECK_CIPOPEN(void)											
{
	usart_4g_send_data("AT+CIPOPEN?\r\n",sizeof("AT+CIPOPEN?\r\n"));
}
/************************************************
函数名称 ： AT_ATE
功    能 ： 关闭回显功能
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT_ATE(void)
{
	usart_4g_send_data("ATE\r\n",sizeof("ATE\r\n"));
}
/************************************************
函数名称 ： AT_ATE
功    能 ： 重新进入透传
参    数 ： 无
返 回 值 ： 无
*************************************************/
void AT_ATO(void)
{
	usart_4g_send_data("ATO\r\n",sizeof("ATO\r\n"));
}
/************************************************
函数名称 ： usart_4g_send_data
功    能 ： 4g数据串口发送
参    数 ： 无
返 回 值 ： 无
*************************************************/
void usart_4g_send_data(uint8_t *data,uint32_t len)
{
	Usart2_SendNByte(data,len);
	printf("SendCmd:%s",data);	
}
/************************************************
函数名称 ： sim7600ce_config
功    能 ： 4g模块配置函数
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600ce_config(void)
{
	printf("start sim7600ce_config...\r\n");	
	AT_CGDCONT();
	OSTimeDly(1000);
	AT_CIPMODE();
	OSTimeDly(1000);
	AT_NETOPEN();
	OSTimeDly(1000);
	AT_CIPOPEN();
	OSTimeDly(1000);
	AT_ATO();
	OSTimeDly(1000);
	usart_4g_config(115200);	
	OSTimeDly(1000);
	
	sim7600_flag.config_sim7600_flag = 1;			
	sim7600_flag.sim7600_work_flag = 1;								//假如配置完就成功
	sim7600_flag.sim7600_register_flag = 0;							//注册成功标志位清零，配置后需要重新注册
}
/************************************************
函数名称 ： NVIC_Configuration
功    能 ： 中断配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* 外设中断 */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;                  //IRQ通道:串口1
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;                    //优先级 :1级
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    //使能IRQ通道
  NVIC_Init(&NVIC_InitStructure);

//  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_5_IRQn;       //IRQ通道:DMA1_Channel2_3_IRQn
//  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;                  //优先级 :1级
//  NVIC_Init(&NVIC_InitStructure);
}

/************************************************
函数名称 ： DMA2_IRQHandler
功    能 ： 
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void USART2_IRQHandler(void)
{
	static uint8_t recv_cnt = 0;
	uint8_t i=0;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  
	{
		uint32_t dma_len = 0,i=0; 			
		USART_ClearFlag(USART2, USART_FLAG_IDLE);							//清除空闲中断标志 USART2->ICR |= 1<<4;
		/* close DMA  */
		DMA_Cmd(DMA1_Channel5,DISABLE); 
		/* dma_len=dma_buff_size-idle_size*/
		dma_len = (uint16_t)(DMA_DATA_LEN - DMA_GetCurrDataCounter(DMA1_Channel5)); 	
//		printf((uint8_t*)"RecvCmd:%s,dma_len=%d\r\n",DMA_Rx_Buf,dma_len);
//		for(i=0;i<dma_len;i++)
//		{
//			printf("%x ",DMA_Rx_Buf[i]);
//		}
//		printf("\r\n");	
		sim7600_flaga_deal(DMA_Rx_Buf,dma_len);
		memset(DMA_Rx_Buf,0,sizeof(DMA_Rx_Buf));
		/* set dma send_len ,the dma_receive buff will be clear automatically*/
		DMA_SetCurrDataCounter(DMA1_Channel5,DMA_DATA_LEN);  						
		DMA_Cmd(DMA1_Channel5,ENABLE);
	} 
//	if(USART_GetITStatus(USART2, USART_IT_TC) != RESET) 							  
//	{   
//		printf("Send data over\r\n");
//		USART_ClearITPendingBit(USART2, USART_IT_TC);
//		DMA_Cmd(DMA1_Channel4, DISABLE);		
//	}
}


