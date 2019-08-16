#ifndef _GPRS_PROTOCOL_APP_H_
#define _GPRS_PROTOCOL_APP_H_

#include <stdint.h>

#define PRO_HEAD_LEN 	8			//Э��̶�����
#define BOARD_NUMB 		DEAL_BOARD	//���嵱ǰ�忨��

enum SIM_4G_CMD
{
	LIFE_SIGNAL_CMD	 	 = 0x00, 	//�����ź�
	PICK_UP_CMD			 = 0x01,	//ȡ������
	UMBRELLA_MSG_CMD	 = 0x02,	//ɡ��Ϣ
	UNLOCKING_CMD		 = 0x03,	//��������
	GIVE_BACK_OVER_CMD	 = 0x04,	//�黹�ɹ�����
	ADD_UMBRELLA_CMD	 = 0x05,	//��ɡ����
	REGISTER_INFO_CMD	 = 0x06,	//ע����Ϣ����
	SYS_CHECK_CMD		 = 0x07,	//ϵͳ״̬��ѯ
	RESATRT_CMD			 = 0x08,	//ϵͳ����

};

enum BOARD_ERR_MESSAGE
{
	CONFIRM_OK	 	 	 = 0x00, 	//ȷ�ϳɹ�
	CONNECT_ERR	 	 	 = 0x01,	//���ӹ���
	PICK_UP_ERR	 	 	 = 0x02,	//ȡ������
	UNLOCKING_ERR	 	 = 0x03,	//��������
	GIVE_BACK_OVER_ERR	 = 0x04,	//��ɡ�쳣
	ADD_UMBRELLA_ERR	 = 0x05,	//��ɡ�쳣
	REGISTER_ERR	 	 = 0x06,	//ע���쳣
	SYS_CHECK_ERR		 = 0X07,	//ϵͳ��ѯ����
	RESTART_ERR			 = 0X08,	//�����쳣
};

enum BOARD_TYPE
{
	  SERVER_BOARD = 0x0f,     		//���������ư�
	  DEAL_BOARD = 0x01,   			//�����
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
