/***************************************************************************//**
  * @file    sec_app.c
  * @author  
  * @version V1.0.0
  * @date    2019年5月6号
  * @brief   主程序
*******************************************************************************/
#include <string.h>
#include "stm32f0xx.h"
#include "ucos_ii.h"
#include "led_app.h"
#include "usart.h"
#include "spi_175xx.h"
#include "spi_flash.h"
#include "wtn6170.h"
#include "motor_ctrl.h"


/*------------------ public para ---------------------*/


/*------------------ private func ---------------------*/
void  App_Task_SEC(void* p_arg);

/*------------------ extern func ----------------------*/
void iwdg_feed(void);

/*------------------ extern para ----------------------*/
extern struct spi_data_info_st spi_info;
extern struct REGISTER_INFO_ST register_info;


/******************************************************************************
* @brief  App_Task_SEC
* @note   秒任务相关函数
* @param  无
* @retval 无
*******************************************************************************/
void  App_Task_SEC(void* p_arg)
{
	uint8_t delay_i = 0;
	LED_Configuration ();
	USART_Initializes();
	spi_nfc_init();
	signal_motro_gpio_init();
	wtn6170_gpio_init();
	spi_flash_init();
	spiflash_test();
	
	/***************** spi flash 存储数据 ******************/
	memset((uint8_t*)&spi_info,0,sizeof(struct spi_data_info_st));
	get_umbrella_info((uint8_t*)&spi_info,sizeof(struct spi_data_info_st));
	printf("info_beg_byte=%d,info_dat_size=%d,total_num=%d,left_num=%d\r\n",
		spi_info.info_beg_byte,spi_info.info_dat_size,spi_info.total_num,spi_info.left_num);
	/***************** spi flash 存储版本信息 *************/
	get_device_info((uint8_t*)&register_info,sizeof(struct REGISTER_INFO_ST));
	
	while(1)
	{
		iwdg_feed();
		delay_i++;
		if(delay_i%5==0)
		{
			led_ctrl_func(SYS_LED,delay_i%2);
			printf("wait for id card...\r\n");
		}	
		OSTimeDly(200);
		spi_nfc_read_func();			
	}
}


