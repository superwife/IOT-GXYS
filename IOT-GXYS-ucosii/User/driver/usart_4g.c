/***************************************************************************//**
  * @file    usart_4g.c
  * @author  
  * @version V1.0.0
  * @date    2019��5��6��
  * @brief   ������
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
�������� �� usart_4g_init
��    �� �� 4g��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void usart_4g_init(void)
{
	usart_4g_gpio_init();
	usart_4g_config(115200);
	sim7600_flaga_init();
}
/************************************************
�������� �� sim7600_flaga_init
��    �� �� sim7600״̬��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void sim7600_flaga_init(void)
{
	sim7600_flag.config_sim7600_flag   = 0;			//4gģ�����ñ�־
	sim7600_flag.sim7600_work_flag 	  = 0;			//4gģ�鹤����־
	sim7600_flag.sim7600_status_flag   = 0;			//4gģ��status״̬��־
	sim7600_flag.sim7600_register_flag = 0;			//4gģ��ע��״̬
}
/************************************************
�������� �� usart_4g_gpio_init
��    �� �� 4g�������ų�ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void usart_4g_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA, ENABLE);
	
	/* ����4G-STATUS ����Ϊ���� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_1;            				//����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;                       	//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                  	//����
	GPIO_Init(GPIOA, &GPIO_InitStructure);
}
/************************************************
�������� �� usart_4g_gpio_init
��    �� �� 4g�������ų�ʼ��
��    �� �� ��
�� �� ֵ �� ��
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

	/* ����USART TX_RX ����Ϊ������� */
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_2 | GPIO_Pin_3;            	//����
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;                       	//����ģʽ
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;                  	//����
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                     	//�������
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;                       	//����
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
	USART_Init(USART2, &USART_InitStructure);                          	//��ʼ��USART2                                      
		
	/* �����ж� */
	NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;                  //IRQͨ��:����1
	NVIC_InitStructure.NVIC_IRQChannelPriority = 2;                    //���ȼ� :1��
	NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    //ʹ��IRQͨ��
	NVIC_Init(&NVIC_InitStructure);	
	
	//���ô��ڣ���ֹ�����жϣ����������ж�
	USART_ITConfig(USART2, USART_IT_IDLE, ENABLE);						//�������н����ж�
	USART_DMACmd(USART2,USART_DMAReq_Rx,ENABLE);					    //ʹ�ܽ���
	//USART_ITConfig(USART2, USART_IT_RXNE, ENABLE);					//��ֹ�����ж�
	USART_ClearFlag(USART2, USART_FLAG_IDLE);							//��������жϱ�־ USART2->ICR |= 1<<4;
	USART_Cmd(USART2, ENABLE);	
	
	DMA_DeInit(DMA1_Channel5);                                         //��λDMA1_Channel3
	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->RDR));   //�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA_Rx_Buf; 		//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                 //���䷽��:���� -> �ڴ�
	DMA_InitStructure.DMA_BufferSize = DMA_DATA_LEN;                     //���䳤��
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //�������:NO
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //�ڴ����:YES
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //���ݿ��:Byte
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                      //ѭ��ģʽ:NO
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            //���ȼ�:��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                       //�ڴ�-�ڴ�:NO	
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	DMA_ClearFlag(DMA1_FLAG_TC5);
	
	DMA_ITConfig(DMA1_Channel5, DMA_IT_TE, ENABLE);
	DMA_Cmd(DMA1_Channel5, ENABLE);                                    //ʹ��DMA(��ʼ����)
}
/************************************************
�������� �� usart_4g_init
��    �� �� 4g��ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void usart_dma_init(void)
{
	DMA_InitTypeDef DMA_InitStructure;

	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);
	
	//set and enable dma tx
	DMA_DeInit(DMA1_Channel4);                                         	//��λDMA1_Channel2

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->TDR));   //�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA_Tx_Buf; 		//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;                 	//���䷽��:�ڴ� -> ����
	DMA_InitStructure.DMA_BufferSize = DMA_DATA_LEN;            		//���䳤��
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;  	//�������:NO
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            	//�ڴ����:YES
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    	//���ݿ��:Byte
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                      	//ѭ��ģʽ:NO
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            	//���ȼ�:��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                       	//�ڴ�-�ڴ�:NO
	DMA_Init(DMA1_Channel4, &DMA_InitStructure);
	
	USART_DMACmd(USART2, USART_DMAReq_Tx, ENABLE);
	DMA_Cmd(DMA1_Channel4, DISABLE);                                    //ʹ��DMA(��ʼ����)
	
	
	//set and enable dma rx
	DMA_DeInit(DMA1_Channel5);                                         //��λDMA1_Channel3

	DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t)(&(USART2->RDR));   //�����ַ
	DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t)DMA_Rx_Buf; 		//�ڴ��ַ
	DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;                 //���䷽��:���� -> �ڴ�
	DMA_InitStructure.DMA_BufferSize = DMA_DATA_LEN;                     //���䳤��
	DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;   //�������:NO
	DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;            //�ڴ����:YES
	DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
	DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;    //���ݿ��:Byte
	DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;                      //ѭ��ģʽ:NO
	DMA_InitStructure.DMA_Priority = DMA_Priority_VeryHigh;            //���ȼ�:��
	DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;                       //�ڴ�-�ڴ�:NO	
	DMA_Init(DMA1_Channel5, &DMA_InitStructure);
	
	
	DMA_Cmd(DMA1_Channel5, ENABLE);                                    //ʹ��DMA(��ʼ����)
}
/************************************************
�������� �� USART2_SendByte
��    �� �� ����2����һ�ֽ�����
��    �� �� Data --- ����
�� �� ֵ �� ��
*************************************************/
void Usart2_SendByte(uint8_t Data)
{
  while((USART2->ISR & USART_FLAG_TXE) == RESET);
  USART2->TDR = (Data & (uint16_t)0x01FF);
}

/************************************************
�������� �� USART2_SendNByte
��    �� �� ����2����N���ַ�
��    �� �� pData ----- �ַ���
            Length --- ����
�� �� ֵ �� ��
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
�������� �� AT_TEST
��    �� �� 4g���ݴ��ڷ���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void AT_TEST(void)
{
	usart_4g_send_data("AT\r\n",sizeof("AT\r\n"));
}
/************************************************
�������� �� AT_CSQ
��    �� �� 
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void AT_CSQ(void)
{
	usart_4g_send_data("AT+CSQ\r\n",sizeof("AT+CSQ\r\n"));
}
/************************************************
�������� �� AT_CGDCONT
��    �� �� ����APN
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void AT_CGDCONT(void)											
{
	usart_4g_send_data("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n",sizeof("AT+CGDCONT=1,\"IP\",\"CMNET\"\r\n"));
}
/************************************************
�������� �� AT_CIPMODE
��    �� �� ����4g���ݴ���ģʽ��1Ϊ͸��ģʽ��0Ϊ��͸��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void AT_CIPMODE(void)											
{
	usart_4g_send_data("AT+CIPMODE=1\r\n",sizeof("AT+CIPMODE=1\r\n"));
}
/************************************************
�������� �� AT_NETCLOSE
��    �� �� �ر���������
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void AT_NETCLOSE(void)											
{
	usart_4g_send_data("AT+NETCLOSE\r\n",sizeof("AT+NETCLOSE\r\n"));
}
/************************************************
�������� �� AT_NETOPEN
��    �� �� ����������
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void AT_NETOPEN(void)											
{
	usart_4g_send_data("AT+NETOPEN\r\n",sizeof("AT+NETOPEN\r\n"));
}
/************************************************
�������� �� AT_CIPRXGET
��    �� �� ���ü�������ģʽ
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void AT_CIPRXGET(void)											
{
	usart_4g_send_data("AT+CIPRXGET=1\r\n",sizeof("AT+CIPRXGET=1\r\n"));
}
/************************************************
�������� �� AT_CIPOPEN
��    �� �� ����ip
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void AT_CIPOPEN(void)											
{
	usart_4g_send_data("AT+CIPOPEN=0,\"TCP\",\"111.67.206.112\",8080\r\n",sizeof("AT+CIPOPEN=0,\"TCP\",\"111.67.206.112\",8080\r\n"));
}
/************************************************
�������� �� AT_CHECK_CIPOPEN
��    �� �� �������
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void AT_CHECK_CIPOPEN(void)											
{
	usart_4g_send_data("AT+CIPOPEN?\r\n",sizeof("AT+CIPOPEN?\r\n"));
}
/************************************************
�������� �� AT_ATE
��    �� �� �رջ��Թ���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void AT_ATE(void)
{
	usart_4g_send_data("ATE\r\n",sizeof("ATE\r\n"));
}
/************************************************
�������� �� AT_ATE
��    �� �� ���½���͸��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void AT_ATO(void)
{
	usart_4g_send_data("ATO\r\n",sizeof("ATO\r\n"));
}
/************************************************
�������� �� usart_4g_send_data
��    �� �� 4g���ݴ��ڷ���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void usart_4g_send_data(uint8_t *data,uint32_t len)
{
	Usart2_SendNByte(data,len);
	printf("SendCmd:%s",data);	
}
/************************************************
�������� �� sim7600ce_config
��    �� �� 4gģ�����ú���
��    �� �� ��
�� �� ֵ �� ��
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
	sim7600_flag.sim7600_work_flag = 1;								//����������ͳɹ�
	sim7600_flag.sim7600_register_flag = 0;							//ע��ɹ���־λ���㣬���ú���Ҫ����ע��
}
/************************************************
�������� �� NVIC_Configuration
��    �� �� �ж�����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void NVIC_Configuration(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* �����ж� */
  NVIC_InitStructure.NVIC_IRQChannel = USART2_IRQn;                  //IRQͨ��:����1
  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;                    //���ȼ� :1��
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;                    //ʹ��IRQͨ��
  NVIC_Init(&NVIC_InitStructure);

//  NVIC_InitStructure.NVIC_IRQChannel = DMA1_Channel4_5_IRQn;       //IRQͨ��:DMA1_Channel2_3_IRQn
//  NVIC_InitStructure.NVIC_IRQChannelPriority = 2;                  //���ȼ� :1��
//  NVIC_Init(&NVIC_InitStructure);
}

/************************************************
�������� �� DMA2_IRQHandler
��    �� �� 
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void USART2_IRQHandler(void)
{
	static uint8_t recv_cnt = 0;
	uint8_t i=0;
	if(USART_GetITStatus(USART2, USART_IT_IDLE) != RESET)  
	{
		uint32_t dma_len = 0,i=0; 			
		USART_ClearFlag(USART2, USART_FLAG_IDLE);							//��������жϱ�־ USART2->ICR |= 1<<4;
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


