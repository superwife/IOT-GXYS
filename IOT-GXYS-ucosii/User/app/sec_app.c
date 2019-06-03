/***************************************************************************//**
  * @file    sec_app.c
  * @author  
  * @version V1.0.0
  * @date    2019��5��6��
  * @brief   ������
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
#include "usart_4g.h"
#include "gprs_protocol_app.h"


/*------------------ public para ---------------------*/
uint8_t nfc_work_mode = 1;									//����nfc����ģʽ��1����ɡģʽ��0����ɡģʽ

/*------------------ private func ---------------------*/
void  App_Task_SEC(void* p_arg);

/*------------------ extern func ----------------------*/
void iwdg_feed(void);

/*------------------ extern para ----------------------*/
extern struct spi_data_info_st spi_info;
extern struct REGISTER_INFO_ST register_info;
extern struct sim7600_flag_st sim7600_flag;


/******************************************************************************
* @brief  App_Task_SEC
* @note   ��������غ���
* @param  ��
* @retval ��
*******************************************************************************/
void  App_Task_SEC(void* p_arg)
{
	uint8_t delay_i = 0;
	uint8_t system_clock = 0;
	
	spi_nfc_init();
	signal_motro_gpio_init();
	wtn6170_gpio_init();
	usart_4g_init();
	spi_flash_init();
	spiflash_test();

	/***************** spi flash �洢���� ******************/
	memset((uint8_t*)&spi_info,0,sizeof(struct spi_data_info_st));
	get_umbrella_info((uint8_t*)&spi_info,sizeof(struct spi_data_info_st));
	printf("info_beg_byte=%d,info_dat_size=%d,total_num=%d,left_num=%d\r\n",
		spi_info.info_beg_byte,spi_info.info_dat_size,spi_info.total_num,spi_info.left_num);
	/***************** spi flash �洢�汾��Ϣ *************/
	get_device_info((uint8_t*)&register_info,sizeof(struct REGISTER_INFO_ST));
	
	while(1)
	{
		iwdg_feed();
		delay_i++;
		/******** �봦�� ********/
		if(delay_i%5==0)
		{
			led_ctrl_func(SYS_LED,delay_i%2);
			printf("wait for id card %d...\r\n",nfc_work_mode);
			system_clock++;
			sim7600_stat_check();
		}	
		/********* nfcʶ�� ***********/
		spi_nfc_read_func();	
		/***** 4gģ��״̬��� ********/	
		if(sim7600_flag.sim7600_work_flag&&(delay_i%30==0))	//���4gģ��������������ô��ʼע�ᣬ���߷��������ź�
		{		
			if(sim7600_flag.sim7600_register_flag==0)
			{
				sim7600_send_register_data((uint8_t *)&register_info,sizeof(struct REGISTER_INFO_ST));
			}
			else
			{
				sim7600_send_lifesignal_data(system_clock);
			}
		}	
		OSTimeDly(200);
	}
}


