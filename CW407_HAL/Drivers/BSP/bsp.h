/****************** Copyright(c) ��������Ƽ���չ���޹�˾ ***********************/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __BSP_H__
#define __BSP_H__
/* Includes ----------------------------------------------------------------- */
#include <stdint.h>
#include <stdio.h>
#include <string.h>
#include "stm32f4xx_hal.h"
#include "cmsis_os.h"

#include "config.h"

/* ͨ��ȡ��ע�ͻ������ע�͵ķ�ʽ�����Ƿ�����ײ�����ģ�� */
#include "main.h"
#include "app.h"
#include "bsp_uart_fifo.h"
#include "bsp_user_lib.h"
#include "bsp_port.h"
#include "bsp_cpu_flash.h"
#include "param.h"

#if SPI_FLASH_EN
#include "bsp_spi_flash.h"
#endif

#if EEPROM_EN
#include "bsp_eeprom_24xx.h"
#endif

#if LCD_EN
#include "lcd.h"
#endif

#if MODBUS_EN
#include "bsp_modbus.h"
#endif

#if GPRS_EN
#include "gprs.h"
#endif

#if SD_CARD_EN

#endif

/* Private macros ----------------------------------------------------------- */

/* Exported types and constants --------------------------------------------- */
/* Exported types ----------------------------------------------------------- */
/* Exported constants ------------------------------------------------------- */
extern const uint8_t BitMask[8];    //λ��������
extern const uint8_t NotBitMask[8];//λ��������
/* Exported macros ---------------------------------------------------------- */

//λ������,ʵ��51���Ƶ�GPIO���ƹ���
//����ʵ��˼��,�ο�<<CM3Ȩ��ָ��>>������(87ҳ~92ҳ).
//IO�ڲ����궨��
#define BITBAND(addr, bitnum) ((addr & 0xF0000000)+0x2000000+((addr &0xFFFFF)<<5)+(bitnum<<2)) 
#define MEM_ADDR(addr)  *((volatile unsigned long  *)(addr)) 
#define BIT_ADDR(addr, bitnum)   MEM_ADDR(BITBAND(addr, bitnum)) 

 
//IO�ڲ���,ֻ�Ե�һ��IO��!
//ȷ��n��ֵС��16!
//#define PAout(n)   BIT_ADDR(GPIOA_ODR_Addr,n)  //��� 

#define  USE_FreeRTOS      1

#if USE_FreeRTOS == 1
	#include "FreeRTOS.h"
	#include "task.h"
	#define DISABLE_INT()    taskENTER_CRITICAL()
	#define ENABLE_INT()     taskEXIT_CRITICAL()
#else
	/* ����ȫ���жϵĺ� */
	#define ENABLE_INT()	__set_PRIMASK(0)	/* ʹ��ȫ���ж� */
	#define DISABLE_INT()	__set_PRIMASK(1)	/* ��ֹȫ���ж� */
#endif

/* Exported variables ------------------------------------------------------- */
/* Exported functions ------------------------------------------------------- */
void bsp_DelayUS(uint16_t cnt);
void DEBUG_PrintfString(char *pString);
void DEBUG_PrintfBytes(char *pBuf, uint16_t length);


#endif /*__BSP_H__*/

/************** (C) COPYRIGHT ��������Ƽ���չ���޹�˾ ********END OF FILE****/
