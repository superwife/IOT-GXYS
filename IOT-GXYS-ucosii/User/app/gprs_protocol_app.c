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
#include "nfc_app.h"
#include "spi_175xx.h"

/*------------------ extern func ---------------------*/
void  App_Task_4G(void* p_arg);

/*------------------ private func ---------------------*/


/*------------------ public para ---------------------*/
uint8_t cmd_data[DMA_DATA_LEN]={0};
uint8_t cmd_len = 0;
//uint8_t recv_deal_flag = 0;

/*------------------ extern para ---------------------*/
extern struct sim7600_flag_st sim7600_flag;
extern struct REGISTER_INFO_ST register_info;
extern struct spi_data_info_st spi_info;
extern uint32_t sim7600_connt_err_cnt;
extern struct NFC_DATA_ST nfc_flag;								//定义nfc工作模式，1：借伞模式，0：添伞模式
extern OS_EVENT *sim_4g_sem;									//4g通信信号量
extern struct IR_DATA_ST ir_dat;								//红外检测信号

void usart_data_send(void);

/******************************************************************************
* @brief  App_Task_4G
* @note   4G通信任务相关函数
* @param  无
* @retval 无
*******************************************************************************/
void  App_Task_4G(void* p_arg)
{
	uint8_t err = 0;
	uint8_t deal_data[64] = {0};
	while(1)
	{	
//		if(recv_deal_flag==1)
//		{
			OSSemPend(sim_4g_sem, 0, &err);
			memset(deal_data,0,sizeof(deal_data));
			printf("recv sim data\r\n");
			memmove(deal_data,cmd_data,cmd_len);
			protocol_deal(deal_data);
//			recv_deal_flag = 0;
//		}
		OSTimeDly(200);
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
/****************************************************
* Function Name  : multi_pakg_deal
* Description    : 
* Input          : 		 	
* Output         : None
* Return         : None
*****************************************************/
void multi_pakg_deal(uint8_t *multi_data,uint8_t total_len)
{
	uint8_t pakg_len = 0,i=0,list_len = 0,shift_len = 0;
	uint8_t data[128] = {0};
	
	list_len = total_len;
	for(i=0;i<total_len;i++)
	{	
		if((multi_data[i]==0x55)&&(multi_data[i+1]==0xaa))
		{
			pakg_len = (multi_data[i+2]<<8)+multi_data[i+3];
			if(pakg_len <= list_len)
			{
				memset(data,0,sizeof(data));
				memmove(data,multi_data+shift_len,pakg_len);
				sim7600_data_deal(data,pakg_len);
				list_len = list_len - pakg_len;
				shift_len = shift_len + pakg_len;
				if(list_len<8)
				{
					return;
				}
			}
		}
	}
}
/************************************************
函数名称 ： sim7600_data_deal
功    能 ： sim7600数据接收处理函数
参    数 ： 接收到数据，接收到数据长度
返 回 值 ： 无
*************************************************/
void sim7600_data_deal(uint8_t *data,uint16_t len)
{
	uint8_t i = 0,crc = 0;
	
#define RECV_PRINT
#ifdef 	RECV_PRINT
	printf("recv len :%d data:\r\n",len);
	for (i=0; i<len; i++)
	{
		printf("%x ",data[i]);
	}
	printf("\r\n");
#endif
	led_ctrl_func(ALM_LED,0);
	if ((data[0]==0x55)&&(data[1]==0xaa)&&(((data[2]<<8)+data[3])==len))
	{	
		for (i=0; i<len-1; i++)
		{
			crc += data[i];
		}
		if (crc == data[len-1])
		{
			sim7600_connt_err_cnt=0;
			memset(cmd_data,0,sizeof(cmd_data));
			memmove(cmd_data,data,len);
			cmd_len = len;
			OSSemPost(sim_4g_sem);
//			recv_deal_flag = 1;
		}
	}
}
/************************************************
函数名称 ： protocol_deal
功    能 ： 消息命令处理函数
参    数 ： 接收到数据，接收到数据长度
返 回 值 ： 无
*************************************************/
void protocol_deal(uint8_t *data)
{
	uint16_t i = 0,count = 0;
	static uint8_t cmd_id[8] = {0},lock_type = 0;
	switch(data[4])
	{
		case LIFE_SIGNAL_CMD:
			printf("recv LIFE_SIGNAL_CMD\r\n");
			
			break;
		case PICK_UP_CMD:
			nfc_flag.update_flag = 1;									//nfc使用更新
			if(data[7]==0x01)
			{
				printf("recv PICK_UP_CMD\r\n");
				nfc_flag.work_mode  = 1;									//前门nfc
				nfc_flag.start_flag = 1;									//开始nfc识别
				nfc_flag.book_back_lock = 0;								//借伞锁
				nfc_flag.wtn_pick_up_flag = 1;								//语音提示取伞
				sim7600_reply_pickup_data(CONFIRM_OK,&data[8]);				//回复消息
			}
			break;
		case UNLOCKING_CMD:
			nfc_flag.update_flag = 1;										//nfc使用更新
			if(data[7]==0x01)
			{
				memmove(cmd_id,&data[9],8);
				switch(data[8])
				{
					case 0x01:														//开借伞锁
						lock_type = 0x01;
						printf("recv UNLOCKING_BOOK_LOCK_CMD\r\n");
						nfc_flag.wtn_pick_up_flag = 0;								//停止语音播报取伞
						nfc_flag.work_mode  = 1;									//前门nfc
						nfc_flag.start_flag = 1;									//开始nfc识别
						nfc_flag.book_back_lock = 0;								//借伞锁
						unlocking_ctrl(BOOK_LOCK,LOCK_ON);							//开锁
						ir_dat.ir_tx1 = 0;
						for(i=0;i<UNLOCK_TIME_OUT_CNT*200;i++)
						{
							OSTimeDly(5);
							if(i%600==0)
							{
								wtn_send_data(PUSH_OFF_UMB);						//推出闸机口
							}
							if(ir_dat.ir_tx1==1)
							{
								wtn_send_data(BOOK_SUCCESS);					//语音提示
								sim7600_reply_unlocking_data(CONFIRM_OK,lock_type,cmd_id);//确认开锁成功
								OSTimeDly(500);
								unlocking_ctrl(BOOK_LOCK,LOCK_OFF);				//关锁
								printf("unlock BOOK_LOCK success\r\n");
								nfc_flag.start_flag = 0;						//停止nfc识别
								ir_dat.ir_tx1 = 0;
								return;
							}
						}
						ir_dat.ir_tx1 = 0;
						nfc_flag.start_flag = 0;									//停止nfc识别
						sim7600_reply_unlocking_data(UNLOCKING_ERR,lock_type,cmd_id);//开锁失败
						unlocking_ctrl(BOOK_LOCK,LOCK_OFF);							//关锁
						printf("unlock BOOK_LOCK err\r\n");
						break;
					case 0x02:
						lock_type = 0x02;
						printf("recv UNLOCKING_RETURN_LOCK_CMD\r\n");				//开还伞锁
						nfc_flag.work_mode  = 1;									//前门nfc
						nfc_flag.start_flag = 1;									//开始nfc识别
						nfc_flag.book_back_lock = 1;								//还伞锁
						unlocking_ctrl(RETURN_LOCK,LOCK_ON);						//开锁
						ir_dat.ir_tx2 = 0;
						for(i=0;i<UNLOCK_TIME_OUT_CNT*200;i++)						//超时检测
						{
							OSTimeDly(5);
							
							if(i%600==0)
							{
								wtn_send_data(PUSH_IN_UMB);							//推入闸机口
							}
							if(ir_dat.ir_tx2==1)
							{
								//wtn_send_data(RETURN_SUCCESS);						//语音提示
								sim7600_reply_unlocking_data(CONFIRM_OK,lock_type,cmd_id);//确认开锁成功
								OSTimeDly(200);
								unlocking_ctrl(RETURN_LOCK,LOCK_OFF);				//关锁
								OSTimeDly(200);
								printf("unlock RETURN_LOCK success\r\n");
								ir_dat.ir_tx2 = 0;
								return;
							}
						}
						ir_dat.ir_tx2 = 0;
						sim7600_reply_unlocking_data(UNLOCKING_ERR,lock_type,cmd_id);//开锁失败
						unlocking_ctrl(RETURN_LOCK,LOCK_OFF);						//关锁
						nfc_flag.start_flag = 0;									//停止nfc识别
						printf("unlock RETURN_LOCK err\r\n");
						break;
					case 0x03:
						lock_type = 0x03;
						printf("recv UNLOCKING_ADD_LOCK_CMD\r\n");					//开添伞锁
						nfc_flag.work_mode  = 0;									//后门nfc
						nfc_flag.start_flag = 1;									//开始nfc识别
						for(i=0;i<3;i++)
						{
							unlocking_ctrl(ADD_LOCK,LOCK_ON);						//开锁
							OSTimeDly(200);
							if(DOOR_LOCK_CHECK==1)
							{
								unlocking_ctrl(ADD_LOCK,LOCK_ON);					//开锁
								wtn_send_data(BOOK_SUCCESS);						//语音提示
								sim7600_reply_unlocking_data(CONFIRM_OK,lock_type,cmd_id);//确认开锁成功
								unlocking_ctrl(ADD_LOCK,LOCK_OFF);					//关锁
								return;
							}
						}
						sim7600_reply_unlocking_data(UNLOCKING_ERR,lock_type,cmd_id);		//确认开锁成功
						unlocking_ctrl(ADD_LOCK,LOCK_OFF);							//关锁
						break;
					default:
						
						break;
				}
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
				wtn_send_data(RETURN_SUCCESS);								//语音提示
				sim7600_reply_return_data(CONFIRM_OK,&data[8]);				//确认归还成功
				nfc_flag.start_flag = 0;									//停止nfc识别
			}
			break;
		case ADD_UMBRELLA_CMD:
			nfc_flag.update_flag = 1;										//nfc使用更新
			if(data[7]==0x01)
			{
				printf("recv ADD_UMBRELLA_CMD\r\n");
				sim7600_reply_add_data(CONFIRM_OK,&data[8]);				//确认添伞成功
				make_buzzer_sound();
			}
			break;
		case REGISTER_INFO_CMD:
			if(data[7]==CONFIRM_OK)
			{
				printf("recv REGISTER_INFO_CMD\r\n");
				sim7600_flag.sim7600_register_flag = 1;						//确认注册成功
			}		
			break;	
		case SYS_CHECK_CMD:
			if(data[7]==0x01)
			{
				printf("recv SYS_CHECK_CMD\r\n");
				sim7600_reply_sys_stat_data();
			}
			break;
		case RESATRT_CMD:
			if(data[7]==0x01)
			{
				printf("recv RESATRT_CMD\r\n");
				sim7600_flag.sys_restart_flag = 1;
				sim7600_reply_restart_data();
			}
			break;	
		default:
			break;
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
	uint8_t buffer[1] = {0};
	buffer[0] = life_cnt;														//生命信号计数
	gprs_protocol_data_send(SERVER_BOARD,LIFE_SIGNAL_CMD,buffer,sizeof(buffer));
}
/************************************************
函数名称 ： sim7600_reply_restart_data
功    能 ： 回复复位信号
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600_reply_restart_data(void)
{
	uint8_t buffer[1] = {0};
	buffer[0] = CONFIRM_OK;
	gprs_protocol_data_send(SERVER_BOARD,RESATRT_CMD,buffer,sizeof(buffer));
}
/************************************************
函数名称 ： sim7600_reply_sys_stat_data
功    能 ： 发送系统状态
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600_reply_sys_stat_data(void)
{
	uint8_t buffer[3] = {0};
	buffer[0] = CONFIRM_OK;														//生命信号计数
	buffer[1] = spi_info.total_num;												//总伞数量
	buffer[2] = spi_info.left_num;												//剩余伞数量
	gprs_protocol_data_send(SERVER_BOARD,SYS_CHECK_CMD,buffer,sizeof(buffer));
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
	if(nfc_flag.work_mode == 1)
	{
		if(nfc_flag.book_back_lock == 0)		//借伞锁
		{
			buffer[0] = 0x00;					//伞信息来自借伞锁nfc	
		}
		else
		{
			buffer[0] = 0x01;					//伞信息来自还伞锁nfc	
		}		
	}
	else
	{
		buffer[0] = 0x02;						//伞信息来自添伞nfc
	}
	printf(" NFC :%d\r\n",nfc_flag.work_mode);
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
void sim7600_reply_pickup_data(uint8_t etype,uint8_t *cmd_id)
{
	uint8_t buffer[9] = {0},i=0;
	buffer[0] = etype;						//回复状态
	for(i=0;i<8;i++)
	{
		buffer[i+1] =  cmd_id[i];
	}
	gprs_protocol_data_send(SERVER_BOARD,PICK_UP_CMD,buffer,sizeof(buffer));
}
/************************************************
函数名称 ： sim7600_reply_unlocking_data
功    能 ： 回复开锁结果
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600_reply_unlocking_data(uint8_t etype,uint8_t lock_type,uint8_t *cmd_id)
{
	uint8_t buffer[10] = {0},i=0;
	buffer[0] = etype;						//回复状态
	buffer[1] = lock_type;					//锁类型
	for(i=0;i<8;i++)
	{
		buffer[i+2] =  cmd_id[i];
	}
	gprs_protocol_data_send(SERVER_BOARD,UNLOCKING_CMD,buffer,sizeof(buffer));
}
/************************************************
函数名称 ： sim7600_reply_return_data
功    能 ： 回复归还成功消息
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600_reply_return_data(uint8_t etype,uint8_t *cmd_id)
{
	uint8_t buffer[9] = {0},i=0;
	buffer[0] = etype;						//回复状态
	for(i=0;i<8;i++)
	{
		buffer[i+1] =  cmd_id[i];
	}
	gprs_protocol_data_send(SERVER_BOARD,GIVE_BACK_OVER_CMD,buffer,sizeof(buffer));
}
/************************************************
函数名称 ： sim7600_reply_add_data
功    能 ： 回复增伞信息
参    数 ： 无
返 回 值 ： 无
*************************************************/
void sim7600_reply_add_data(uint8_t etype,uint8_t *cmd_id)
{
	uint8_t buffer[9] = {0},i=0;
	buffer[0] = etype;						//数据有效
	for(i=0;i<8;i++)
	{
		buffer[i+1] =  cmd_id[i];
	}
	gprs_protocol_data_send(SERVER_BOARD,ADD_UMBRELLA_CMD,buffer,sizeof(buffer));
}

/************************************************
函数名称 ： gprs_protocol_data_send
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

/************************************************
函数名称 ： usart_data_send
功    能 ： 发送数据函数
参    数 ： 无
返 回 值 ： 无
*************************************************/
void usart_data_send(void)	   
{
    uint8_t usart2_data[128]={0}; 
    uint8_t crc=0, i=0;
	uint8_t data[1]={0x02},len = 1;
    	
    usart2_data[0] = 0x55;
    usart2_data[1] = 0xAA;
    usart2_data[2] = (uint8_t)((len+PRO_HEAD_LEN) >> 8);
    usart2_data[3] = len+PRO_HEAD_LEN;
    usart2_data[4] = CONNECT_ERR;
    usart2_data[5] = SERVER_BOARD;	
    usart2_data[6] = BOARD_NUMB;

    for(i=0; i<len; i++)
    {
        usart2_data[7+i] = data[i];
    }
    crc=0;
    for(i=0; i<PRO_HEAD_LEN+len-1; i++)
    {
        crc += usart2_data[i];
    }
    usart2_data[PRO_HEAD_LEN+len-1] = crc;

	USART1_SendNByte(usart2_data,len + PRO_HEAD_LEN);
}

