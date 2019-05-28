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
	
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_USART1, ENABLE );

  /******************************************************************
  USART������ʼ��:  ������     ����λ��   ֹͣλ��  У��λ��
                    115200         8         1        0(NO)
  *******************************************************************/
  USART_InitStructure.USART_BaudRate = 115200;                       //�趨��������
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;        //�趨��������λ��
  USART_InitStructure.USART_StopBits = USART_StopBits_1;             //�趨ֹͣλ����
  USART_InitStructure.USART_Parity = USART_Parity_No ;               //����У��λ
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;//������������
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;    //ʹ�ý��պͷ��͹���
  USART_Init(USART1, &USART_InitStructure);                          //��ʼ��USART1

  USART_ITConfig(USART1, USART_IT_RXNE, ENABLE);                     //ʹ��USART1�����ж�

  USART_Cmd(USART1, ENABLE);                                         //ʹ��USART1
}

/************************************************
�������� �� USART_Initializes
��    �� �� ���ڳ�ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void USART_Initializes(void)
{
  USART_GPIO_Configuration();
  USART_Configuration();
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


