/***************************************************************************//**
  * @file    gprs_protocol_app.c
  * @author  
  * @version V1.0.0
  * @date    2019年5月15号
  * @brief   主程序
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
* @note   4G通信任务相关函数
* @param  无
* @retval 无
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
		if(sim7600_flag.sim7600_work_flag&&(system_clock%10==0))	//如果4g模块正常工作，那么开始注册，或者发送生命信号
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
		
//		sim7600_connt_err_cnt++;								//4g连接异常处理
//		if(sim7600_connt_err_cnt>50)		
//		{
//			sim7600_flag.sim7600_work_flag = 0;					
//		}
		OSTimeDly(1000);
	}
}

/************************************************
函数名称 ： sim7600_stat_check
功    能 ： sim7600状态检测
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600_stat_check(void)
{
	static uint8_t config_cnt = 0;
	if(USART_4G_STATUS_STAT==1)									//4g初始化完成，可以工作
	{
		sim7600_flag.sim7600_status_flag = 1;
		config_cnt++;
		if(config_cnt>10)										//等待10s后开始检测是否配置4g
		{
			if(sim7600_flag.config_sim7600_flag==0)				//如果没配置4g，那么配置
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
函数名称 ： sim7600_flaga_deal
功    能 ： sim7600数据接收处理函数
参    数 ： 接收到数据，接收到数据长度
返 回 值 ： 无
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
						wtn_send_data(PLEASE_PICK_UP);								//语音提示取伞
						sim7600_reply_pickup_data(CONFIRM_OK);						//回复消息
					}
					break;
				case UNLOCKING_CMD:
					if(data[7]==0x01)
					{
						printf("recv UNLOCKING_CMD\r\n");
						unlocking();												//开锁
						wtn_send_data(BOOK_SUCCESS);								//语音提示
						sim7600_reply_unlocking_data(CONFIRM_OK);					//确认开锁成功
					}
					break;
				case UMBRELLA_MSG_CMD:												//确认服务器收到伞消息
					if(data[7]==CONFIRM_OK)
					{
						printf("recv UMBRELLA_MSG_CMD\r\n");
					}
					break;
				case GIVE_BACK_OVER_CMD:
					if(data[7]==0x01)
					{
						printf("recv GIVE_BACK_OVER_CMD\r\n");
						sim7600_reply_return_data(CONFIRM_OK);						//确认归还成功
					}
					break;
				case ADD_UMBRELLA_CMD:
					if(data[7]==0x01)
					{
						printf("recv ADD_UMBRELLA_CMD\r\n");
						sim7600_reply_add_data(CONFIRM_OK);							//确认添伞成功
					}
					break;
				case REGISTER_INFO_CMD:
					if(data[7]==CONFIRM_OK)
					{
						printf("recv REGISTER_INFO_CMD\r\n");
						sim7600_flag.sim7600_register_flag = 1;						//确认注册成功
					}		
					break;		
				default:
					break;
			}
		}
	}
}
/************************************************
函数名称 ： sim7600_send_lifesignal_data
功    能 ： 发送生命信号
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600_send_lifesignal_data(uint8_t life_cnt)
{
	uint8_t buffer[21] = {0};
	buffer[0] = life_cnt;														//生命信号计数
	memmove(buffer+1,register_info.dev_code,sizeof(register_info.dev_code));	//设备编码信息，设备ID
	buffer[19] = spi_info.total_num;											//总伞数量
	buffer[20] = spi_info.left_num;												//剩余伞数量
	gprs_protocol_data_send(SERVER_BOARD,LIFE_SIGNAL_CMD,buffer,sizeof(buffer));
}
/************************************************
函数名称 ： sim7600_send_umbrella_data
功    能 ： 发送伞信息
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600_send_umbrella_data(uint8_t *data,uint8_t len)
{
	uint8_t buffer[12] = {0};
	if(data==NULL)
	{
		printf("umbrella_data err\r\n");
	}
	
	buffer[0] = 0x01;						//数据有效
	memmove(buffer+1,data,len);
	gprs_protocol_data_send(SERVER_BOARD,UMBRELLA_MSG_CMD,buffer,len+1);
}
/************************************************
函数名称 ： sim7600_send_register_data
功    能 ： 发送注册信息
参    数 ： 无
返 回 值 ： 无
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
函数名称 ： sim7600_reply_pickup_data
功    能 ： 回复取货
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600_reply_pickup_data(uint8_t etype)
{
	uint8_t buffer[1] = {0};
	buffer[0] = etype;						//回复状态
	gprs_protocol_data_send(SERVER_BOARD,PICK_UP_CMD,buffer,sizeof(buffer));
}
/************************************************
函数名称 ： sim7600_reply_unlocking_data
功    能 ： 回复开锁结果
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600_reply_unlocking_data(uint8_t etype)
{
	uint8_t buffer[1] = {0};
	buffer[0] = etype;						//回复状态
	gprs_protocol_data_send(SERVER_BOARD,UNLOCKING_CMD,buffer,sizeof(buffer));
}
/************************************************
函数名称 ： sim7600_reply_return_data
功    能 ： 回复归还成功消息
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600_reply_return_data(uint8_t etype)
{
	uint8_t buffer[1] = {0};
	buffer[0] = etype;						//回复状态
	gprs_protocol_data_send(SERVER_BOARD,GIVE_BACK_OVER_CMD,buffer,sizeof(buffer));
}
/************************************************
函数名称 ： sim7600_reply_add_data
功    能 ： 回复增伞信息
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600_reply_add_data(uint8_t etype)
{
	uint8_t buffer[1] = {0};
	buffer[0] = etype;						//数据有效
	gprs_protocol_data_send(SERVER_BOARD,ADD_UMBRELLA_CMD,buffer,sizeof(buffer));
}

/************************************************
函数名称 ： sim7600_flaga_deal
功    能 ： sim7600数据接收处理函数
参    数 ： 无
返 回 值 ： 无
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



