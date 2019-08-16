#ifndef _PTU_APP_H_
#define _PTU_APP_H_
#include <stdint.h>

#define USART1_DMA_LEN		64

void ptu_data_deal(uint8_t *deal_data,uint8_t len);

struct PTU_DATA_ST{
	uint8_t ptu_mode_flag;
	uint8_t ptu_config_flag;
	uint8_t reset_flag;
	uint8_t rx_buf[USART1_DMA_LEN];
};

enum PTU_BOARD_TYPE
{
	PTU_BOARD = 0x0f,     		
	PTU_DEAL_BOARD = 0x01,   			
};

/***** PTUøÿ÷∆√¸¡Ó *****/
enum PTU_CMD_TYPE{
	PTU_CONFIG_4G_CMD = 0x10,
	PTU_CONFIG_DEV_CMD= 0x11,
	PTU_IAP_UPDATE	  = 0x12,
	PTU_RESTART_CMD	  = 0x13,
	PTU_PRINT_CTRL_CMD= 0x14
};

void ptu_dat_init(void);
void ptu_sys_check(void);
void ptu_data_send(uint8_t id, uint8_t *data, uint16_t len);
void reply_read_dev_config_data(void);
void reply_write_dev_config_data(void);
void reply_read_4g_config_data(void);
void reply_write_4g_config_data(void);
void reply_usart_ctrl_data(uint8_t type);



#endif
