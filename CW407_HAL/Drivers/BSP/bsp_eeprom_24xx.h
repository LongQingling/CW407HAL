/*
*********************************************************************************************************
*
*	ģ������ : ����EEPROM 24xx02����ģ��
*	�ļ����� : bsp_eeprom_24xx.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	�޸ļ�¼ :
*		�汾��  ����       ����    ˵��
*		v1.0    2012-10-12 armfly  ST�̼���汾 V2.1.0
*
*	Copyright (C), 2012-2013, ���������� www.armfly.com
*
*********************************************************************************************************
*/
#include "stm32f1xx_hal.h"

#ifndef _BSP_EEPROM_24XX_H
#define _BSP_EEPROM_24XX_H

//#define AT24C02
//#define AT24C04
#define AT24C64

#ifdef AT24C02
	#define EE_MODEL_NAME		"AT24C02"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		8			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				256			/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		1			/* ��ַ�ֽڸ��� */
	#define EE_ADDR_A8			0			/* ��ַ�ֽڵĸ�8bit�������ֽ� */
#endif

#ifdef AT24C04
	#define EE_MODEL_NAME		"AT24C04"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		8			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				512			/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		1			/* ��ַ�ֽڸ��� */
	#define EE_ADDR_A8			1			/* ��ַ�ֽڵĸ�8bit�����ֽ� */
#endif

#ifdef AT24C64
	#define EE_MODEL_NAME		"AT24C64"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		32			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				(8*1024)	/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		2			/* ��ַ�ֽڸ��� */
	#define EE_ADDR_A8			0			/* ��ַ�ֽڵĸ�8bit�������ֽ� */
#endif

#ifdef AT24C128
	#define EE_MODEL_NAME		"AT24C128"
	#define EE_DEV_ADDR			0xA0		/* �豸��ַ */
	#define EE_PAGE_SIZE		64			/* ҳ���С(�ֽ�) */
	#define EE_SIZE				(16*1024)	/* ������(�ֽ�) */
	#define EE_ADDR_BYTES		2			/* ��ַ�ֽڸ��� */
	#define EE_ADDR_A8			0			/* ��ַ�ֽڵĸ�8bit�������ֽ� */
#endif

#if (EE_ADDR_BYTES == 2)
	#define EE_ADD_SIZE   I2C_MEMADD_SIZE_16BIT
#else
	#define EE_ADD_SIZE   I2C_MEMADD_SIZE_8BIT
#endif

HAL_StatusTypeDef EEPROM_WriteData(uint16_t _usMemAddress, uint8_t *_pucData, uint16_t _usSize);
HAL_StatusTypeDef EEPROM_ReadData(uint16_t _usMemAddress, uint8_t *_pucData, uint16_t _usSize);

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
