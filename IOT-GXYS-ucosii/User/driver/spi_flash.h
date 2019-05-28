#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_
#include "stm32f0xx.h"
#include <stdint.h>
/***********************************************
  W25X80芯片容量：1MB (8Mbit)
	页数：	16 page
	扇区数：16 sector
	块数：	16 block
***********************************************/

#define DEVICE_CODE_INFO	"20190527000000001"
#define SOFTWARE_VERSION	1000
#define HARDWARE_VERSION	1000


/*------------------ W25Q80内存信息 -------------------*/
#define SPI_FLASH_BASE_ADDR 			0x000000
#define SPI_FLASH_PAGE_BYTE 			256
#define SPI_FLASH_PAGE_BEGIN 			0
#define SPI_FLASH_PAGE_END 				15
#define SPI_FLASH_PAGE_NUM 				16

#define SPI_FLASH_SECTOR_BEGIN 			0
#define SPI_FLASH_SECTOR_END 			15
#define SPI_FLASH_SECTOR_NUM 			16

#define SPI_FLASH_BLOCK_BEGIN 			0
#define SPI_FLASH_BLOCK_END 			15
#define SPI_FLASH_BLOCK_NUM 			16

#define SPI_FLASH_TOTAL_PAGE			SPI_FLASH_BLOCK_NUM*SPI_FLASH_SECTOR_NUM*SPI_FLASH_PAGE_NUM


/*-------------- spi存储伞信息框架信息 -----------------------*/
#define DEVICE_SPI_INFO_SAVE_SECTOR			1			//存储设备扇区
#define DEVICE_SPI_INFO_SAVE_PAGE			17			//存储设备信息页
#define DEVICE_SPI_FLASH_INFO_PAGE_BEGIN 	17			//存储设备信息开始页
#define DEVICE_SPI_FLASH_INFO_PAGE_END 		32			//存储设备信息结束页

/*-------------- spi存储设备框架信息 -----------------------*/
#define DEVICE_CODE_INFO_SECTOR				2			//存储设备扇区
#define DEVICE_CODE_INFO_PAGE				33			//存储设备信息页
#define DEVICE_CODE_INFO_PAGE_BEGIN 		33			//存储设备信息开始页
#define DEVICE_CODE_INFO_PAGE_END 			48			//存储设备信息结束页

/*--------- 伞信息 --------*/
struct spi_data_info_st{
	uint8_t info_beg_byte;								//伞信息数据当前字节位置	
	uint8_t info_dat_size;								//伞信息数据大小
	uint8_t total_num;									//伞共计数量
	uint8_t left_num;									//伞剩余数量
};
/*--------- 设备信息 --------*/
struct REGISTER_INFO_ST{
	uint8_t dev_code[18];			//设备编码
	uint16_t software_version;		//软件版本
	uint16_t hardware_version;		//硬件版本
};

/* 宏定义 --------------------------------------------------------------------*/
#define SFLASH_CS_ENABLE          GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define SFLASH_CS_DISABLE         GPIO_SetBits(GPIOB,GPIO_Pin_12)

void spi_flash_init(void);
void spi_flash_gpio_init(void);
void spi_flash_config(void);
uint8_t SPI_WriteReadByte(uint8_t TxData);
void spiflash_test(void);
void get_umbrella_info(uint8_t *data,uint8_t len);
void update_umbrella_info(uint8_t *data,uint8_t len);
void get_device_info(uint8_t *data,uint8_t len);

#endif
