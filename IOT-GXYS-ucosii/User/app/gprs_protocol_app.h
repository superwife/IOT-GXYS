#ifndef _GPRS_PROTOCOL_APP_H_
#define _GPRS_PROTOCOL_APP_H_

#include <stdint.h>

#define PRO_HEAD_LEN 	8			//协议固定长度
#define BOARD_NUMB 		DEAL_BOARD	//定义当前板卡号

enum SIM_4G_CMD
{
	LIFE_SIGNAL_CMD	 	 = 0x00, 	//生命信号
	PICK_UP_CMD			 = 0x01,	//取货命令
	UMBRELLA_MSG_CMD	 = 0x02,	//伞信息
	UNLOCKING_CMD		 = 0x03,	//开锁命令
	GIVE_BACK_OVER_CMD	 = 0x04,	//归还成功命令
	ADD_UMBRELLA_CMD	 = 0x05,	//增伞命令
	REGISTER_INFO_CMD	 = 0x06,	//注册信息命令
	SYS_CHECK_CMD		 = 0x07,	//系统状态查询
	RESATRT_CMD			 = 0x08,	//系统重启

};

enum BOARD_ERR_MESSAGE
{
	CONFIRM_OK	 	 	 = 0x00, 	//确认成功
	CONNECT_ERR	 	 	 = 0x01,	//连接故障
	PICK_UP_ERR	 	 	 = 0x02,	//取货故障
	UNLOCKING_ERR	 	 = 0x03,	//开锁故障
	GIVE_BACK_OVER_ERR	 = 0x04,	//还伞异常
	ADD_UMBRELLA_ERR	 = 0x05,	//添伞异常
	REGISTER_ERR	 	 = 0x06,	//注册异常
	SYS_CHECK_ERR		 = 0X07,	//系统查询故障
	RESTART_ERR			 = 0X08,	//重启异常
};

enum BOARD_TYPE
{
	  SERVER_BOARD = 0x0f,     		//服务器控制板
	  DEAL_BOARD = 0x01,   			//处理板
};

void multi_pakg_deal(uint8_t *multi_data,uint8_t total_len);
void sim7600_data_deal(uint8_t *data,uint16_t len);
void gprs_protocol_data_send(uint8_t taget, uint8_t id, uint8_t *data, uint16_t len);
void sim7600_reply_pickup_data(uint8_t etype,uint8_t *cmd_id);
void sim7600_reply_unlocking_data(uint8_t etype,uint8_t lock_type,uint8_t *cmd_id);
void sim7600_reply_return_data(uint8_t etype,uint8_t *cmd_id);
void sim7600_reply_add_data(uint8_t etype,uint8_t *cmd_id);
void sim7600_reply_sys_stat_data(void);
void sim7600_reply_restart_data(void);

void sim7600_send_umbrella_data(uint8_t *data,uint8_t len);
void sim7600_send_lifesignal_data(uint8_t life_cnt);
void sim7600_send_register_data(uint8_t *data,uint8_t len);

void sim7600_stat_check(void);
void protocol_deal(uint8_t *data);



#endif
