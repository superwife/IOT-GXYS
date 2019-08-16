/***************************************************************************//**
  * @file    
  * @author  
  * @version V1.0.0
  * @date    2019��6��17��
  * @brief   ptu
*******************************************************************************/
#include "stm32f0xx.h"
#include "ptu_app.h"
#include "led_app.h"
#include <string.h>
#include "usart.h"
#include "spi_flash.h"

#define PTU_HEAD_LEN 	8			//Э��̶�����

struct PTU_DATA_ST ptu_dat;

/*------------------ extern para ----------------------*/
extern struct spi_data_info_st spi_info;
extern struct REGISTER_INFO_ST register_info;
extern struct SYS_TCPIP_INFO_ST sys_tcp_info;

/************************************************
�������� �� ptu_dat_init
��    �� �� ptu���ݳ�ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void ptu_dat_init(void)
{
	ptu_dat.ptu_config_flag  = 0;							//PTU���ñ�־λ
	ptu_dat.ptu_mode_flag	 = 0;							//PTUģʽ��־λ��0����û�н��룬1�������ptuģʽ�����ص����ڴ�ӡ				
	ptu_dat.reset_flag 		 = 0;							//PTU������־λ
}
/************************************************
�������� �� ptu_sys_check
��    �� �� ptu����״̬���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void ptu_sys_check(void)
{
	if(ptu_dat.ptu_config_flag == 1)						//���ptu�������ò���
	{
		update_device_info();
		update_umbrella_info((uint8_t*)&spi_info,sizeof(struct spi_data_info_st));
		reply_write_dev_config_data();
		ptu_dat.reset_flag = 1;
	}
	
	if(ptu_dat.reset_flag==1)
	{
		ptu_dat.reset_flag = 0;
		OSTimeDly(1000);
		printf("NVIC_SystemReset\r\n");
		NVIC_SystemReset();
	}
}

/************************************************
�������� �� USART_Initializes
��    �� �� ���ڳ�ʼ��
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void ptu_data_deal(uint8_t *deal_data,uint8_t len)
{
	uint8_t i,j,crc = 0,data[64]={0};
	uint16_t data_len=0;
	
	led_ctrl_func(ALM_LED,1);
	for(i=0;i<len;i++)                                          		//ѭ����ͷ
	{
		if((deal_data[i]==0x55)&&(deal_data[i+1]==0xaa))
		{
			memmove(data,&deal_data[i],len-i);  
			data_len = data[3]+(data[2]<<8); 
			crc = 0;
			for (j=0; j<data_len-1; j++)
			{
				crc += data[j];
			}
			if (crc == data[data_len-1])                            	//У����ȷ
			{
				switch(data[4])
				{
					case PTU_CONFIG_4G_CMD:
						if(data[7]==0x01)								//ȷ��Ϊ��ȡ����
						{
							reply_read_4g_config_data();
						}
						else if(data[7]==0x02)							//ȷ��Ϊд����
						{
							printf("ip:%d.%d.%d.%d port:%d\r\n",data[8],data[9],data[10],data[11],(data[12] + (data[13]<<8)));
							reply_write_4g_config_data();
						}			
						break;   
					case PTU_CONFIG_DEV_CMD:
						if(data[7]==0x01)								//ȷ��Ϊ��ȡ����
						{
							reply_read_dev_config_data();
						}
						else if(data[7]==0x02)							//ȷ��Ϊд����
						{
							spi_info.total_num = data[8];				
							spi_info.left_num  = data[9];
							
							memmove(register_info.dev_code,&data[10],18);
							memmove(register_info.software_version,&data[28],7);
							memmove(register_info.hardware_version,&data[35],7);
							ptu_dat.ptu_config_flag = 1;
						}
						break;
					case PTU_PRINT_CTRL_CMD:
						if(data[7]==1)
						{
							ptu_dat.ptu_mode_flag = 0;
							reply_usart_ctrl_data(data[7]);
						}
						else if(data[7]==0)
						{
							ptu_dat.ptu_mode_flag = 1;
							reply_usart_ctrl_data(data[7]);
						}
						break;
					case PTU_RESTART_CMD:
						NVIC_SystemReset();						
						break;
					default:
				  
						break;
				}
				return;
			}	  
		}
  }
}
/************************************************
�������� �� reply_usart_ctrl_data
��    �� �� �ظ����ڴ�ӡ����
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void reply_usart_ctrl_data(uint8_t type)
{
	uint8_t buffer[1] = {0};
	buffer[0] = type;

	ptu_data_send(PTU_PRINT_CTRL_CMD,buffer,1);
}
/************************************************
�������� �� reply_write_4g_config_data
��    �� �� �ظ�д������Ϣ
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void reply_write_4g_config_data(void)
{
	uint8_t buffer[1] = {0},i = 0;
	buffer[0] = 0x02;

	ptu_data_send(PTU_CONFIG_4G_CMD,buffer,1);
}
/************************************************
�������� �� reply_read_4g_config_data
��    �� �� �ظ���ȡ������Ϣ
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void reply_read_4g_config_data(void)
{
	uint8_t buffer[64] = {0},i = 0;
	buffer[0] = 0x01;	
	memmove(&buffer[1],(uint8_t*)&sys_tcp_info,sizeof(struct SYS_TCPIP_INFO_ST));	//�豸IP�����Ϣ
	ptu_data_send(PTU_CONFIG_4G_CMD,buffer,sizeof(struct SYS_TCPIP_INFO_ST)+1);
}
/************************************************
�������� �� reply_write_dev_config_data
��    �� �� �ظ�д������Ϣ
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void reply_write_dev_config_data(void)
{
	uint8_t buffer[64] = {0},i = 0;
	buffer[0] = 0x02;

	ptu_data_send(PTU_CONFIG_DEV_CMD,buffer,1);
}
/************************************************
�������� �� reply_read_4g_config_data
��    �� �� �ظ���ȡ�豸������Ϣ
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void reply_read_dev_config_data(void)
{
	uint8_t buffer[64] = {0},i = 0;
	buffer[0] = 0x01;
	buffer[1] = spi_info.total_num;													//��ȡɡ����
	buffer[2] = spi_info.left_num;													//��ȡɡʣ������
	memmove(&buffer[3],(uint8_t*)&register_info,sizeof(struct REGISTER_INFO_ST));	//�豸�����Ϣ
		
	ptu_data_send(PTU_CONFIG_DEV_CMD,buffer,sizeof(struct REGISTER_INFO_ST)+3);
}
/************************************************
�������� �� ptu_data_send
��    �� �� ���ݷ���
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void ptu_data_send(uint8_t id, uint8_t *data, uint16_t len)	   
{ 
    uint8_t usart1_data[128]={0}; 
    uint8_t crc=0, i=0;
    	
    usart1_data[0] = 0x55;
    usart1_data[1] = 0xAA;
    usart1_data[2] = (uint8_t)((len+PTU_HEAD_LEN) >> 8);
    usart1_data[3] = len+PTU_HEAD_LEN;
    usart1_data[4] = id;
    usart1_data[5] = PTU_BOARD;	
    usart1_data[6] = PTU_DEAL_BOARD;

    for(i=0; i<len; i++)
    {
        usart1_data[7+i] = data[i];
    }
    crc=0;
    for(i=0; i<PTU_HEAD_LEN+len-1; i++)
    {
        crc += usart1_data[i];
    }
    usart1_data[PTU_HEAD_LEN+len-1] = crc;

	USART1_SendNByte(usart1_data,len + PTU_HEAD_LEN);
}
