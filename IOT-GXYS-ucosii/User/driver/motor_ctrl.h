#ifndef _MOTOR_CTRL_H_
#define _MOTOR_CTRL_H_

#define SINGLE_MOTOR_CHECK_PIN  	GPIO_Pin_9
#define SINGLE_MOTOR_EN_PIN	        GPIO_Pin_8

#define SINGLE_MOTOR_PORT        	GPIOB
#define SINGLE_MOTOR_CLK    	 	RCC_AHBPeriph_GPIOB

#define SINGLE_MOTOR_RUN_CHECK     	GPIO_ReadInputDataBit(SINGLE_MOTOR_PORT, SINGLE_MOTOR_CHECK_PIN)

#define SINGLE_MOTOR_RUN_CMD		GPIO_SetBits(SINGLE_MOTOR_PORT, SINGLE_MOTOR_EN_PIN)
#define SINGLE_MOTOR_STOP_CMD		GPIO_ResetBits(SINGLE_MOTOR_PORT, SINGLE_MOTOR_EN_PIN)

void signal_motro_gpio_init(void);
void unlocking(void);

#endif
