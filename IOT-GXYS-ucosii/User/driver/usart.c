/**
  ********************************  STM32F0xx  *********************************
  * @�ļ���     �� usart.c
  * @����       �� 
  * @��汾     �� V1.5.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2019��05��06��
  * @ժҪ       �� USARTԴ�ļ�
  ******************************************************************************/
/*----------------------------------------------------------------------------
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
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
�������� �� USART_GPIO_Configuration
��    �� �� USART��ʹ�ùܽ�������붨��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void USART_GPIO_Configuration(void)
{
  GPIO_InitTypeDef GPIO_InitStructure;
	
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);

  GPIO_PinAFConfig(GPIOA, GPIO_PinSource9, GPIO_AF_1);
  GPIO_PinAFConfig(GPIOA, GPIO_PinSource10, GPIO_AF_1);

  /* ����USART TX_RX ����Ϊ������� */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;            //����
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                       //����ģʽ
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                  //����
  GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                     //�������
  GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                       //����
  GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/************************************************
�������� �� USART_Configuration
��    �� �� ����USART
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void USART_Configuration(void)
{
	USART_InitTypeDef USART_InitStructure;
	NVIC_InitTypeDef NVIC_InitStructure;
  	
	RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );

	/******************************************************************
	USART������ʼ��:  ������     ����λ��   ֹͣλ��  У��λ��
					115200         8         1        0(NO)
	*******************************************************************/
	USART_InitStructure.USART_BaudRate = 9600;                       //�趨��������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //�趨��������λ��
	USART_InitStructure.USART_StopBits = USART_StopBits_1;             //�趨ֹͣλ����
	USART_InitStructure.USART_Parity = USART_Parity_No ;               //����У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //ʹ�ý��պͷ��͹���
	USART_Init(USART1, &USART_InitStructure);                          //��ʼ��USART1

	/* �����ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                  //IRQͨ��:����1
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;                    //���ȼ� :1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    //ʹ��IRQͨ��
	NVIC_Init(&NVIC_InitStructure);
	
	
	USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                     //ʹ��USART1�����ж�
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);					 //ʹ�ܽ��տ����ж�

	USART_Cmd(USART1, ENABLE);                                         //ʹ��USART1
}
/************************************************
�������� �� usart1_init
��    �� �� �������ų�ʼ��
��    �� �� ��
�� �� ֵ �� ��
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

	/* ����USART TX_RX ����Ϊ������� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_9 | GPIO_Pin_10;            //����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                       //����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                  //����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                     //�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                       //����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	
	/******************************************************************
	USART������ʼ��:  ������     ����λ��   ֹͣλ��  У��λ��
	    115200         8           1          0        (NO)
	*******************************************************************/
	USART_InitStructure.USART_BaudRate = BaudRate;                     	//�趨��������
	USART_InitStructure.USART_WordLength = USART_WordLength_8b;        	//�趨��������λ��
	USART_InitStructure.USART_StopBits = USART_StopBits_1;             	//�趨ֹͣλ����
	USART_InitStructure.USART_Parity = USART_Parity_No ;               	//����У��λ
	USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//������������
	USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    	//ʹ�ý��պͷ��͹���
	USART_Init(USART1, &USART_InitStructure);                          	//��ʼ��USART2                                      
		
	/* �����ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;                  //IRQͨ��:����1
	NVIC_InitStructure.NVIC_IRQChannelPriority = 3;                    //���ȼ� :1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    //ʹ��IRQͨ��
	NVIC_Init(&NVIC_InitStructure);	
	
	//���ô��ڣ���ֹ�����жϣ����������ж�
	USART_ITConfig(USART1, USART_IT_IDLE, ENABLE);						//�������н����ж�
	USART_DMACmd(USART1,USART_DMAReq_Rx,ENABLE);					    //ʹ�ܽ���
	USART_ClearFlag(USART1, USART_FLAG_IDLE);							//��������жϱ�־ USART2->ICR |= 1<<4;
	USART_Cmd(USART1, ENABLE);	
	
	DMA_DeInit(DMA1_Channel3);                                         //��λDMA1_Channel3
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART1->RDR));   //�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)ptu_dat.rx_buf; 		//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                 //���䷽��:���� -> �ڴ�
	DMA_InitStructure.DMA_BufferSize = USART1_DMA_LEN;                     //���䳤��
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //�������:NO
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //�ڴ����:YES
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //���ݿ��:Byte
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                      //ѭ��ģʽ:NO
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            //���ȼ�:��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                       //�ڴ�-�ڴ�:NO	
	DMA_Init(DMA1_Channel3, &DMA_InitStructure);
	DMA_ClearFlag(DMA1_FLAG_TC3);
	
	DMA_ITConfig(DMA1_Channel3, DMA_IT_TE, ENABLE);
	DMA_Cmd(DMA1_Channel3, ENABLE);                                    //ʹ��DMA(��ʼ����)
}

/************************************************
�������� �� USART1_IRQHandler
��    �� �� ����USART
��    �� �� ��
�� �� ֵ �� ��
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
		USART_ClearFlag(USART1, USART_FLAG_IDLE);							//��������жϱ�־ USART2->ICR |= 1<<4;
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
�������� �� USART_Initializes
��    �� �� ���ڳ�ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void USART_Initializes(void)
{
//	USART_GPIO_Configuration();
//	USART_Configuration();
	usart1_config(115200);
}

/************************************************
�������� �� USART1_SendByte
��    �� �� ����1����һ�ֽ�����
��    �� �� Data --- ����
�� �� ֵ �� ��
*************************************************/
void USART1_SendByte(uint8_t Data)
{
  while((USART1->ISR & USART_FLAG_TXE) == RESET);
  USART1->TDR = (Data & (uint16_t)0x01FF);
}

/************************************************
�������� �� USART1_SendNByte
��    �� �� ����1����N���ַ�
��    �� �� pData ----- �ַ���
            Length --- ����
�� �� ֵ �� ��
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
�������� �� USART1_Printf
��    �� �� ����1��ӡ���
��    �� �� String --- �ַ���
�� �� ֵ �� ��
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

