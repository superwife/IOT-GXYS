#ifndef _MOTOR_CTRL_H_
#define _MOTOR_CTRL_H_

#define UNLOCK_TIME_OUT_CNT			5

//�����ɡ�������
#define BOOK_TRAVEL_SW_PIN  		GPIO_Pin_9
#define BOOK_MOTOR_EN_PIN	        GPIO_Pin_8
#define BOOK_DATA_PORT        		GPIOB
#define BOOK_DATA_CLK    	 		RCC_AHBPeriph_GPIOB

#define BOOK_TRAVEL_SW_CHECK     	GPIO_ReadInputDataBit(BOOK_DATA_PORT, BOOK_TRAVEL_SW_PIN)
#define BOOK_MOTOR_RUN				GPIO_SetBits(BOOK_DATA_PORT, BOOK_MOTOR_EN_PIN)
#define BOOK_MOTOR_STOP				GPIO_ResetBits(BOOK_DATA_PORT, BOOK_MOTOR_EN_PIN)

//���廹ɡ���������ض���
#define RETURN_TRAVEL_SW_PIN  		GPIO_Pin_5
#define RETURN_MOTOR_EN_PIN	        GPIO_Pin_4
#define RETURN_DATA_PORT        	GPIOA
#define RETURN_DATA_CLK    	 		RCC_AHBPeriph_GPIOA

#define RETURN_TRAVEL_SW_CHECK      GPIO_ReadInputDataBit(RETURN_DATA_PORT, RETURN_TRAVEL_SW_PIN)
#define RETURN_MOTOR_RUN			GPIO_SetBits(RETURN_DATA_PORT, RETURN_MOTOR_EN_PIN)
#define RETURN_MOTOR_STOP			GPIO_ResetBits(RETURN_DATA_PORT, RETURN_MOTOR_EN_PIN)


/******** �������� ********/
typedef enum {
	BOOK_LOCK,						//��ɡ��
	RETURN_LOCK,					//��ɡ��
	ADD_LOCK						//��ɡ��
}LOCK_TYPE;
/******** �������� ********/
typedef enum {
	LOCK_ON,						//����
	LOCK_OFF,						//����
}LOCK_ON_OFF_TYPE;



void signal_motro_gpio_init(void);
void unlocking_ctrl(LOCK_TYPE etpye,LOCK_ON_OFF_TYPE CTRL_CMD);

#endif
