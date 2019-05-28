/**
  ********************************  STM32F0xx  *********************************
  * @�ļ���     �� usart.c
  * @����       �� 
  * @��汾     �� V1.5.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2019��05��07��
  * @ժҪ       �� USARTԴ�ļ�
********************************************************************************/
#include "spi_175xx.h"
#include "fm175xx.h"
#include "led_app.h"
#include "wtn6170.h"
#include "gprs_protocol_app.h"
/************************************************
�������� �� ShowID
��    �� �� ��ӡ�豸ID
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void ShowID(uint8_t *p)	 //��ʾ���Ŀ��ţ���ʮ��������ʾ
{
    uint8_t num[9];
    uint8_t i;

    for(i=0; i<4; i++)
    {
        num[i*2]=p[i]/16;
        num[i*2]>9?(num[i*2]+='7'):(num[i*2]+='0');
        num[i*2+1]=p[i]%16;
        num[i*2+1]>9?(num[i*2+1]+='7'):(num[i*2+1]+='0');
    }
    num[8]=0;
    printf("ID>>>%s\r\n", num);
}
/************************************************
�������� �� spi_nfc_read_func
��    �� �� SPI��ȡ����
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void spi_nfc_read_func(void)
{
	static char status;
	static unsigned char TagType[2], SelectedSnr[4];
	
	status= PcdRequest(REQ_ALL,TagType);
	if(!status)
	{
		printf("Read Card ID Success\r\n");
		status = PcdAnticoll(SelectedSnr);
		ShowID(SelectedSnr);
		sim7600_send_umbrella_data(SelectedSnr,sizeof(SelectedSnr));			//����ɡ��Ϣ
		OSTimeDly(500);
//		if(!status)
//		{
//			status=PcdSelect(SelectedSnr);
//			if(!status)
//			{
//				snr = 1;  //������1
//				status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SelectedSnr);// У��1�������룬����λ��ÿһ������3��
//				{
//					if(!status)
//					{
//						status = PcdRead((snr*4+0), buf);  // ��������ȡ1����0�����ݵ�buf[0]-buf[16] 
//						//status = PcdWrite((snr*4+0), buf);  // д������buf[0]-buf[16]д��1����0��
//						if(!status)
//						{
//							//��д�ɹ�������LED
//							WaitCardOff();
//							led_ctrl_func(ALM_LED,1);
//						}
//					}
//				}
//			}
//		}
	}
}

/************************************************
�������� �� spi_nfc_init
��    �� �� SPI��ʼ��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void spi_nfc_init(void)
{
  spi_nfc_gpio_init();
  spi_nfc_mode_config();
	InitializeSystem();
}
/************************************************
�������� �� InitializeSystem
��    �� �� 
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void InitializeSystem(void)
{
	led_ctrl_func(ALM_LED,0);
	delay_10ms(10);
	PcdReset();
	PcdAntennaOff(); 
	PcdAntennaOn();  
	M500PcdConfigISOType( 'A' );
	led_ctrl_func(ALM_LED,1);
	delay_10ms(10);	
	led_ctrl_func(ALM_LED,0);
	delay_10ms(10);
	led_ctrl_func(ALM_LED,1);
	delay_10ms(10);	
	led_ctrl_func(ALM_LED,0);
}

/************************************************
�������� �� SPI_Configuration
��    �� �� SPI����
            SPIΪ��ģʽ��ʱ����ƽʱΪ�ͣ������زɼ�����
            8λ���ݸ�ʽ���������Ƭѡ�����ݸ�λ��ǰ
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void spi_nfc_mode_config(void)
{
//  SPI_InitTypeDef  SPI_InitStructure;

//  /* SPI ��ʼ������ */
//  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI����Ϊ˫��˫��ȫ˫��
//  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                      //����Ϊ�� SPI
//  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI���ͽ��� 8 λ֡�ṹ
//  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                        //ʱ�����ո�
//  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                       //���ݲ����ڵڶ���ʱ����
//  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                          //������� NSS �ź�
//  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //������Ԥ��ƵֵΪ2
//  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                 //���ݴ���� MSB λ��ʼ
//  SPI_InitStructure.SPI_CRCPolynomial = 7;                           //���������� CRCֵ����Ķ���ʽ
//  SPI_Init(SPI1, &SPI_InitStructure);

//  SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
//  SPI_Cmd(SPI1, ENABLE);                                             //ʹ�� SPI1
}
/************************************************
�������� �� spi_gpio_init
��    �� �� SPI��������
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void spi_nfc_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Enable the GPIO Clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOA, ENABLE);

	/* Configure the GPIO pin ��λ���� PB6*/
	GPIO_InitStructure.GPIO_Pin = SPI1_NFC_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                     //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SPI1_NFC_RST_PORT, &GPIO_InitStructure);

	/* Configure the GPIO pin MISO���� PB5*/
	GPIO_InitStructure.GPIO_Pin = SPI1_NFC_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SPI1_NFC_MISO_PORT, &GPIO_InitStructure);


	/* Configure the GPIO pin MOSI���� PB4*/
	GPIO_InitStructure.GPIO_Pin = SPI1_NFC_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                     //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SPI1_NFC_MOSI_PORT, &GPIO_InitStructure);

	/* Configure the GPIO pin SCK���� PB3*/
	GPIO_InitStructure.GPIO_Pin = SPI1_NFC_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                     //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SPI1_NFC_SCK_PORT, &GPIO_InitStructure);



	/* Configure the GPIO pin NSSƬѡ���� PA15*/
	GPIO_InitStructure.GPIO_Pin = SPI1_NFC_NSS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                     //�������
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SPI1_NFC_NSS_PORT, &GPIO_InitStructure);
	

	
}
