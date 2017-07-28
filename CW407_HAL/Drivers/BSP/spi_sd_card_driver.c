/****************** Copyright(c) ��������Ƽ���չ���޹�˾ ***********************
*  ���ļ�ΪSPI����SD���ĵײ������ļ�
*  ����SPIģ�鼰���IO�ĳ�ʼ����SPI��дSD����дָ������ݵȣ�
*******************************************************************************/
/* Includes ------------------------------------------------------------------*/
#include "stm32f4xx_hal.h"                        // STM32F10x Library DefinitioN
#include "spi_sd_card_driver.h"
#include "ff.h"
#include "diskio.h"
#include "bsp.h"
#include <stdint.h>
#include <stdio.h>

/* Private typedef -----------------------------------------------------------*/
/* Private define ------------------------------------------------------------*/
/* Private macro -------------------------------------------------------------*/
#if SD_DEBUG_EN==0
#define printf(...)
#endif
/* Private variables ---------------------------------------------------------*/
SD_TypeDef SD_Type = SD_TYPE_MMC; //Ĭ��ΪMMC��

/* Global variables ---------------------------------------------------------*/
const unsigned char *SdTypeTab[] =
{
   "SD_TYPE_NO_CARD",
   "SD_TYPE_MMC",
   "SD_TYPE_V1",
   "SD_TYPE_V2",
   "SD_TYPE_V2HC"
};

/* Declaration of extern functions ------------------------------------------*/
extern void Error_Handler(void);
/* Private function prototypes -----------------------------------------------*/

/* Private functions ---------------------------------------------------------*/



/* ************************** ��Ҫ��ֲ�Ľӿں��� ****************************** */
/*******************************************************************************
* Function Name  : SD_SPI_Configuration
* Description    : SPIģ���ʼ�������������IO�ڵĳ�ʼ����
* Input          : None
* Output         : None
* Return         : None
*******************************************************************************/
void SD_SPI_Configuration(void)
{
   return;
}


/*******************************************************************************
* Function Name  : SPI_SetSpeed
* Description    : SPI�����ٶ�Ϊ����
* Input          : uint8_t SpeedSet 
*                  ����ٶ���������0�������ģʽ����0�����ģʽ
*                  SPI_SPEED_HIGH   1
*                  SPI_SPEED_LOW    0
* Output         : None
* Return         : None
*******************************************************************************/
static void SPI_SetSpeed(unsigned char SpeedSet)
{
   hspi1.Instance = SPI1;
   hspi1.Init.Mode = SPI_MODE_MASTER;
   hspi1.Init.Direction = SPI_DIRECTION_2LINES;
   hspi1.Init.DataSize = SPI_DATASIZE_8BIT;
   hspi1.Init.CLKPolarity = SPI_POLARITY_HIGH;
   hspi1.Init.CLKPhase = SPI_PHASE_2EDGE;
   hspi1.Init.NSS = SPI_NSS_SOFT;
// hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_2;
   hspi1.Init.FirstBit = SPI_FIRSTBIT_MSB;
   hspi1.Init.TIMode = SPI_TIMODE_DISABLE;
   hspi1.Init.CRCCalculation = SPI_CRCCALCULATION_DISABLE;
   hspi1.Init.CRCPolynomial = 10;

   //����ٶ���������0�������ģʽ����0�����ģʽ
   if (SpeedSet == SPI_SPEED_LOW)
   {
      hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_256;
   }
   else
   {
      hspi1.Init.BaudRatePrescaler = SPI_BAUDRATEPRESCALER_4;
   }

   if (HAL_SPI_Init(&hspi1) != HAL_OK)
   {
      Error_Handler();
   }

   return;
}

/*******************************************************************************
* Function Name  : SPI_ReadWriteByte
* Description    : SPI��дһ���ֽڣ�������ɺ󷵻ر���ͨѶ��ȡ�����ݣ�
* Input          : uint8_t TxData �����͵���
* Output         : None
* Return         : uint8_t RxData �յ�����
*******************************************************************************/
static unsigned char SPI_ReadWriteByte(unsigned char TxData)
{
   unsigned char RxData = 0;

   HAL_SPI_TransmitReceive(&hspi1,&TxData,&RxData,1,0xffff);

   return RxData;
}

/* ************************ ��Ҫ��ֲ�Ľӿں������� **************************** */


/*******************************************************************************
* Function Name  : SD_WaitReady
* Description    : �ȴ�SD��Ready
* Input          : None
* Output         : None
* Return         : uint8_t 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
unsigned char SD_WaitReady(void)
{
   unsigned char r1;
   unsigned short retry;
   retry = 0;
   do
   {
      r1 = SPI_ReadWriteByte(0xFF);
      if (retry == 0xfffe)
      {
         return 1;
      }
   }
   while (r1 != 0xFF);

   return 0;
}

/*******************************************************************************
* Function Name  : SD_SendCommand
* Description    : ��SD������һ������
* Input          : uint8_t cmd   ���� 
*                  uint32_t arg  �������
*                  uint8_t crc   crcУ��ֵ
* Output         : None
* Return         : uint8_t r1 SD�����ص���Ӧ
*******************************************************************************/
unsigned char SD_SendCommand(unsigned char cmd, unsigned long arg, unsigned char crc)
{
   unsigned char r1;
   unsigned char Retry = 0;

   SD_CS_DISABLE();
   //����8λ������ʱ��SD��
   SPI_ReadWriteByte(0xff);
   //Ƭѡ���õͣ�ѡ��SD��
   SD_CS_ENABLE();

   //����
   SPI_ReadWriteByte(cmd | 0x40);                         //�ֱ�д������
   SPI_ReadWriteByte(arg >> 24);
   SPI_ReadWriteByte(arg >> 16);
   SPI_ReadWriteByte(arg >> 8);
   SPI_ReadWriteByte(arg);
   SPI_ReadWriteByte(crc);

   //�ȴ���Ӧ����ʱ�˳�
   while ((r1 = SPI_ReadWriteByte(0xFF)) == 0xFF)
   {
      Retry++;
      if (Retry > 200)
      {
         break;
      }
   }

   //�ر�Ƭѡ
   SD_CS_DISABLE();
   //�������϶�������8��ʱ�ӣ���SD�����ʣ�µĹ���
   SPI_ReadWriteByte(0xFF);

   //����״ֵ̬
   return r1;
}


/*******************************************************************************
* Function Name  : SD_SendCommand_NoDeassert
* Description    : ��SD������һ������(�����ǲ�ʧ��Ƭѡ�����к������ݴ�����
* Input          : uint8_t cmd   ���� 
*                  uint32_t arg  �������
*                  uint8_t crc   crcУ��ֵ
* Output         : None
* Return         : uint8_t r1 SD�����ص���Ӧ
*******************************************************************************/
unsigned char SD_SendCommand_NoDeassert(unsigned char cmd, unsigned long arg, unsigned char crc)
{
   unsigned char r1;
   unsigned char Retry = 0;
   //????????
   SPI_ReadWriteByte(0xff); //
                            //Ƭѡ���õͣ�ѡ��SD��
   SD_CS_ENABLE();
   //����
   SPI_ReadWriteByte(cmd | 0x40);                         //�ֱ�д������
   SPI_ReadWriteByte(arg >> 24);
   SPI_ReadWriteByte(arg >> 16);
   SPI_ReadWriteByte(arg >> 8);
   SPI_ReadWriteByte(arg);
   SPI_ReadWriteByte(crc);
   //�ȴ���Ӧ����ʱ�˳�
   while ((r1 = SPI_ReadWriteByte(0xFF)) == 0xFF)
   {
      Retry++;
      if (Retry > 200)
      {
         break;
      }
   }
   //������Ӧֵ
   return r1;
}


/*******************************************************************************
* Function Name  : SD_Init
* Description    : ��ʼ��SD��
* Input          : None
* Output         : None
* Return         : uint8_t 
*                  0��NO_ERR
*                  1��TIME_OUT
*                  2��NO_CARD
*                  3��unknown card���޷�ʶ�𿨣�
*                  99��NO_CARD
*******************************************************************************/
uint8_t SD_Init(void)
{
   unsigned short i;      // ����ѭ������
   unsigned char r1;      // ���SD���ķ���ֵ
   unsigned short retry;  // �������г�ʱ����
   unsigned char buff[6];

   SD_Type = SD_TYPE_NO_CARD;  //Ĭ�����޿����ȴ���ʼ���ɹ�

   SD_SPI_Configuration();
   SPI_SetSpeed(SPI_SPEED_LOW);

   //���û�м�⵽�����룬ֱ���˳������ش����־
   if (!SD_DET())
   {
      //return 99;
      return STA_NODISK;  //  FatFS�����־��û�в������
   }

   //SD���ϵ�
   SD_PWR_ON();
   // ����ʱ���ȴ�SD���ϵ����
   for(i=0;i<0xf00;i++);

   //�Ȳ���>74�����壬��SD���Լ���ʼ�����
   for (i = 0; i < 10; i++)
   {
      SPI_ReadWriteByte(0xFF);
   }

   //-----------------SD����λ��idle��ʼ-----------------
   //ѭ����������CMD0��ֱ��SD������0x01,����IDLE״̬
   //��ʱ��ֱ���˳�
   retry = 0;
   do
   {
      //����CMD0����SD������IDLE״̬
      r1 = SD_SendCommand(CMD0, 0, 0x95);
      retry++;
   }
   while ((r1 != 0x01) && (retry < 200));
   //����ѭ���󣬼��ԭ�򣺳�ʼ���ɹ���or ���Գ�ʱ��
   if (retry == 200)
   {
      return 1;   //��ʱ����1
   }
   //-----------------SD����λ��idle����-----------------


   //��ȡ��Ƭ��SD�汾��Ϣ
   r1 = SD_SendCommand_NoDeassert(CMD8, 0x1aa, 0x87);

   //������V2.0���ĳ�ʼ��
   //������Ҫ��ȡOCR���ݣ��ж���SD2.0����SD2.0HC��
   if (r1 == 0x01)
   {
#if SD_DEBUG_EN>0
      printf("CMD8 respond 1.\r\n");
#endif

      //V2.0�Ŀ���CMD8�����ᴫ��4�ֽڵ����ݣ�Ҫ�����ٽ���������
      buff[0] = SPI_ReadWriteByte(0xFF);  //should be 0x00
      buff[1] = SPI_ReadWriteByte(0xFF);  //should be 0x00
      buff[2] = SPI_ReadWriteByte(0xFF);  //should be 0x01
      buff[3] = SPI_ReadWriteByte(0xFF);  //should be 0xAA

      SD_CS_DISABLE();
      //the next 8 clocks
      SPI_ReadWriteByte(0xFF);

      //�жϸÿ��Ƿ�֧��2.7V-3.6V�ĵ�ѹ��Χ
      if (buff[2] == 0x01 && buff[3] == 0xAA)
      {
#if SD_DEBUG_EN>0
         printf("SD card support 2.7~3.6V\r\n");
#endif
         //֧�ֵ�ѹ��Χ�����Բ���
         retry = 0;
         //������ʼ��ָ��CMD55+ACMD41
         do
         {
            r1 = SD_SendCommand(CMD55, 0, 0);
            if (r1 != 0x01)
            {
               return 3;   //�������û�з�����ȷӦ��ֱ���˳��������޷�ʶ��
#if SD_DEBUG_EN>0
               printf("CMD55 no response.\r\n");
#endif
            }
#if SD_DEBUG_EN>0
            printf("CMD55 respond: %d\r\n");
#endif
            r1 = SD_SendCommand(ACMD41, 0x40000000, 0); //��Ϊ��V2�汾����������HCS����1��˵������֧��V2�����ϵĿ���
            if (retry > 200)
            {
               return 1;  //��ʱ����
#if SD_DEBUG_EN>0
               printf("ACMD41 response time out.\r\n");
#endif
            }
#if SD_DEBUG_EN>0
            printf("ACMD41 respond: %d\r\n", r1);
#endif
         }
         while (r1 != 0); //�ȴ�ֱ��CMD41����0����ʾSD����ʼ����ɡ��������ǿ���״̬��

         //��ʼ��ָ�����ɣ���������ȡOCR��Ϣ

         //-----------����SD2.0���汾��ʼ-----------
         r1 = SD_SendCommand_NoDeassert(CMD58, 0, 0);
         if (r1 != 0x00)
         {
            return 3;  //�������û�з�����ȷӦ��ֱ���˳��������޷�ʶ��
#if SD_DEBUG_EN>0
            printf("CMD58 no response.\r\n");
#endif
         }
         //��OCRָ����󣬽�������4�ֽڵ�OCR��Ϣ
         buff[0] = SPI_ReadWriteByte(0xFF);
         buff[1] = SPI_ReadWriteByte(0xFF);
         buff[2] = SPI_ReadWriteByte(0xFF);
         buff[3] = SPI_ReadWriteByte(0xFF);

         //OCR������ɣ�Ƭѡ�ø�
         SD_CS_DISABLE();
         SPI_ReadWriteByte(0xFF);

         //�����յ���OCR�е�bit30λ��CCS����ȷ����ΪSD2.0����SDHC
         //���CCS=1��SDHC   CCS=0��SD2.0
         if (buff[0] & 0x40)    //���CCS
         {
            SD_Type = SD_TYPE_V2HC;
#if SD_DEBUG_EN>0
            printf("This is a SDHC card.\r\n");
#endif
         }
         else
         {
            SD_Type = SD_TYPE_V2;
#if SD_DEBUG_EN>0
            printf("This is a SD V2 card.\r\n");
#endif
         }
         //-----------����SD2.0���汾����-----------
      }

   }
   else if (r1 == 0x05) //�����Ƭ�汾��Ϣ��v1.0�汾�ģ���r1=0x05����������³�ʼ��
   {
#if SD_DEBUG_EN>0
      printf("CMD8 respond: %d\r\n", r1);
#endif
      //���ÿ�����ΪSDV1.0����������⵽ΪMMC�������޸�ΪMMC
      SD_Type = SD_TYPE_V1;

      //�����V1.0����CMD8ָ���û�к�������
      //Ƭѡ�øߣ�������������
      SD_CS_DISABLE();
      //�෢8��CLK����SD������������
      SPI_ReadWriteByte(0xFF);

      //-----------------SD����MMC����ʼ����ʼ-----------------

      //������ʼ��ָ��CMD55+ACMD41
      // �����Ӧ��˵����SD�����ҳ�ʼ�����
      // û�л�Ӧ��˵����MMC�������������Ӧ��ʼ��
      retry = 0;
      do
      {
         //�ȷ�CMD55��Ӧ����0x01���������
         r1 = SD_SendCommand(CMD55, 0, 0);
         if (r1 != 0x01)
         {
            return 3;   //�������û�з�����ȷӦ��ֱ���˳��������޷�ʶ��
#if SD_DEBUG_EN>0
            printf("CMD55 no response.\r\n");
#endif
         }
         //�õ���ȷ��Ӧ�󣬷�ACMD41��Ӧ�õ�����ֵ0x00����������200��
         r1 = SD_SendCommand(ACMD41, 0, 0); //��Ϊ��V1x�汾����������HCS����0��˵������֧��V1x�Ŀ���
         retry++;
      }
      while ((r1 != 0x00) && (retry < 400)); //���ĵ�ֻ��ҪACMD����0����1�Կ�
                                             // �ж��ǳ�ʱ���ǵõ���ȷ��Ӧ
                                             // ���л�Ӧ����SD����û�л�Ӧ����MMC��

      //----------MMC�������ʼ��������ʼ------------
      if (retry == 400)
      {
         retry = 0;
         //����MMC����ʼ�����û�в��ԣ�
         do
         {
            r1 = SD_SendCommand(1, 0, 0);
            retry++;
         }
         while ((r1 != 0x00) && (retry < 400));
         if (retry == 400)
         {
            return 1;   //MMC����ʼ����ʱ
         }
         //д�뿨����
         SD_Type = SD_TYPE_MMC;
#if SD_DEBUG_EN>0
         printf("This is a MMC card.\r\n");
#endif
      }
      //----------MMC�������ʼ����������------------

      //����SPIΪ����ģʽ
//        SPI_SetSpeed(1);

//        SPI_ReadWriteByte(0xFF);

//************************************** �������Ŀ��Ƿ���Ҫ����������С��**********************************************
//��ֹCRCУ�飨Ĭ�Ϲر�CRC�����Բ���Ҫ���ã�
      /*
      r1 = SD_SendCommand(CMD59, 0, 0x01);
      if(r1 != 0x00)
      {
          return r1;  //������󣬷���r1
      }
      */
      //����Sector Size
      r1 = SD_SendCommand(CMD16, 512, 0xff);
      if (r1 != 0x00)
      {
         return 3;   //�������û�з�����ȷӦ��ֱ���˳��������޷�ʶ��
      }
//************************************************************************************
//-----------------SD����MMC����ʼ������-----------------

   } //SD��ΪV1.0�汾�ĳ�ʼ������

   r1 = SD_SendCommand(CMD16, 512, 0xff);
   if (r1 != 0x00)
   {
      return 3;   //�������û�з�����ȷӦ��ֱ���˳��������޷�ʶ��
   }

   //����SPIΪ����ģʽ
   SPI_SetSpeed(1);
#if SD_DEBUG_EN>0
   printf("SD card initialize successfully.\r\nSPI set high speed.\r\n");
#endif
   return 0;  //��ʼ���ɹ�
}

/*******************************************************************************
* Function Name  : SD_ReceiveData
* Description    : ��SD���ж���ָ�����ȵ����ݣ������ڸ���λ��
* Input          : uint8_t *data(��Ŷ������ݵ��ڴ�>len)
*                  uint16_t len(���ݳ��ȣ�
*                  uint8_t release(������ɺ��Ƿ��ͷ�����CS�ø� 0�����ͷ� 1���ͷţ�
* Output         : None
* Return         : uint8_t 
*                  0��NO_ERR
*                  other��������Ϣ
*******************************************************************************/
unsigned char SD_ReceiveData(unsigned char *data, unsigned short len, unsigned char release)
{
   unsigned short retry;
   unsigned char r1;

   // ����һ�δ���
   SD_CS_ENABLE();
   //�ȴ�SD������������ʼ����0xFE
   retry = 0;
   do
   {
      r1 = SPI_ReadWriteByte(0xFF);
      retry++;
      if (retry > 2000)  //2000�εȴ���û��Ӧ���˳�����
      {
         SD_CS_DISABLE();
         return 1;
      }
   }
   while (r1 != 0xFE);
   //��ʼ��������
   while (len--)
   {
      *data = SPI_ReadWriteByte(0xFF);
      data++;
   }
   //������2��αCRC��dummy CRC��
   SPI_ReadWriteByte(0xFF);
   SPI_ReadWriteByte(0xFF);
   //�����ͷ����ߣ���CS�ø�
   if (release == RELEASE)
   {
      //�������
      SD_CS_DISABLE();
      SPI_ReadWriteByte(0xFF);
   }

   return 0;
}



/*******************************************************************************
* Function Name  : SD_GetCID
* Description    : ��ȡSD����CID��Ϣ��������������Ϣ
* Input          : uint8_t *cid_data(���CID���ڴ棬����16Byte��
* Output         : None
* Return         : uint8_t 
*                  0��NO_ERR
*                  1��TIME_OUT
*                  other��������Ϣ
*******************************************************************************/
unsigned char SD_GetCID(unsigned char *cid_data)
{
   unsigned char r1;

   //��CMD10�����CID
   r1 = SD_SendCommand(CMD10, 0, 0xFF);
   if (r1 != 0x00)
   {
      return r1;  //û������ȷӦ�����˳�������
   }
   //����16���ֽڵ�����
   SD_ReceiveData(cid_data, 16, RELEASE);

   return 0;
}


/*******************************************************************************
* Function Name  : SD_GetCSD
* Description    : ��ȡSD����CSD��Ϣ�������������ٶ���Ϣ
* Input          : uint8_t *cid_data(���CID���ڴ棬����16Byte��
* Output         : None
* Return         : uint8_t 
*                  0��NO_ERR
*                  1��TIME_OUT
*                  other��������Ϣ
*******************************************************************************/
unsigned char SD_GetCSD(unsigned char *csd_data)
{
   unsigned char r1;

   //��CMD9�����CSD
   r1 = SD_SendCommand(CMD9, 0, 0xFF);
   if (r1 != 0x00)
   {
      return r1;  //û������ȷӦ�����˳�������
   }
   //����16���ֽڵ�����
   SD_ReceiveData(csd_data, 16, RELEASE);

   return 0;
}


/*******************************************************************************
* Function Name  : SD_GetCapacity
* Description    : ��ȡSD��������,���ص�����������(512B/����)
* Input          : None
* Output         : None
* Return         : uint32_t capacity 
*                   0�� ȡ�������� 
*******************************************************************************/
unsigned long SD_GetCapacity(void)
{
   unsigned char csd[16];
   unsigned long Capacity;
//    unsigned char r1;
//    unsigned short i;
//    unsigned short temp;

   //ȡCSD��Ϣ������ڼ��������0
   if (SD_GetCSD(csd) != 0)
   {
      return 0;
   }

   //���ΪSDHC�����������淽ʽ����
   if ((csd[0] & 0xC0) == 0x40) //CSD V2.0
   {
      Capacity = (csd[9] + ((uint32_t)csd[8] << 8) + ((uint32_t)(csd[7] & 63) << 16) + 1) << 10;
   }
   else    //CSD V1.0
   {
      //�������Ϊ���ϰ汾
      ////////////formula of the capacity///////////////
      //
      //  memory capacity = BLOCKNR * BLOCK_LEN
      //
      //	BLOCKNR = (C_SIZE + 1)* MULT
      //
      //           C_SIZE_MULT+2
      //	MULT = 2
      //
      //               READ_BL_LEN
      //	BLOCK_LEN = 2
      /**********************************************/
      /*
      //C_SIZE
      i = csd[6] & 0x03;
      i <<= 8;
      i += csd[7];
      i <<= 2;
      i += ((csd[8] & 0xc0) >> 6);

      //C_SIZE_MULT
      r1 = csd[9] & 0x03;
      r1 <<= 1;
      r1 += ((csd[10] & 0x80) >> 7);

      //BLOCKNR
      r1 += 2;
      temp = 1;
      while (r1)
      {
          temp *= 2;
          r1--;
      }
      Capacity = ((uint32_t)(i + 1)) * ((uint32_t)temp);//BLOCKNR

      // READ_BL_LEN
      i = csd[5] & 0x0f;

      //BLOCK_LEN
      temp = 1;
      while (i)
      {
          temp *= 2;
          i--;
      }
      //The final result
      Capacity *= (uint32_t)temp;//����/�ֽ�
      //Capacity /= 512;
      */
      //                                                        ( C_SIZE +1 ) * MULT                                                    * BLOCK_LEN
      //Capacity = (((csd[6]&0x03)<<10) | (csd[7]<<2) | ((csd[8]&0xC0)>>6) + 1) * (1 << ((((csd[9]&0x03)<<1) | ((csd[10]&0x80)>>7)) + 2)) << (csd[5] & 0x0f);//����/�ֽ�
      Capacity = (((csd[6] & 0x03) << 10) | (csd[7] << 2) | ((csd[8] & 0xC0) >> 6) + 1) * (1 << ((((csd[9] & 0x03) << 1) | ((csd[10] & 0x80) >> 7)) + 2)); //����/����
   }
   return (uint32_t)Capacity;
}

/*******************************************************************************
* Function Name  : SD_ReadSingleBlock
* Description    : ��SD����һ��block
* Input          : uint32_t sector ȡ��ַ��sectorֵ���������ַ�� 
*                  uint8_t *buffer ���ݴ洢��ַ����С����512byte�� 
* Output         : None
* Return         : uint8_t r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
unsigned char SD_ReadSingleBlock(unsigned long sector, unsigned char *buffer)
{
   uint8_t r1;

   //����Ϊ����ģʽ
//    SPI_SetSpeed(SPI_SPEED_HIGH);

   if (SD_Type < SD_TYPE_V2HC)
   {
      //�������SDHC����sector��ַת��byte��ַ
      sector = sector << 9;
   }

   r1 = SD_SendCommand(CMD17, sector, 0); //������

   if (r1 != 0x00)
   {
      return r1;
   }

   r1 = SD_ReceiveData(buffer, 512, RELEASE);
   if (r1 != 0)
   {
      return r1;   //�����ݳ���
   }
   else
   {
      return 0;
   }
}


/*******************************************************************************
* Function Name  : SD_WriteSingleBlock
* Description    : д��SD����һ��block
* Input          : uint32_t sector ������ַ��sectorֵ���������ַ�� 
*                  uint8_t *buffer ���ݴ洢��ַ����С����512byte�� 
* Output         : None
* Return         : uint8_t r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
unsigned char SD_WriteSingleBlock(unsigned long sector, const unsigned char *data)
{
   unsigned char r1;
   unsigned short i;
   unsigned short retry;

   //����Ϊ����ģʽ
//    SPI_SetSpeed(SPI_SPEED_HIGH);

   //�������SDHC����������sector��ַ������ת����byte��ַ
   if (SD_Type < SD_TYPE_V2HC)
   {
      sector = sector << 9;
   }

   r1 = SD_SendCommand(CMD24, sector, 0x00);
   if (r1 != 0x00)
   {
      return r1;  //Ӧ����ȷ��ֱ�ӷ���
   }

   //��ʼ׼�����ݴ���
   SD_CS_ENABLE();
   //�ȷ�3�������ݣ��ȴ�SD��׼����
   SPI_ReadWriteByte(0xff);
   SPI_ReadWriteByte(0xff);
   SPI_ReadWriteByte(0xff);
   //����ʼ����0xFE
   SPI_ReadWriteByte(0xFE);

   //��һ��sector������
   for (i = 0; i < 512; i++)
   {
      SPI_ReadWriteByte(*data++);
   }
   //��2��Byte��dummy CRC
   SPI_ReadWriteByte(0xff);
   SPI_ReadWriteByte(0xff);

   //�ȴ�SD��Ӧ��
   r1 = SPI_ReadWriteByte(0xff);
   if ((r1 & 0x1F) != 0x05)
   {
      SD_CS_DISABLE();
      return r1;
   }

   //�ȴ��������
   retry = 0;
   while (!SPI_ReadWriteByte(0xff))
   {
      retry++;
      if (retry > 0xfffe)        //�����ʱ��д��û����ɣ������˳�
      {
         SD_CS_DISABLE();
         return 1;           //д�볬ʱ����1
      }
   }

   //д����ɣ�Ƭѡ��1
   SD_CS_DISABLE();
   SPI_ReadWriteByte(0xff);

   return 0;
}



/*******************************************************************************
* Function Name  : SD_ReadMultiBlocks
* Description    : ��SD���Ķ��block
* Input          : uint32_t sector ȡ��ַ��sectorֵ���������ַ�� 
*                  uint8_t *buffer ���ݴ洢��ַ����С����512byte��
*                  uint8_t count ������count��block
* Output         : None
* Return         : uint8_t r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
unsigned char SD_ReadMultiBlocks(unsigned long sector, unsigned char *buffer, unsigned char count)
{
   unsigned char r1;

   //����Ϊ����ģʽ
//    SPI_SetSpeed(SPI_SPEED_HIGH);

   if (SD_Type < SD_TYPE_V2HC)
   {
      //�������SDHC����sector��ַת��byte��ַ
      sector = sector << 9;
   }
   //SD_WaitReady();
   //�����������
   r1 = SD_SendCommand(CMD18, sector, 0); //������
   if (r1 != 0x00)
   {
      return r1;
   }
   //��ʼ��������
   do
   {
      if (SD_ReceiveData(buffer, 512, NO_RELEASE) != 0x00)
      {
         break;
      }
      buffer += 512;
   }
   while (--count);

   //ȫ��������ϣ�����ֹͣ����
   SD_SendCommand(CMD12, 0, 0);
   //�ͷ�����
   SD_CS_DISABLE();
   SPI_ReadWriteByte(0xFF);

   if (count != 0)
   {
      return count;   //���û�д��꣬����ʣ�����
   }
   else
   {
      return 0;
   }
}


/*******************************************************************************
* Function Name  : SD_WriteMultiBlocks
* Description    : д��SD����N��block
* Input          : uint32_t sector ������ַ��sectorֵ���������ַ�� 
*                  uint8_t *buffer ���ݴ洢��ַ����С����512byte��
*                  uint8_t count д���block��Ŀ
* Output         : None
* Return         : uint8_t r1 
*                   0�� �ɹ�
*                   other��ʧ��
*******************************************************************************/
unsigned char SD_WriteMultiBlocks(unsigned long sector, const unsigned char *data, unsigned char count)
{
   unsigned char r1;
   unsigned short i;

   //����Ϊ����ģʽ
//    SPI_SetSpeed(SPI_SPEED_HIGH);

   //�������SDHC����������sector��ַ������ת����byte��ַ
   if (SD_Type < SD_TYPE_V2HC)
   {
      sector = sector << 9;
   }
   //���Ŀ�꿨����MMC��������ACMD23ָ��ʹ��Ԥ����
   if (SD_Type != SD_TYPE_MMC)
   {
      r1 = SD_SendCommand(ACMD23, count, 0x00);
   }
   //�����д��ָ��
   r1 = SD_SendCommand(CMD25, sector, 0x00);
   if (r1 != 0x00)
   {
      return r1;  //Ӧ����ȷ��ֱ�ӷ���
   }

   //��ʼ׼�����ݴ���
   SD_CS_ENABLE();
   //�ȷ�3�������ݣ��ȴ�SD��׼����
   SPI_ReadWriteByte(0xff);
   SPI_ReadWriteByte(0xff);

   //--------������N��sectorд���ѭ������
   do
   {
      //����ʼ����0xFC �����Ƕ��д��
      SPI_ReadWriteByte(0xFC);

      //��һ��sector������
      for (i = 0; i < 512; i++)
      {
         SPI_ReadWriteByte(*data++);
      }
      //��2��Byte��dummy CRC
      SPI_ReadWriteByte(0xff);
      SPI_ReadWriteByte(0xff);

      //�ȴ�SD��Ӧ��
      r1 = SPI_ReadWriteByte(0xff);
      if ((r1 & 0x1F) != 0x05)
      {
         SD_CS_DISABLE();    //���Ӧ��Ϊ��������������ֱ���˳�
         return r1;
      }

      //�ȴ�SD��д�����
      if (SD_WaitReady() == 1)
      {
         SD_CS_DISABLE();    //�ȴ�SD��д����ɳ�ʱ��ֱ���˳�����
         return 1;
      }

      //��sector���ݴ������
   }
   while (--count);

   //��������������0xFD
   r1 = SPI_ReadWriteByte(0xFD);
   if (r1 == 0x00)
   {
      count =  0xfe;
   }

   if (SD_WaitReady())
   {
      return 3; //�ȴ���ʱ
   }

   //д����ɣ�Ƭѡ��1
   SD_CS_DISABLE();
   SPI_ReadWriteByte(0xff);

   return count;   //����countֵ�����д����count=0������count=1
}

/************** (C) COPYRIGHT ��������Ƽ���չ���޹�˾ ********END OF FILE****/

