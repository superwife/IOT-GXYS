/**
  ********************************  STM32F0xx  *********************************
  * @�ļ���     �� nfc.c
  * @����       �� 
  * @��汾     �� V1.5.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2019��07��19��
  * @ժҪ       �� 
  ******************************************************************************/
/*----------------------------------------------------------------------------*/
#include <string.h>
#include "stm32f0xx.h"
#include "ucos_ii.h"
#include "usart.h"
#include "spi_175xx.h"
#include "usart_4g.h"
#include "gprs_protocol_app.h"
#include "nfc_app.h"
#include "wtn6170.h"

struct NFC_DATA_ST nfc_flag;							

/******************************************************************************
* @brief  App_Task_4G
* @note   4Gͨ��������غ���
* @param  ��
* @retval ��
*******************************************************************************/
void  App_Task_NFC(void* p_arg)
{
	uint8_t wtn_count = 0;
	uint16_t delay_i = 0,wtn_cnt = 0;
	uint16_t nfc_deal_cnt = 0;
	
	nfc_flag_init();
	spi_nfc_init();
	
	while(1)
	{		
		/************** nfcʶ�� ******************/
		if(nfc_flag.start_flag==1)
		{
			spi_nfc_read_func();
			
			if(nfc_flag.update_flag==1)							//nfc����и��£�˵����nfc����ʹ��
			{
				nfc_deal_cnt = 0;
			}
			else
			{
				nfc_deal_cnt++;
				if(nfc_deal_cnt>=(20*60))						//�������1min��Ȼû�и���ʶ�������˳�ʶ��
				{
					nfc_deal_cnt = 0;
					nfc_flag.start_flag = 0;
				}
			}		
		}
		delay_i++;
		/**************** ������������ȡɡ **************/
		if(nfc_flag.wtn_pick_up_flag==1)
		{			
			if(wtn_cnt%40==0)
			{
				//printf("wtn PLEASE_PICK_UP\r\n");
				wtn_send_data(PLEASE_PICK_UP);						//������ʾȡɡ
				wtn_count++;
				if(wtn_count>=10)
				{
					nfc_flag.wtn_pick_up_flag = 0;
				}
			}
			wtn_cnt++;
		}
		else
		{
			wtn_cnt = 0;
			wtn_count = 0;
		}
		
		if(delay_i%200==0)
		{
			nfc_flag.update_flag = 0;							//ÿ10���һ��nfc���±�־λ
			printf("nfc_flag.start_flag = %d,update_flag = %d,book_back_lock = %d\r\n",nfc_flag.start_flag,nfc_flag.update_flag,nfc_flag.book_back_lock);
		}
			
		OSTimeDly(50);
	}
}
/************************************************
�������� �� nfc_flag_init
��    �� �� ��ʼ��flag
��    �� �� ��
�� �� ֵ �� ��
*************************************************/
void nfc_flag_init(void)
{
	nfc_flag.work_mode  	= 0;	//����nfc����ģʽ��1����ɡģʽ��0����ɡģʽ
	nfc_flag.start_flag 	= 0;	//nfc��ʼʶ���־λ��
	nfc_flag.book_back_lock = 0;	//����nfcʶ�������ͣ�0����ɡ����1����ɡ��
	nfc_flag.update_flag    = 0;	//nfc���±�־λ��0������û��nfc��ظ��£�1��������nfc��ظ���
	nfc_flag.wtn_pick_up_flag = 0;	//��ʾȡ����־λ
}
