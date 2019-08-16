/***************************************************************************//**
  * @file    sec_app.c
  * @author  
  * @version V1.0.0
  * @date    2019年5月9号
  * @brief   主程序
*******************************************************************************/
//#include "stm32f0xx.h"
#include "stm32f0xx_spi.h"
#include "spi_flash.h"
#include "w25qxx.h"
#include <string.h>

/*-------------- public para ----------------*/
struct spi_data_info_st spi_info;
struct REGISTER_INFO_ST register_info;
struct SYS_TCPIP_INFO_ST sys_tcp_info;

/************************************************
函数名称 ： spi_flash_init
功    能 ： spiflash初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void spi_flash_init(void)
{
	spi_flash_gpio_init();
	spi_flash_config();
}

/************************************************
函数名称 ： spi_flash_gpio_init
功    能 ： spiflash引脚初始化
参    数 ： 无
返 回 值 ： 无
*************************************************/
void spi_flash_gpio_init(void)
{
	GPIO_InitTypeDef GPIO_InitStructure;
	
	//使能SPIflash所在GPIO的时钟
    RCC_AHBPeriphClockCmd(RCC_AHBPeriph_GPIOA|RCC_AHBPeriph_GPIOB, ENABLE);
			
	/* Configure SPI2 pins: PB13-SCK, PB14-MISO and PB15-MOSI */
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;
	GPIO_InitStructure.GPIO_PuPd = GPIO_PuPd_UP;
	GPIO_InitStructure.GPIO_Pin = GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF;
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource13,GPIO_AF_0);  
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource14,GPIO_AF_0);
	GPIO_PinAFConfig(GPIOB,GPIO_PinSource15,GPIO_AF_0);	
	
	/*Configure SPI2 pins: PB12-NSS片选*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_12;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;		
	GPIO_Init(GPIOB, &GPIO_InitStructure);
	
	/*Configure SPI2 pins: PA8-CE*/
	GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_8;
	GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_OUT;
	GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
	GPIO_InitStructure.GPIO_OType = GPIO_OType_PP;         
	GPIO_InitStructure.GPIO_PuPd  = GPIO_PuPd_NOPULL;		
	GPIO_Init(GPIOA, &GPIO_InitStructure);
	GPIO_SetBits(GPIOA,GPIO_Pin_8);
	
	SFLASH_CS_DISABLE;
}
/************************************************
函数名称 ： spi_flash_config
功    能 ： spiflash配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
void spi_flash_config(void)
{
	SPI_InitTypeDef  SPI_InitStructure;
	
	RCC_APB1PeriphClockCmd(RCC_APB1Periph_SPI2, ENABLE );

	/* SPI 初始化定义 */
	SPI_InitStructure.SPI_Direction = SPI_Direction_2Lines_FullDuplex; //SPI设置为双线双向全双工
	SPI_InitStructure.SPI_Mode = SPI_Mode_Master;                      //设置为主 SPI
	SPI_InitStructure.SPI_DataSize = SPI_DataSize_8b;                  //SPI发送接收 8 位帧结构
	SPI_InitStructure.SPI_CPOL = SPI_CPOL_High;                        //时钟悬空高
	SPI_InitStructure.SPI_CPHA = SPI_CPHA_2Edge;                       //数据捕获于第二个时钟沿
	SPI_InitStructure.SPI_NSS = SPI_NSS_Soft;                          //软件控制 NSS 信号
	SPI_InitStructure.SPI_BaudRatePrescaler = SPI_BaudRatePrescaler_2; //波特率预分频值为2
	SPI_InitStructure.SPI_FirstBit = SPI_FirstBit_MSB;                 //数据传输从 MSB 位开始
	SPI_InitStructure.SPI_CRCPolynomial = 7;                           //定义了用于 CRC值计算的多项式
	SPI_Init(SPI2, &SPI_InitStructure);

	SPI_RxFIFOThresholdConfig(SPI2, SPI_RxFIFOThreshold_QF);
	SPI_Cmd(SPI2, ENABLE);                                             //使能 SPI2
}
/************************************************
函数名称 ： SPI_WriteReadByte
功    能 ： spiflash配置
参    数 ： 无
返 回 值 ： 无
*************************************************/
uint8_t SPI_WriteReadByte(uint8_t TxData)
{
  while((SPI2->SR & SPI_I2S_FLAG_TXE) == (uint16_t)RESET);
  SPI_SendData8(SPI2, TxData);

  while((SPI2->SR & SPI_I2S_FLAG_RXNE) == (uint16_t)RESET);
  return SPI_ReceiveData8(SPI2);
}

/************************************************
函数名称 ： spiflash_test
功    能 ： spiflash测试
参    数 ： 无
返 回 值 ： 无
*************************************************/
void spiflash_test(void)
{
	uint16_t spi_flash_id = 0;
	uint32_t page_index;
	uint32_t i,err_cnt=0,cnt=0;
	uint8_t write_buf[SPI_FLASH_PAGE_BYTE],read_buf[SPI_FLASH_PAGE_BYTE];
	
	spi_flash_id = SFLASH_ReadID();
	printf("spi_flash_id;%x\r\n",spi_flash_id);
	printf("spiflash_test start\r\n");
	
	for (i=0; i<SPI_FLASH_PAGE_BYTE; i++)
	{
		write_buf[i] = (uint8_t)i;
	}
	for (i=0; i<1; i++)	
	{
		SFLASH_EraseSector(i);															
	}
	for (page_index=0; page_index<15; page_index++)
	{
		SFLASH_WritePage(write_buf, page_index*SPI_FLASH_PAGE_BYTE, SPI_FLASH_PAGE_BYTE);	
		//SFLASH_WriteNByte(write_buf, page_index*SPI_FLASH_PAGE_BYTE, SPI_FLASH_PAGE_BYTE);
		SFLASH_ReadNByte(read_buf,page_index*SPI_FLASH_PAGE_BYTE, SPI_FLASH_PAGE_BYTE);         //读取n字节数据
		for (i=0; i<SPI_FLASH_PAGE_BYTE; i++)
		{
			if (read_buf[i] != write_buf[i])																			
			{
				cnt++;
				printf("page_index:%d,read_buf[%d]=%d,write_buf[%d]=%d\r\n", page_index,i,read_buf[i], i,write_buf[i]);
			}
		}
		if(cnt)
		{
			cnt = 0;
			err_cnt++;
		}
	}
	if(err_cnt)
	{
		err_cnt = 0;
		printf("spiflash test error! error cnt =%d \r\n",err_cnt);
	}
	else
	{
		printf("*******************spiflash test success!************************\r\n");
	}
}
/************************************************
函数名称 ： get_umbrella_info
功    能 ： 获取伞信息
参    数 ： 指针+ 指针长度
返 回 值 ： 无
*************************************************/
void get_umbrella_info(void)
{
	uint8_t read_page_buf[SPI_FLASH_PAGE_BYTE] = {0};
	uint8_t write_page_buf[SPI_FLASH_PAGE_BYTE] = {0};
	
	memset(read_page_buf,0,sizeof(read_page_buf));
	SFLASH_ReadNByte(read_page_buf,DEVICE_UMB_INFO_SAVE_PAGE*SPI_FLASH_PAGE_BYTE, SPI_FLASH_PAGE_BYTE);         //读取页字节数据
	memmove((uint8_t*)&spi_info,read_page_buf,sizeof(struct spi_data_info_st));
	if(spi_info.total_num==0xff)
	{
		printf("write init umbrella infor\r\n");
		spi_info.info_beg_byte = 0;
		spi_info.info_dat_size = sizeof(struct spi_data_info_st);
		spi_info.total_num = 25;
		spi_info.left_num  = 0;	
		
		SFLASH_EraseSector(DEVICE_UMB_INFO_SAVE_SECTOR);															//擦除扇区信息
		memset(write_page_buf,0,sizeof(write_page_buf));
		memmove(write_page_buf,&spi_info,sizeof(struct spi_data_info_st));
		SFLASH_WritePage(write_page_buf, DEVICE_UMB_INFO_SAVE_PAGE*SPI_FLASH_PAGE_BYTE, SPI_FLASH_PAGE_BYTE);		//更新页字节数据	
	}
}
/************************************************
函数名称 ： get_umbrella_info
功    能 ： 更新伞信息
参    数 ： 指针+ 指针长度
返 回 值 ： 无
*************************************************/
void update_umbrella_info(uint8_t *data,uint8_t len)
{
	uint8_t write_page_buf[SPI_FLASH_PAGE_BYTE] = {0};
	
	SFLASH_EraseSector(DEVICE_UMB_INFO_SAVE_SECTOR);															//擦除扇区信息			
	memset(write_page_buf,0,sizeof(write_page_buf));
	memmove(write_page_buf,data,len);
	SFLASH_WritePage(write_page_buf, DEVICE_UMB_INFO_SAVE_PAGE*SPI_FLASH_PAGE_BYTE, SPI_FLASH_PAGE_BYTE);		//更新页字节数据	
}

/************************************************
函数名称 ： get_device_info
功    能 ： 获取设备信息
参    数 ： 无
返 回 值 ： 无
*************************************************/
void get_device_info(void)
{	
	uint8_t read_page_buf[SPI_FLASH_PAGE_BYTE] = {0};
	uint8_t write_page_buf[SPI_FLASH_PAGE_BYTE] = {0};
	
	memset(read_page_buf,0,sizeof(read_page_buf));
	SFLASH_ReadNByte(read_page_buf,DEVICE_CODE_INFO_PAGE*SPI_FLASH_PAGE_BYTE, SPI_FLASH_PAGE_BYTE);         	//读取页字节数据
	memmove((uint8_t*)&register_info,read_page_buf,sizeof(struct REGISTER_INFO_ST));
	
	//117.177.222.140,15100   正常使用
	//47.100.119.84 ， 30684   调试使用
//	sys_tcp_info.ip[0] = 47;
//	sys_tcp_info.ip[1] = 100;
//	sys_tcp_info.ip[2] = 119;
//	sys_tcp_info.ip[3] = 84;	
//	sys_tcp_info.port  = 30684;
	
	sys_tcp_info.ip[0] = 117;
	sys_tcp_info.ip[1] = 177;
	sys_tcp_info.ip[2] = 222;
	sys_tcp_info.ip[3] = 140;	
	sys_tcp_info.port  = 15100;
	
	if(register_info.software_version[0]!='V')																	//确认是否有版本更新
	{
		printf("write init version device infor\r\n");
		memmove(register_info.dev_code,DEVICE_CODE_INFO,sizeof(DEVICE_CODE_INFO));
		memmove(register_info.software_version,SOFTWARE_VERSION,sizeof(SOFTWARE_VERSION));
		memmove(register_info.hardware_version,HARDWARE_VERSION,sizeof(HARDWARE_VERSION));		
		
		SFLASH_EraseSector(DEVICE_CODE_INFO_SECTOR);															//擦除扇区信息
		memset(write_page_buf,0,sizeof(write_page_buf));
		memmove(write_page_buf,&register_info,sizeof(struct REGISTER_INFO_ST));
		SFLASH_WritePage(write_page_buf, DEVICE_CODE_INFO_PAGE*SPI_FLASH_PAGE_BYTE, SPI_FLASH_PAGE_BYTE);		//更新页字节数据	
	}
	
	
//	if(strncmp(register_info.software_version,SOFTWARE_VERSION,sizeof(SOFTWARE_VERSION))!=0)					//确认是否有版本更新
//	{
//		printf("New version update device infor\r\n");
//		memmove(register_info.dev_code,DEVICE_CODE_INFO,sizeof(register_info.dev_code));
//		memmove(register_info.software_version,SOFTWARE_VERSION,sizeof(register_info.software_version));
//		memmove(register_info.hardware_version,HARDWARE_VERSION,sizeof(register_info.hardware_version));		
//		
//		SFLASH_EraseSector(DEVICE_CODE_INFO_SECTOR);															//擦除扇区信息
//		memset(write_page_buf,0,sizeof(write_page_buf));
//		memmove(write_page_buf,&register_info,sizeof(struct REGISTER_INFO_ST));
//		SFLASH_WritePage(write_page_buf, DEVICE_CODE_INFO_PAGE*SPI_FLASH_PAGE_BYTE, SPI_FLASH_PAGE_BYTE);		//更新页字节数据	
//	}
//	memmove(data,&register_info,sizeof(struct REGISTER_INFO_ST));
	
}
/************************************************
函数名称 ： update_device_info
功    能 ： 获取设备信息
参    数 ： 无
返 回 值 ： 无
*************************************************/
void update_device_info(void)
{	
	uint8_t write_page_buf[SPI_FLASH_PAGE_BYTE] = {0};
	uint8_t read_page_buf[SPI_FLASH_PAGE_BYTE] = {0};
	
	printf("before version:\r\ndevice code:%s \r\nsoftware_version:%s\r\nhardware_version:%s\r\n",register_info.dev_code,register_info.software_version,register_info.hardware_version);
	SFLASH_EraseSector(DEVICE_CODE_INFO_SECTOR);															//擦除扇区信息
	
	memmove(write_page_buf,&register_info,sizeof(struct REGISTER_INFO_ST));
	SFLASH_WritePage(write_page_buf, DEVICE_CODE_INFO_PAGE*SPI_FLASH_PAGE_BYTE, SPI_FLASH_PAGE_BYTE);		//更新页字节数据	

	SFLASH_ReadNByte(read_page_buf,DEVICE_CODE_INFO_PAGE*SPI_FLASH_PAGE_BYTE, SPI_FLASH_PAGE_BYTE);         //读取页字节数据
	memmove(&register_info,read_page_buf,sizeof(struct REGISTER_INFO_ST));
	printf("after version:\r\ndevice code:%s \r\nsoftware_version:%s\r\nhardware_version:%s\r\n",register_info.dev_code,register_info.software_version,register_info.hardware_version);
}





	



