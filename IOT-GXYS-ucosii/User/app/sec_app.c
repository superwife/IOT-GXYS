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
#include "ptu_app.h"

/*------------------ public para ---------------------*/
uint32_t sim7600_connt_err_cnt = 0;
/*------------------ private func ---------------------*/
void  App_Task_SEC(void* p_arg);

/*------------------ extern func ----------------------*/
void iwdg_feed(void);

/*------------------ extern para ----------------------*/
extern struct spi_data_info_st spi_info;
extern struct REGISTER_INFO_ST register_info;
extern struct sim7600_flag_st sim7600_flag;
extern struct PTU_DATA_ST ptu_dat;
extern struct SYS_TCPIP_INFO_ST sys_tcp_info;

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
	
	ptu_dat_init();
	signal_motor_gpio_init();
	wtn6170_gpio_init();	
	usart_4g_init();
	spi_flash_init();
	spiflash_test();
	led_ctrl_func(ALM_LED,1);

	/***************** spi flash �洢���� ******************/
	memset((uint8_t*)&spi_info,0,sizeof(struct spi_data_info_st));
	get_umbrella_info();
	printf("info_beg_byte=%d,info_dat_size=%d,total_num=%d,left_num=%d\r\n",
		spi_info.info_beg_byte,spi_info.info_dat_size,spi_info.total_num,spi_info.left_num);
	/***************** spi flash �洢�汾��Ϣ *************/
	get_device_info();
	printf("device code:%s \r\nsoftware_version:%s\r\nhardware_version:%s\r\n",
		register_info.dev_code,register_info.software_version,register_info.hardware_version);
	printf("device ip:%d.%d.%d.%d  port:%d\r\n",sys_tcp_info.ip[0],sys_tcp_info.ip[1],sys_tcp_info.ip[2],sys_tcp_info.ip[3],sys_tcp_info.port);
	
	while(1)
	{
		delay_i++;
		/******** �봦�� ********/
		if(delay_i%5==0)
		{
			led_ctrl_func(SYS_LED,delay_i%2);
			printf("sim7600_connt_err_cnt = %d\r\n",sim7600_connt_err_cnt);
			system_clock++;
			sim7600_connt_err_cnt++;								//4g�����쳣����
			sim7600_stat_check();									//4g��·״̬���
			ptu_sys_check();										//PTU״̬���
			if(sim7600_flag.sys_restart_flag==1)
			{
				sim7600_flag.sys_restart_flag = 0;
				NVIC_SystemReset();
			}
			iwdg_feed();
		}	
				
		/***** 4gģ��״̬��� ********/	
		if(sim7600_flag.sim7600_work_flag&&(delay_i%30==0))			//���4gģ��������������ô��ʼע�ᣬ���߷��������ź�
		{		
			if(sim7600_flag.sim7600_register_flag==0)
			{
				sim7600_send_register_data((uint8_t *)&register_info,sizeof(struct REGISTER_INFO_ST));
			}
			else if(sim7600_connt_err_cnt>10)
			{
				sim7600_send_lifesignal_data(system_clock);
			}
		}	
		/***** 4gͨ�ų�ʱ�쳣��� ****/	
		if(sim7600_connt_err_cnt>40)		
		{
			sim7600_flag.reconnect_cnt++;
			printf("sim7600_flag.reconnect_cnt:%d\r\n",sim7600_flag.reconnect_cnt);
			if(sim7600_flag.reconnect_cnt>=3)													//���4gģ����ע���쳣����ô�ص�����
			{
				sim7600_flag.reconnect_cnt = 0;
				POWER_4G_OFF;
				OSTimeDly(4000);
				usart_4g_init();
			}
			led_ctrl_func(ALM_LED,1);
			
			sim7600_flag.sim7600_work_flag = 0;	
			sim7600_connt_err_cnt = 0;		
		}
		
		OSTimeDly(200);
	}
}


