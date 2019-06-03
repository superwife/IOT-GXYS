#ifndef _GPRS_PROTOCOL_APP_H_
#define _GPRS_PROTOCOL_APP_H_

#include <stdint.h>

#define PRO_HEAD_LEN 	8			//Э��̶�����
#define BOARD_NUMB 		DEAL_BOARD	//���嵱ǰ�忨��

enum SIM_4G_CMD
{
	LIFE_SIGNAL_CMD, 				//�����ź�
	PICK_UP_CMD,					//ȡ������
	UMBRELLA_MSG_CMD,				//ɡ��Ϣ
	UNLOCKING_CMD,					//��������
	GIVE_BACK_OVER_CMD,				//�黹�ɹ�����
	ADD_UMBRELLA_CMD,				//��ɡ����
	REGISTER_INFO_CMD,				//ע����Ϣ����
};

enum BOARD_ERR_MESSAGE
{
	CONFIRM_OK, 					//ȷ�ϳɹ�
	CONNECT_ERR,					//���ӹ���
	PICK_UP_ERR,					//ȡ������
	UNLOCKING_ERR,					//��������
	GIVE_BACK_OVER_ERR,				//��ɡ�쳣
	ADD_UMBRELLA_ERR,				//��ɡ�쳣
	REGISTER_ERR,					//ע���쳣
};

enum BOARD_TYPE
{
	  SERVER_BOARD = 0x0f,     		//���������ư�
	  DEAL_BOARD = 0x01,   			//�����
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
