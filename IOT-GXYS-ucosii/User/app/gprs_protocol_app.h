#ifndef _GPRS_PROTOCOL_APP_H_
#define _GPRS_PROTOCOL_APP_H_

#include <stdint.h>

#define PRO_HEAD_LEN 	8			//协议固定长度
#define BOARD_NUMB 		DEAL_BOARD	//定义当前板卡号

enum SIM_4G_CMD
{
	LIFE_SIGNAL_CMD, 				//生命信号
	PICK_UP_CMD,					//取货命令
	UMBRELLA_MSG_CMD,				//伞信息
	UNLOCKING_CMD,					//开锁命令
	GIVE_BACK_OVER_CMD,				//归还成功命令
	ADD_UMBRELLA_CMD,				//增伞命令
	REGISTER_INFO_CMD,				//注册信息命令
};

enum BOARD_ERR_MESSAGE
{
	CONFIRM_OK, 					//确认成功
	CONNECT_ERR,					//连接故障
	PICK_UP_ERR,					//取货故障
	UNLOCKING_ERR,					//开锁故障
	GIVE_BACK_OVER_ERR,				//还伞异常
	ADD_UMBRELLA_ERR,				//添伞异常
	REGISTER_ERR,					//注册异常
};

enum BOARD_TYPE
{
	  SERVER_BOARD = 0x0f,     		//服务器控制板
	  DEAL_BOARD = 0x01,   			//处理板
};


void sim7600_data_deal(uint8_t *data,uint16_t len);
void gprs_protocol_data_send(uint8_t taget, uint8_t id, uint8_t *data, uint16_t len);
void sim7600_reply_pickup_data(uint8_t etype);
void sim7600_reply_unlocking_data(uint8_t etype);
void sim7600_reply_return_data(uint8_t etype);
void sim7600_reply_add_data(uint8_t etype);

void sim7600_send_umbrella_data(uint8_t *data,uint8_t len);
void sim7600_send_lifesignal_data(uint8_t life_cnt);
void sim7600_send_register_data(uint8_t *data,uint8_t len);

void sim7600_stat_check(void);
void protocol_deal(uint8_t *data);



#endif
