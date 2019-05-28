/**
  ********************************  STM32F0xx  *********************************
  * @�ļ���     �� sflash.c
  * @����       �� 
  * @��汾     �� V1.5.0
  * @�ļ��汾   �� V1.0.0
  * @����       �� 2019��05��09��
  * @ժҪ       �� SFLASH����Դ�ļ�(��W25Q80Ϊ��)
  ******************************************************************************/
/************************************ ˵�� *************************************
1�������ӿ�֧��оƬ���ͣ�
   ԭ����֧������������SPIͨ�ŵ�FLASH
   ��оƬ�������Բ���(��:��ѹ���ٶȵ�), ���ܲ���оƬ��֧��

2��W25Q80��Ϣ��
ҳ(Page) ------ 256Byte
����(Sector) -- 4KByte ------ (����16ҳ)
��(Block) ----- 64KByte ----- (����16����)

W25X80оƬ������1MB (8Mbit)
          ҳ����16page
        ��������16sector,1sector = 16page
          ������16block,1block = 16sector

3����д������
�� ------------ һ������һҳ(256B)
д ------------ ҳ
���� ---------- �������顢����оƬ
********************************************************************************/
/*----------------------------------------------------------------------------
  ������־:
  2016-06-04 V1.0.0:��ʼ�汾
  ----------------------------------------------------------------------------*/
/* ������ͷ�ļ� --------------------------------------------------------------*/
#include "w25qxx.h"
#include "spi_flash.h"


/************************************************
�������� �� SFLASH_WriteEnable
��    �� �� SPI_FLASHдʹ�ܣ���WEL��λ
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
static void SFLASH_WriteEnable(void)
{
  SFLASH_CS_ENABLE;                              //ʹ������
  SPI_WriteReadByte(SFLASH_WRITE_ENABLE);        //��дʹ�ܡ�ָ��
  SFLASH_CS_DISABLE;                             //ʧ������
}

/************************************************
�������� �� SFLASH_WriteDisable
��    �� �� SPI_FLASHд��ֹ,��WEL����
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
static void SFLASH_WriteDisable(void)
{
  SFLASH_CS_ENABLE;                              //ʹ������
  SPI_WriteReadByte(SFLASH_WRITE_DISABLE);       //��дʧ�ܡ�ָ��
  SFLASH_CS_DISABLE;                             //ʧ������
}

//�Ĵ�������˵��
//BIT7 6   5   4   3   2   1   0
//SPR  RV  TB  BP2 BP1 BP0 WEL BUSY
//SPR:Ĭ��0,״̬�Ĵ�������λ,���WPʹ��
//TB,BP2,BP1,BP0:FLASH����д��������
//WEL:дʹ������
//BUSY:æ���λ(1,æ;0,����)
//Ĭ��:0x00
/************************************************
�������� �� SFLASH_ReadSR
��    �� �� ��ȡSFLASH״̬�Ĵ���
��    �� �� ��
�� �� ֵ �� Byte --- ��ȡ�ֽ�
��    �� �� strongerHuang
*************************************************/
uint8_t SFLASH_ReadSR(void)
{
  uint8_t data_tmp;
  SFLASH_CS_ENABLE;                              //ʹ������
  SPI_WriteReadByte(SFLASH_READ_STATUS_REG);     //����״̬�Ĵ�����ָ��
  data_tmp = SPI_WriteReadByte(0xFF);            //��ȡһ���ֽ�
  SFLASH_CS_DISABLE;                             //ʧ������
  return data_tmp;
}

/************************************************
�������� �� SFLASH_WriteSR
��    �� �� дSFLASH״̬�Ĵ���
��    �� �� SR --- д״̬�Ĵ�������
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SFLASH_WriteSR(uint8_t SR)
{
  SFLASH_WriteEnable();                          //дʹ��
  SPI_WriteReadByte(SFLASH_WRITE_STATUS_REG);    //��д״̬�Ĵ�����ָ��
  SPI_WriteReadByte(SR);                         //д��һ���ֽ�
  SFLASH_CS_DISABLE;                             //ʧ������
}

/************************************************
�������� �� SFLASH_ReadNByte
��    �� �� ��ReadAddr��ַ��ʼ������ȡSFLASH��nByte
��    �� �� pBuffer ---- ���ݴ洢���׵�ַ
            ReadAddr --- Ҫ��ȡSFLASH Flash���׵�ַ��ַ
            nByte ------ Ҫ��ȡ���ֽ���(���65535B = 64K ��)

�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SFLASH_ReadNByte(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t nByte)
{
  SFLASH_CS_ENABLE;                              //ʹ������
  SPI_WriteReadByte(SFLASH_READ_DATA);           //�������ݡ�ָ��
  SPI_WriteReadByte((uint8_t)((ReadAddr)>>16));  //����24bit��ַ
  SPI_WriteReadByte((uint8_t)((ReadAddr)>>8));
  SPI_WriteReadByte((uint8_t)ReadAddr);

  while(nByte--)                                 //ѭ������
  {
    *pBuffer = SPI_WriteReadByte(0xFF);
    pBuffer++;
  }

  SFLASH_CS_DISABLE;                             //ʧ������
}

/************************************************
�������� �� SFLASH_FastReadNByte
��    �� �� ��ReadAddr��ַ��ʼ�������ٶ�ȡSFLASH��nByte
��    �� �� pBuffer ---- ���ݴ洢���׵�ַ
            ReadAddr --- Ҫ��ȡSFLASH Flash���׵�ַ��ַ
            nByte ------ Ҫ��ȡ���ֽ���(���65535B = 64K ��)
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SFLASH_FastReadNByte(uint8_t* pBuffer, uint32_t ReadAddr, uint16_t nByte)
{
  SFLASH_CS_ENABLE;                              //ʹ������
  SPI_WriteReadByte(SFLASH_FAST_READ);           //��������ݡ�ָ��
  SPI_WriteReadByte((uint8_t)((ReadAddr)>>16));  //����24bit��ַ
  SPI_WriteReadByte((uint8_t)((ReadAddr)>>8));
  SPI_WriteReadByte((uint8_t)ReadAddr);
  SPI_WriteReadByte(0xFF);                       //�ȴ�8��ʱ��

  while(nByte--)                                 //ѭ������
  {
    *pBuffer = SPI_WriteReadByte(0xFF);
    pBuffer++;
  }

  SFLASH_CS_DISABLE;                             //ʧ������
}

/************************************************
�������� �� SFLASH_WritePage
��    �� �� ��SFLASH��д������1ҳ(256���ֽ�)������
��    �� �� pBuffer ----- д���������׵�ַ
            WriteAddr --- Ҫд��Flash�ĵ�ַ
            nByte ------- Ҫд����ֽ���(���1ҳ)
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SFLASH_WritePage(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t nByte)
{
  SFLASH_WriteEnable();                          //дʹ��

  SFLASH_CS_ENABLE;                              //ʹ������
  SPI_WriteReadByte(SFLASH_WRITE_PAGE);          //��ҳ��̡�ָ��
  SPI_WriteReadByte((uint8_t)((WriteAddr)>>16)); //����24bit��ַ
  SPI_WriteReadByte((uint8_t)((WriteAddr)>>8));
  SPI_WriteReadByte((uint8_t)WriteAddr);

  while (nByte--)
  {
    SPI_WriteReadByte(*pBuffer);
    pBuffer++;
  }

  SFLASH_CS_DISABLE;
  SFLASH_WaitForNoBusy();                        //�ȴ����У��ȴ�д�������
}

/************************************************
�������� �� SFLASH_WriteNoCheck
��    �� �� �޼���дSFLASH
            ����ȷ����д�ĵ�ַ��Χ�ڵ�����ȫ��Ϊ0xFF,�����ڷ�0xFF��д������ݽ�ʧ��!
            �����Զ���ҳ����
            ��ָ����ַ��ʼд��ָ�����ȵ�����,����Ҫȷ����ַ��Խ��!
��    �� �� pBuffer ----- д���������׵�ַ
            WriteAddr --- Ҫд��Flash�ĵ�ַ
            nByte ------- Ҫд����ֽ���
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SFLASH_WriteNoCheck(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t nByte)
{
  uint16_t PageRemain = 256 - WriteAddr%256;     //��ҳʣ���д���ֽ���

  if(nByte <= PageRemain)
    PageRemain = nByte;                          //������256���ֽ�

  while(1)
  {
    SFLASH_WritePage(pBuffer, WriteAddr, PageRemain);
    if(nByte == PageRemain)                      //д�����
      break;
    else                                         //д��δ����
    {
      pBuffer += PageRemain;                     //��һҳд������
      WriteAddr += PageRemain;                   //��һҳд�����ݵ�ַ
      nByte -= PageRemain;                       //��д���ֽ����ݼ�

      if(nByte > 256)
        PageRemain = 256;                        //��д��1ҳ(256�ֽ�)������
      else
        PageRemain = nByte;                      //��д������1ҳ(256�ֽ�)������
    }
  }
}


/************************************************
�������� �� SFLASH_WriteNByte
��    �� �� ��ReadAddr��ַ��ʼ����д��nByte��SFLASH��
��    �� �� pBuffer ----- д���������׵�ַ
            WriteAddr --- Ҫд��Flash�ĵ�ַ
            nByte ------- Ҫд����ֽ���(���65535B = 64K ��)
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SFLASH_WriteNByte(uint8_t* pBuffer, uint32_t WriteAddr, uint16_t nByte)
{
  //static uint8_t SectorBuf[4096];              //����buf
  uint8_t SectorBuf[4096];                	 	//����buf
  uint32_t SecPos;                               //����λ��
  uint16_t SecOff;                               //����ƫ��
  uint16_t SecRemain;                            //ʣ������
  uint16_t i;

  SecPos = WriteAddr/4096;                       //��ַ��������(0~511)
  SecOff = WriteAddr%4096;                       //��ַ����������ƫ��
  SecRemain = 4096-SecOff;                       //��ַ��������ʣ���ֽ���(������С4096B=4KB)

  if(nByte <= SecRemain)
    SecRemain = nByte;                           //д�����ݴ�С < ʣ��ռ��С (��ʣ��ռ乻������Щ����)

  while(1)
  {
    /* ��1����У�� */
    SFLASH_ReadNByte(SectorBuf, SecPos*4096, 4096);        //������������������
    for(i=0; i<SecRemain; i++)                             //У������,�Ƿ���Ҫ����
    {
      if(SectorBuf[SecOff + i] != 0xFF)                    //�洢���ݲ�Ϊ0xFF ����Ҫ����
        break;
    }
    if(i < SecRemain)                                      //��Ҫ����
    {
      SFLASH_EraseSector(SecPos);                          //����������
      for(i=0; i<SecRemain; i++)                           //����д�������(��1��ʱ����д������������ʣ��Ŀռ�)
      {
        SectorBuf[SecOff + i] = pBuffer[i];
      }
      SFLASH_WriteNoCheck(SectorBuf, SecPos*4096, 4096);   //д���������������� = ������ + ��д�����ݣ�
    }
    else
      SFLASH_WriteNoCheck(pBuffer, WriteAddr, SecRemain);  //����Ҫ����,ֱ��д������ʣ��ռ�

    if(nByte == SecRemain)                       //д�����
    {
      SFLASH_WriteDisable();                     //дʧ��, �˳�д
      break;
    }
    else                                         //д��δ����
    {
      SecPos++;                                  //������ַ��1
      SecOff = 0;                                //ƫ��λ�ù���
      pBuffer += SecRemain;                      //ָ��ƫ��
      WriteAddr += SecRemain;                    //д��ַƫ��
      nByte -= SecRemain;                        //��д���ֽ����ݼ�
      if(nByte > 4096)
        SecRemain = 4096;                        //��д��1����(4096�ֽ�)������
      else
        SecRemain = nByte;                       //��д������1����(4096�ֽ�)������
    }
  }
}

/************************************************
�������� �� SFLASH_WaitForNoBusy
��    �� �� �ȴ���æ
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SFLASH_WaitForNoBusy(void)
{
  while((SFLASH_ReadSR()&0x01)==0x01);           //�ȴ�BUSYλ���
}

/************************************************
�������� �� SFLASH_EraseBlock
��    �� �� ������
            ��������Ҫһ��ʱ��
��    �� �� BlockAddr --- ���ַ 0~16
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SFLASH_EraseBlock(uint32_t BlockAddr)
{
  BlockAddr *= 65536;                            //���׵�ַ
  SFLASH_WriteEnable();                          //дʹ��
  SFLASH_WaitForNoBusy();
  SFLASH_CS_ENABLE;                              //ʹ������
  SPI_WriteReadByte(SFLASH_ERASE_BLOCK);         //�������顷ָ��
  SPI_WriteReadByte((uint8_t)((BlockAddr)>>16)); //������ַ
  SPI_WriteReadByte((uint8_t)((BlockAddr)>>8));
  SPI_WriteReadByte((uint8_t)BlockAddr);
  SFLASH_CS_DISABLE;

  SFLASH_WaitForNoBusy();                        //�ȴ��������
}

/************************************************
�������� �� SFLASH_EraseSector
��    �� �� ��������
��    �� �� SectorAddr --- ������ַ 0~511
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SFLASH_EraseSector(uint32_t SectorAddr)
{
  SectorAddr *= 4096;                            //�����׵�ַ
  SFLASH_WriteEnable();                          //дʹ��
  SFLASH_WaitForNoBusy();
  SFLASH_CS_ENABLE;                              //ʹ������
  SPI_WriteReadByte(SFLASH_ERASE_SECTOR);        //������������ָ��
  SPI_WriteReadByte((uint8_t)((SectorAddr)>>16));//������ַ
  SPI_WriteReadByte((uint8_t)((SectorAddr)>>8));
  SPI_WriteReadByte((uint8_t)SectorAddr);
  SFLASH_CS_DISABLE;

  SFLASH_WaitForNoBusy();                        //�ȴ��������
}

/************************************************
�������� �� SFLASH_EraseChip
��    �� �� ��������оƬ(��Ƭ����ʱ��ϳ�)
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SFLASH_EraseChip(void)
{
  SFLASH_WriteEnable();                          //дʹ��
  SFLASH_WaitForNoBusy();
  SFLASH_CS_ENABLE;                              //ʹ������
  SPI_WriteReadByte(SFLASH_ERASE_CHIP);          //������оƬ��ָ��
  SFLASH_CS_DISABLE;

  SFLASH_WaitForNoBusy();                        //�ȴ�оƬ��������
}

/************************************************
�������� �� SFLASH_PowerDown
��    �� �� �������ģʽ
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SFLASH_PowerDown(void)
{
  SFLASH_CS_ENABLE;                              //ʹ������
  SPI_WriteReadByte(SFLASH_POWER_DOWN);          //�����硷ָ��
  SFLASH_CS_DISABLE;                             //ʧ������
}

/************************************************
�������� �� SFLASH_WAKEUP
��    �� �� ���绽��
��    �� �� ��
�� �� ֵ �� ��
��    �� �� strongerHuang
*************************************************/
void SFLASH_WAKEUP(void)
{
  SFLASH_CS_ENABLE;                              //ʹ������
  SPI_WriteReadByte(SFLASH_RELEASE_POWER_DOWN);  //�����绽�ѡ�ָ��
  SFLASH_CS_DISABLE;                             //ʧ������
}

/************************************************
�������� �� SFLASH_ReadID
��    �� �� ��ȡоƬID SFLASH��ID��0XEF14��
��    �� �� ��
�� �� ֵ �� ID --- 16λID��
��    �� �� strongerHuang
*************************************************/
uint16_t SFLASH_ReadID(void)
{
  uint16_t ID = 0;
  SFLASH_CS_ENABLE;                              //ʹ������

  SPI_WriteReadByte(SFLASH_DEVICE_ID);           //���豸ID��ָ��
  SPI_WriteReadByte(0x00);
  SPI_WriteReadByte(0x00);
  SPI_WriteReadByte(0x00);

  ID |= SPI_WriteReadByte(0xFF)<<8;              //��ȡID
  ID |= SPI_WriteReadByte(0xFF);
  SFLASH_CS_DISABLE;                             //ʧ������
  return ID;
}


/**** Copyright (C)2016 strongerHuang. All Rights Reserved **** END OF FILE ****/
