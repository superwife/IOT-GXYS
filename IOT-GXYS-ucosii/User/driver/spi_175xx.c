/**
  ********************************  STM32F0xx  *********************************
  * @文件名     ： usart.c
  * @作者       ： 
  * @库版本     ： V1.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2019年05月07日
  * @摘要       ： USART源文件
********************************************************************************/
#include "spi_175xx.h"
#include "fm175xx.h"
#include "led_app.h"
#include "wtn6170.h"
#include "gprs_protocol_app.h"
/************************************************
函数名称 ： ShowID
功    能 ： 打印设备ID
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void ShowID(uint8_t *p)	 //显示卡的卡号，以十六进制显示
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
函数名称 ： spi_nfc_read_func
功    能 ： SPI读取函数
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
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
		sim7600_send_umbrella_data(SelectedSnr,sizeof(SelectedSnr));			//发送伞信息
		OSTimeDly(500);
//		if(!status)
//		{
//			status=PcdSelect(SelectedSnr);
//			if(!status)
//			{
//				snr = 1;  //扇区号1
//				status = PcdAuthState(KEYA, (snr*4+3), DefaultKey, SelectedSnr);// 校验1扇区密码，密码位于每一扇区第3块
//				{
//					if(!status)
//					{
//						status = PcdRead((snr*4+0), buf);  // 读卡，读取1扇区0块数据到buf[0]-buf[16] 
//						//status = PcdWrite((snr*4+0), buf);  // 写卡，将buf[0]-buf[16]写入1扇区0块
//						if(!status)
//						{
//							//读写成功，点亮LED
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
函数名称 ： spi_nfc_init
功    能 ： SPI初始化
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void spi_nfc_init(void)
{
  spi_nfc_gpio_init();
  spi_nfc_mode_config();
	InitializeSystem();
}
/************************************************
函数名称 ： InitializeSystem
功    能 ： 
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
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
函数名称 ： SPI_Configuration
功    能 ： SPI配置
            SPI为主模式，时钟线平时为低，上升沿采集数据
            8位数据格式，软件控制片选，数据高位在前
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void spi_nfc_mode_config(void)
{
//  SPI_InitTypeDef  SPI_InitStructure;

//  /* SPI 初始化定义 */
//  SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI设置为双线双向全双工
//  SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                      //设置为主 SPI
//  SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI发送接收 8 位帧结构
//  SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                        //时钟悬空高
//  SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                       //数据捕获于第二个时钟沿
//  SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                          //软件控制 NSS 信号
//  SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //波特率预分频值为2
//  SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                 //数据传输从 MSB 位开始
//  SPI_InitStructure.SPI_CRCPolynomial = 7;                           //定义了用于 CRC值计算的多项式
//  SPI_Init(SPI1, &SPI_InitStructure);

//  SPI_RxFIFOThresholdConfig(SPI1, SPI_RxFIFOThreshold_QF);
//  SPI_Cmd(SPI1, ENABLE);                                             //使能 SPI1
}
/************************************************
函数名称 ： spi_gpio_init
功    能 ： SPI引脚配置
参    数 ： 无
返 回 值 ： 无
作    者 ： strongerHuang
*************************************************/
void spi_nfc_gpio_init(void)
{
	GPIO_InitTypeDef  GPIO_InitStructure;

	/* Enable the GPIO Clock */
	RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOB|RCC_AHBPeriph_GPIOA, ENABLE);

	/* Configure the GPIO pin 复位引脚 PB6*/
	GPIO_InitStructure.GPIO_Pin = SPI1_NFC_RST_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                     //推完输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SPI1_NFC_RST_PORT, &GPIO_InitStructure);

	/* Configure the GPIO pin MISO引脚 PB5*/
	GPIO_InitStructure.GPIO_Pin = SPI1_NFC_MISO_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SPI1_NFC_MISO_PORT, &GPIO_InitStructure);


	/* Configure the GPIO pin MOSI引脚 PB4*/
	GPIO_InitStructure.GPIO_Pin = SPI1_NFC_MOSI_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                     //推完输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SPI1_NFC_MOSI_PORT, &GPIO_InitStructure);

	/* Configure the GPIO pin SCK引脚 PB3*/
	GPIO_InitStructure.GPIO_Pin = SPI1_NFC_SCK_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                     //推完输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SPI1_NFC_SCK_PORT, &GPIO_InitStructure);



	/* Configure the GPIO pin NSS片选引脚 PA15*/
	GPIO_InitStructure.GPIO_Pin = SPI1_NFC_NSS_PIN;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_OUT;
	//	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;                     //推完输出
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_Init(SPI1_NFC_NSS_PORT, &GPIO_InitStructure);
	

	
}
