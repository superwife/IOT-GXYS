#ifndef _SPI_FLASH_H_
#define _SPI_FLASH_H_
#include "stm32f0xx.h"
#include <stdint.h>
/***********************************************
  W25X80оƬ������1MB (8Mbit)
	ҳ����	16 page
	��������16 sector
	������	16 block
***********************************************/

#define DEVICE_CODE_INFO	"1511010600000001"
#define SOFTWARE_VERSION	"V1.0.0"
#define HARDWARE_VERSION	"V1.0.0"


/*------------------ W25Q80�ڴ���Ϣ -------------------*/
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


/*-------------- spi�洢ɡ��Ϣ�����Ϣ -----------------------*/
#define DEVICE_UMB_INFO_SAVE_SECTOR			1			//�洢�豸����
#define DEVICE_UMB_INFO_SAVE_PAGE			17			//�洢�豸��Ϣҳ
#define DEVICE_UMB_FLASH_INFO_PAGE_BEGIN 	17			//�洢�豸��Ϣ��ʼҳ
#define DEVICE_UMB_FLASH_INFO_PAGE_END 		32			//�洢�豸��Ϣ����ҳ

/*-------------- spi�洢�豸�����Ϣ -----------------------*/
#define DEVICE_CODE_INFO_SECTOR				2			//�洢�豸����
#define DEVICE_CODE_INFO_PAGE				33			//�洢�豸��Ϣҳ
#define DEVICE_CODE_INFO_PAGE_BEGIN 		33			//�洢�豸��Ϣ��ʼҳ
#define DEVICE_CODE_INFO_PAGE_END 			48			//�洢�豸��Ϣ����ҳ

/*--------- ɡ��Ϣ --------*/
struct spi_data_info_st{
	uint8_t info_beg_byte;								//ɡ��Ϣ���ݵ�ǰ�ֽ�λ��	
	uint8_t info_dat_size;								//ɡ��Ϣ���ݴ�С
	uint8_t total_num;									//ɡ��������
	uint8_t left_num;									//ɡʣ������
};
/*--------- �豸��Ϣ --------*/
struct REGISTER_INFO_ST{
	uint8_t dev_code[18];			//�豸����
	uint8_t software_version[7];	//����汾
	uint8_t hardware_version[7];	//Ӳ���汾
};

/*--------- �豸IP��Ϣ--------*/
struct SYS_TCPIP_INFO_ST{
	uint8_t ip[4];					//�豸IP��ַ
	uint16_t port;					//�豸�˿ں�
};


/* �궨�� --------------------------------------------------------------------*/
#define SFLASH_CS_ENABLE          GPIO_ResetBits(GPIOB,GPIO_Pin_12)
#define SFLASH_CS_DISABLE         GPIO_SetBits(GPIOB,GPIO_Pin_12)

void spi_flash_init(void);
void spi_flash_gpio_init(void);
void spi_flash_config(void);
uint8_t SPI_WriteReadByte(uint8_t TxData);
void spiflash_test(void);
void get_umbrella_info(void);
void update_umbrella_info(uint8_t *data,uint8_t len);
void get_device_info(void);

void update_device_info(void);

#endif
