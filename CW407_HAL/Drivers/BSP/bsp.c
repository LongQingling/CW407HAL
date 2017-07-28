/****************** Copyright(c) ��������Ƽ���չ���޹�˾ ***********************
* 
*------File Info--------------------------------------------------------------
* File Name: 			bsp.c
* Latest modified Date: �����޸����ڣ�YYYY-MM-DD��
* Latest Version:       �����޶��汾��
* Description:          ��Ӳ������صĶ���ͺ���
* compiler:             MDK v5.20
* MCU:                  STM32F407
* Oscillator Crystal Frequency:    8MHz
*-----------------------------------------------------------------------------
* Created By:    �ļ��Ĵ���������
* Created date:  �ļ��Ĵ������ڣ�YYYY-MM-DD��
* Version:       �ļ�����ʱ�İ汾��
* Descriptions:  �ļ�����ʱ�ļ�Ҫ����
*
*-----------------------------------------------------------------------------
* Modified by:   �޸��˵�����
* Modified date: �ļ����޸����ڣ�YYYY-MM-DD��
* Version:       �ļ��޶��İ汾��
* Description:   �ļ��޶��ļ�Ҫ����
*
******************************************************************************/


/* Includes -----------------------------------------------------------------*/
#include "stdint.h"                    /* data type definitions and macro*/
#include "stm32f4xx_hal.h"
#include "main.h"
#include "bsp.h"

//������Ӧ�ĵײ�����,ͨ��Config.h�ļ�������Ӧ�Ĺ���


#if SPI_FLASH_EN
#include "bsp_spi_flash.c"
#endif

#if SD_CARD_EN
//#include "spi_sd_card_driver.c"
#endif

#if EEPROM_EN
#include "bsp_eeprom_24xx.c"
#endif

#if MODBUS_EN
#include "bsp_modbus.c"
#endif

/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
#define COM_RS485	huart3
/* Private macro ------------------------------------------------------------*/
/* Private constants ------------------------------------------------------- */
/* Private variables --------------------------------------------------------*/

/* Private function prototypes ----------------------------------------------*/
/* Private functions --------------------------------------------------------*/

/* Forward Declaration of local functions -----------------------------------*/
/* Declaration of extern functions ------------------------------------------*/
/* Global variables ---------------------------------------------------------*/
extern const uint8_t BitMask[8] = {0x01,0x02,0x04,0x10,0x20,0x40,0x80};    //λ��������
extern const uint8_t NotBitMask[8] = {0xfe,0xfd,0xfb,0xf7,0xef,0xdf,0xbf,0x7f};//λ��������


/** @defgroup ETH_Private_Functions
  * @{
  */

/**
  * @brief  Delay some us
  * @param  cnt 1~65535
  * @retval None
  */
void bsp_DelayUS(uint16_t cnt)
{
    unsigned char i;
    while (cnt--)
    {
        i = 11;
        while (i--);
    }
}

/**
  * @brief  ����ʱ��ӡ������Ϣ
  * @param pString: ��Ҫ��ӡ���ַ���                   
  * @retval none                
  */
void DEBUG_PrintfString(char *pString)
{
//  comSendString(DEBUG_COM, pString);
    HAL_UART_Transmit(&huart1,(uint8_t*)pString,strlen(pString),0xffff);  //����ʱ���ܻ��õ�
//  HAL_UART_Transmit_IT(&huart1,pString,strlen(pString));  //����ʱ���ܻ��õ�
}

/**
  * @brief  ����ʱ��ӡ������Ϣ
  * @param  pBuf: ��Ҫ��ӡ������ 
  *         length ��Ҫ��ӡ�����ݳ��� 
  * @retval none                
  */
void DEBUG_PrintfBytes(char *pBuf, uint16_t length)
{
//  comSendBuf(DEBUG_COM, (uint8_t *)pBuf, length);
    HAL_UART_Transmit_IT(&huart1,pBuf,length);  //����ʱ���ܻ��õ�
}


/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
/************** (C) COPYRIGHT ��������Ƽ���չ���޹�˾ ********END OF FILE****/
