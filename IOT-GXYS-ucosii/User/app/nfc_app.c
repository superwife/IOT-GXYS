/**
  ********************************  STM32F0xx  *********************************
  * @文件名     ： nfc.c
  * @作者       ： 
  * @库版本     ： V1.5.0
  * @文件版本   ： V1.0.0
  * @日期       ： 2019年07月19日
  * @摘要       ： 
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
* @note   4G通信任务相关函数
* @param  无
* @retval 无
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
		/************** nfc识别 ******************/
		if(nfc_flag.start_flag==1)
		{
			spi_nfc_read_func();
			
			if(nfc_flag.update_flag==1)							//nfc相关有更新，说明，nfc还在使用
			{
				nfc_deal_cnt = 0;
			}
			else
			{
				nfc_deal_cnt++;
				if(nfc_deal_cnt>=(20*60))						//如果超过1min仍然没有更新识别，主动退出识别
				{
					nfc_deal_cnt = 0;
					nfc_flag.start_flag = 0;
				}
			}		
		}
		delay_i++;
		/**************** 连续语音播报取伞 **************/
		if(nfc_flag.wtn_pick_up_flag==1)
		{			
			if(wtn_cnt%40==0)
			{
				//printf("wtn PLEASE_PICK_UP\r\n");
				wtn_send_data(PLEASE_PICK_UP);						//语音提示取伞
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
			nfc_flag.update_flag = 0;							//每10清除一次nfc更新标志位
			printf("nfc_flag.start_flag = %d,update_flag = %d,book_back_lock = %d\r\n",nfc_flag.start_flag,nfc_flag.update_flag,nfc_flag.book_back_lock);
		}
			
		OSTimeDly(50);
	}
}
/************************************************
函数名称 ： nfc_flag_init
功    能 ： 初始化flag
参    数 ： 无
返 回 值 ： 无
*************************************************/
void nfc_flag_init(void)
{
	nfc_flag.work_mode  	= 0;	//定义nfc工作模式，1：借伞模式，0：添伞模式
	nfc_flag.start_flag 	= 0;	//nfc开始识别标志位；
	nfc_flag.book_back_lock = 0;	//定义nfc识别锁类型，0：借伞锁，1：还伞锁
	nfc_flag.update_flag    = 0;	//nfc更新标志位，0：代表没有nfc相关更新，1：代表有nfc相关更新
	nfc_flag.wtn_pick_up_flag = 0;	//提示取货标志位
}
