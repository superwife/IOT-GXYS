#ifndef _MOTOR_CTRL_H_
#define _MOTOR_CTRL_H_

#define UNLOCK_TIME_OUT_CNT			8

/************ �����ɡ����ɡ�����ŵ������ *****************/
#define BOOK_MOTOR_PIN	        	GPIO_Pin_7
#define RETURN_MOTOR_PIN			GPIO_Pin_8
#define DOOR_MOTOR_PIN				GPIO_Pin_9
#define DOOR_STAT_CHECK_PIN			GPIO_Pin_0

#define MOTOR_DATA_PORT        		GPIOB
#define MOTOR_DATA_CLK    	 		RCC_AHBPeriph_GPIOB
//��ɡ���
#define BOOK_MOTOR_RUN				GPIO_SetBits(MOTOR_DATA_PORT, BOOK_MOTOR_PIN)
#define BOOK_MOTOR_STOP				GPIO_ResetBits(MOTOR_DATA_PORT, BOOK_MOTOR_PIN)
//��ɡ���
#define RETURN_MOTOR_RUN			GPIO_SetBits(MOTOR_DATA_PORT, RETURN_MOTOR_PIN)
#define RETURN_MOTOR_STOP			GPIO_ResetBits(MOTOR_DATA_PORT, RETURN_MOTOR_PIN)
//������
#define DOOR_MOTOR_RUN				GPIO_SetBits(MOTOR_DATA_PORT, DOOR_MOTOR_PIN)
#define DOOR_MOTOR_STOP				GPIO_ResetBits(MOTOR_DATA_PORT, DOOR_MOTOR_PIN)
//��⿪��״̬
#define DOOR_LOCK_CHECK     		GPIO_ReadInputDataBit(INFRA_RX_POART, BOOK_INFRA_RX_PIN)

/****************** ��������źż���� *********************/
#define BOOK_INFRA_RX_PIN  			GPIO_Pin_4
#define RETURN_INFRA_RX_PIN  		GPIO_Pin_5

#define INFRA_RX_POART    	 		GPIOA
#define INFRA_RX_DATA_CLK    	 	RCC_AHBPeriph_GPIOA

#define BOOK_INFRA_RX_CHECK     	GPIO_ReadInputDataBit(INFRA_RX_POART, BOOK_INFRA_RX_PIN)
#define RETURN_INFRA_RX_CHECK      	GPIO_ReadInputDataBit(INFRA_RX_POART, RETURN_INFRA_RX_PIN)


/******** �������� ********/
typedef enum {
	BOOK_LOCK   = 0x01,				//��ɡ��
	RETURN_LOCK = 0x02,				//��ɡ��
	ADD_LOCK	= 0x03				//��ɡ��
}LOCK_TYPE;
/******** �������� ********/
typedef enum {
	LOCK_ON,						//����
	LOCK_OFF,						//����
}LOCK_ON_OFF_TYPE;



void signal_motro_gpio_init(void);
void unlocking_ctrl(LOCK_TYPE etpye,LOCK_ON_OFF_TYPE CTRL_CMD);

#endif
