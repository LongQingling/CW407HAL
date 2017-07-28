/****************** Copyright(c) ��������Ƽ���չ���޹�˾ ***********************
* 
*------File Info--------------------------------------------------------------
* File Name: Config.h
* Latest modified Date: �����޸����ڣ�YYYY-MM-DD��
* Latest Version:       �����޶��汾��
* Description:          ���ñ�������Ŀ�Ĺ���0
* compiler:             MDK v5.21
* MCU:                  STM32F429
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

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __CONFIG_H__
#define __CONFIG_H__
/* Includes ----------------------------------------------------------------- */
//#include <stdint.h>
/* Private macros ----------------------------------------------------------- */
/***************************************************************************** 
* ע������ʹ����ع��ܽ�����ʹ��APP��Ĺ��ܺ���,������Ӳ����ʼ��,Ӳ����ʼ������HAL��,Ŀǰû��������
*****************************************************************************/

#define SD_CARD_EN                      1   /* 1:ʹ��SDIO����TF�� */
#define SPI_FLASH_EN                    0   /* 1:ʹ��SPI FLASH disk ע��ʹ�ܺ�Ҫ��CubeMX�������IO�ʹ�SPI1(��ӳ��) */
#define EEPROM_EN                       0   /* 1:ʹ��EEPROM ע��ʹ�ܺ�Ҫ��CubeMX�������IO�ʹ�SPI1(��ӳ��) */
#define ETHERNET_EN                     0   /* 1:ʹ����̫������ */
#define ADC_EN                          0   /* 1:ʹ��ADC�ɼ����� */
#define MODBUS_EN                       0   /* 1:ʹ��Modbus���� */
#define LCD_EN                          0   /* 1:ʹ�ܴ��������� */
#define GPRS_EN                         0   /* ʹ��GPRS���� */
#define SDRAM_EN                        1   /* 1:ʹ��SRAM���� */

#define RELEASE_EN                      0   /* �������� */

/**
  * @brief  FMC SDRAM Bank address
  */   
#define SDRAM_DEVICE_ADDR         ((uint32_t)0x68000000)
#define SDRAM_DEVICE_SIZE         ((uint32_t)0x100000)  /* SRAM device size in MBytes */

/***************************************************************************** 
* RTOS��������
*****************************************************************************/
/* ����ջ��С����(��λ:Byte) */
#define TASK_DEFAULT_STACK_SIZE        128  /* Ĭ�����������ջ��С */
#define TASK_ADC_STACK_SIZE            1024  /* ADC����ջ��С */
#define TASK_GPRS_STACK_SIZE           (1*1024)  /* GPRS����ջ��С */
#define TASK_ETHERNET_STACK_SIZE       128  /* ��̫������ջ��С */

/* �������ȼ����� */
#define TASK_DEFAULT_PRIORITY        osPriorityBelowNormal  /* Ĭ��������������ȼ� */
#define TASK_ADC_PRIORITY            osPriorityAboveNormal  /* ADC�������ȼ� */
#define TASK_GPRS_PRIORITY                osPriorityNormal  /* GPRS�������ȼ� */
#define TASK_EHTERNET_PRIORITY               osPriorityLow  /* ��̫���������ȼ� */

//#define TASK_DEFAULT_PRIORITY        osPriorityNormal  /* Ĭ��������������ȼ� */
//#define TASK_ADC_PRIORITY            osPriorityAboveNormal  /* ADC�������ȼ� */
////#define TASK_GPRS_PRIORITY                osPriorityNormal  /* GPRS�������ȼ� */
//#define TASK_EHTERNET_PRIORITY               osPriorityLow  /* ��̫���������ȼ� */

/***************************************************************************** 
* ADC ������ 8��ģ��ͨ��AIN1-AIN8
*****************************************************************************/

#define ADC_SAMPLE_PERIOD_MS           1    /* ADC��������,��λms */
#define ADC_FIFO_SIZE                  50   /* ADC FIFO��������С(ADC_FIFO_TRIM*2 < ��Χ < 65535)*/
#define ADC_FIFO_TRIM                   5   /* ǰ����޼������� (0~1000̫��û����)*/

/*
ģ��ͨ��AIN1-AIN8�����������
0:ԭʼADCֵ(0-4095); 1, 0~+10V; 2, -10V~+10V; 3, 0~5V; 4, -5V~+5V; 5, 0~+20mA; 6, -20mA~+20mA
*/
#define AIN1_RANGE						1	/* AIN1������� */
#define AIN2_RANGE						1	/* AIN������� */
#define AIN3_RANGE						1	/* AIN������� */
#define AIN4_RANGE						1	/* AIN������� */
#define AIN5_RANGE						5	/* AIN������� */
#define AIN6_RANGE						5	/* AIN������� */
#define AIN7_RANGE						5	/* AIN������� */
#define AIN8_RANGE						5	/* AIN������� */

/***************************************************************************** 
* ��������  
*****************************************************************************/
#define USE_DHCP                        1 /* enable DHCP, if disabled static address is used*/

/*Static IP ADDRESS*/
#define IP_ADDR0   192
#define IP_ADDR1   168
#define IP_ADDR2   0
#define IP_ADDR3   105

/*NETMASK*/
#define NETMASK_ADDR0   255
#define NETMASK_ADDR1   255
#define NETMASK_ADDR2   255
#define NETMASK_ADDR3   0

/*Gateway Address*/
#define GW_ADDR0   192
#define GW_ADDR1   168
#define GW_ADDR2   1
#define GW_ADDR3   1

#define DEST_IP_ADDR0   192
#define DEST_IP_ADDR1   168
#define DEST_IP_ADDR2   1
#define DEST_IP_ADDR3   100

#define DEST_PORT       6000

#define TCP_CLIENT_BUFFER_ZISE      2000    /* �ͻ���Ӧ�ó�����ջ�������С */
#define TCP_CLIENT_NUM              2       /* �ͻ�����Ŀ,��������Զ�̷����� */
/***************************************************************************** 
* ����DEBUG����
*****************************************************************************/
#define DEBUG_COM        COM1   /* ���Դ��� */
#define DEBUG_EN            1   /* ���Թ����ܿ��� */
#define ADC_DEBUG_EN        0   /* ʹ��ADC���Թ��� */
#define GPRS_DEBUG_EN       1   /* ʹ��GPRSģ����Թ��� */

#if (DEBUG_EN==0)
#define ADC_DEBUG_EN        0   /* ��ֹADC���Թ��� */
#define GPRS_DEBUG_EN       0   /* ��ֹGPRSģ����Թ��� */
#endif


#define guiVersion 2017020901   /* ����汾�� */
/* Exported types and constants --------------------------------------------- */
/* Exported types ----------------------------------------------------------- */
/* Exported constants ------------------------------------------------------- */
/* Exported macros ---------------------------------------------------------- */


/* Exported variables ------------------------------------------------------- */
/* Exported functions ------------------------------------------------------- */



#endif /*__CONFIG_H__*/

/************** (C) COPYRIGHT ��������Ƽ���չ���޹�˾ ********END OF FILE****/
