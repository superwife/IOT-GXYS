/***************************************************************************//**
  * @file    gprs_protocol_app.c
  * @author  
  * @version V1.0.0
  * @date    2019��5��15��
  * @brief   ������
*******************************************************************************/
#include "stm32f0xx.h"
#include "ucos_ii.h"
#include <string.h>
#include "usart_4g.h"
#include "gprs_protocol_app.h"
#include "motor_ctrl.h"
#include "wtn6170.h"
#include "led_app.h"
#include "spi_flash.h"

/*------------------ extern func ---------------------*/
void  App_Task_4G(void* p_arg);

/*------------------ private func ---------------------*/
static void sim7600_stat_check(void);

/*------------------ public para ---------------------*/
uint16_t sim7600_connt_err_cnt=0;

/*------------------ extern para ---------------------*/
extern struct sim7600_flag_ST sim7600_flag;
extern struct REGISTER_INFO_ST register_info;
extern struct spi_data_info_st spi_info;

/******************************************************************************
* @brief  App_Task_4G
* @note   4Gͨ��������غ���
* @param  ��
* @retval ��
*******************************************************************************/
void  App_Task_4G(void* p_arg)
{
	uint8_t system_clock = 0;
	
	usart_4g_init();

	while(1)
	{
		system_clock++;
		led_ctrl_func(ALM_LED,system_clock%2);
		sim7600_stat_check();
		if(sim7600_flag.sim7600_work_flag&&(system_clock%10==0))	//���4gģ��������������ô��ʼע�ᣬ���߷��������ź�
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
		
//		sim7600_connt_err_cnt++;								//4g�����쳣����
//		if(sim7600_connt_err_cnt>50)		
//		{
//			sim7600_flag.sim7600_work_flag = 0;					
//		}
		OSTimeDly(1000);
	}
}

/************************************************
�������� �� sim7600_stat_check
��    �� �� sim7600״̬���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void sim7600_stat_check(void)
{
	static uint8_t config_cnt = 0;
	if(USART_4G_STATUS_STAT==1)									//4g��ʼ����ɣ����Թ���
	{
		sim7600_flag.sim7600_status_flag = 1;
		config_cnt++;
		if(config_cnt>10)										//�ȴ�10s��ʼ����Ƿ�����4g
		{
			if(sim7600_flag.config_sim7600_flag==0)				//���û����4g����ô����
			{
				sim7600ce_config();	
				printf("4g config over\r\n");
			}
			if(sim7600_flag.sim7600_work_flag==0)
			{
				printf("4g work err\r\n");
				sim7600ce_config();	
			}
		}	
	}
	else if(USART_4G_STATUS_STAT==0)
	{	
		sim7600_flag.sim7600_status_flag = 0;
		printf("4g err\r\n");
	}
	
}
/************************************************
�������� �� sim7600_flaga_deal
��    �� �� sim7600���ݽ��մ�����
��    �� �� ���յ����ݣ����յ����ݳ���
�� �� ֵ �� ��
*************************************************/
void sim7600_flaga_deal(uint8_t *data,uint16_t len)
{
	uint8_t i = 0,crc = 0;
	
#define RECV_PRINT
#ifdef 	RECV_PRINT
	printf("recv data:\r\n");
	for (i=0; i<len; i++)
	{
		printf("%x ",data[i]);
	}
	printf("\r\n");
#endif
	
	if ((*(uint16_t*)(&data[0])==0xaa55)&&(((data[2]<<8)+data[3])==len))
	{	
		for (i=0; i<len-1; i++)
		{
			crc += data[i];
		}
		if (crc == data[len-1])
		{
			sim7600_connt_err_cnt=0;
			switch(data[4])
			{
				case LIFE_SIGNAL_CMD:
					printf("recv LIFE_SIGNAL_CMD\r\n");
					
					break;
				case PICK_UP_CMD:
					if(data[7]==0x01)
					{
						printf("recv PICK_UP_CMD\r\n");
						wtn_send_data(PLEASE_PICK_UP);								//������ʾȡɡ
						sim7600_reply_pickup_data(CONFIRM_OK);						//�ظ���Ϣ
					}
					break;
				case UNLOCKING_CMD:
					if(data[7]==0x01)
					{
						printf("recv UNLOCKING_CMD\r\n");
						unlocking();												//����
						wtn_send_data(BOOK_SUCCESS);								//������ʾ
						sim7600_reply_unlocking_data(CONFIRM_OK);					//ȷ�Ͽ����ɹ�
					}
					break;
				case UMBRELLA_MSG_CMD:												//ȷ�Ϸ������յ�ɡ��Ϣ
					if(data[7]==CONFIRM_OK)
					{
						printf("recv UMBRELLA_MSG_CMD\r\n");
					}
					break;
				case GIVE_BACK_OVER_CMD:
					if(data[7]==0x01)
					{
						printf("recv GIVE_BACK_OVER_CMD\r\n");
						sim7600_reply_return_data(CONFIRM_OK);						//ȷ�Ϲ黹�ɹ�
					}
					break;
				case ADD_UMBRELLA_CMD:
					if(data[7]==0x01)
					{
						printf("recv ADD_UMBRELLA_CMD\r\n");
						sim7600_reply_add_data(CONFIRM_OK);							//ȷ����ɡ�ɹ�
					}
					break;
				case REGISTER_INFO_CMD:
					if(data[7]==CONFIRM_OK)
					{
						printf("recv REGISTER_INFO_CMD\r\n");
						sim7600_flag.sim7600_register_flag = 1;						//ȷ��ע��ɹ�
					}		
					break;		
				default:
					break;
			}
		}
	}
}
/************************************************
�������� �� sim7600_send_lifesignal_data
��    �� �� ���������ź�
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void sim7600_send_lifesignal_data(uint8_t life_cnt)
{
	uint8_t buffer[21] = {0};
	buffer[0] = life_cnt;														//�����źż���
	memmove(buffer+1,register_info.dev_code,sizeof(register_info.dev_code));	//�豸������Ϣ���豸ID
	buffer[19] = spi_info.total_num;											//��ɡ����
	buffer[20] = spi_info.left_num;												//ʣ��ɡ����
	gprs_protocol_data_send(SERVER_BOARD,LIFE_SIGNAL_CMD,buffer,sizeof(buffer));
}
/************************************************
�������� �� sim7600_send_umbrella_data
��    �� �� ����ɡ��Ϣ
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void sim7600_send_umbrella_data(uint8_t *data,uint8_t len)
{
	uint8_t buffer[12] = {0};
	if(data==NULL)
	{
		printf("umbrella_data err\r\n");
	}
	
	buffer[0] = 0x01;						//������Ч
	memmove(buffer+1,data,len);
	gprs_protocol_data_send(SERVER_BOARD,UMBRELLA_MSG_CMD,buffer,len+1);
}
/************************************************
�������� �� sim7600_send_register_data
��    �� �� ����ע����Ϣ
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void sim7600_send_register_data(uint8_t *data,uint8_t len)
{
	if(data==NULL)
	{
		printf("register_data err\r\n");
	}
	gprs_protocol_data_send(SERVER_BOARD,REGISTER_INFO_CMD,data,len);
}
/************************************************
�������� �� sim7600_reply_pickup_data
��    �� �� �ظ�ȡ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void sim7600_reply_pickup_data(uint8_t etype)
{
	uint8_t buffer[1] = {0};
	buffer[0] = etype;						//�ظ�״̬
	gprs_protocol_data_send(SERVER_BOARD,PICK_UP_CMD,buffer,sizeof(buffer));
}
/************************************************
�������� �� sim7600_reply_unlocking_data
��    �� �� �ظ��������
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void sim7600_reply_unlocking_data(uint8_t etype)
{
	uint8_t buffer[1] = {0};
	buffer[0] = etype;						//�ظ�״̬
	gprs_protocol_data_send(SERVER_BOARD,UNLOCKING_CMD,buffer,sizeof(buffer));
}
/************************************************
�������� �� sim7600_reply_return_data
��    �� �� �ظ��黹�ɹ���Ϣ
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void sim7600_reply_return_data(uint8_t etype)
{
	uint8_t buffer[1] = {0};
	buffer[0] = etype;						//�ظ�״̬
	gprs_protocol_data_send(SERVER_BOARD,GIVE_BACK_OVER_CMD,buffer,sizeof(buffer));
}
/************************************************
�������� �� sim7600_reply_add_data
��    �� �� �ظ���ɡ��Ϣ
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void sim7600_reply_add_data(uint8_t etype)
{
	uint8_t buffer[1] = {0};
	buffer[0] = etype;						//������Ч
	gprs_protocol_data_send(SERVER_BOARD,ADD_UMBRELLA_CMD,buffer,sizeof(buffer));
}

/************************************************
�������� �� sim7600_flaga_deal
��    �� �� sim7600���ݽ��մ�����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void gprs_protocol_data_send(uint8_t taget, uint8_t id, uint8_t *data, uint16_t len)	   
{ 
    uint8_t usart_4g_data[128]={0}; 
    uint8_t crc=0, i=0;
    	
    usart_4g_data[0] = 0x55;
    usart_4g_data[1] = 0xAA;
    usart_4g_data[2] = (uint8_t)((len+PRO_HEAD_LEN) >> 8);
    usart_4g_data[3] = len+PRO_HEAD_LEN;
    usart_4g_data[4] = id;
    usart_4g_data[5] = taget;	
    usart_4g_data[6] = BOARD_NUMB;

    for(i=0; i<len; i++)
    {
        usart_4g_data[7+i] = data[i];
    }
    crc=0;
    for(i=0; i<PRO_HEAD_LEN+len-1; i++)
    {
        crc += usart_4g_data[i];
    }
    usart_4g_data[PRO_HEAD_LEN+len-1] = crc;

	usart_4g_send_data(usart_4g_data,len + PRO_HEAD_LEN);
}



