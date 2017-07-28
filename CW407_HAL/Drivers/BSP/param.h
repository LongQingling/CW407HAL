/*
*********************************************************************************************************
*
*	ģ������ : Ӧ�ó������ģ��
*	�ļ����� : param.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2016-2026, ��������Ƽ���չ���޹�˾
*
*********************************************************************************************************
*/

#ifndef __PARAM_H
#define __PARAM_H

#include <stdint.h>

#define PARAM_SAVE_TO_FLASH		/* �����洢��CPU�ڲ�Flash */

#ifdef PARAM_SAVE_TO_FLASH
#if 0  /* ������*/
/* �洢����������Ϣ */
#define PARAM_START_ADDR		((int32_t)0x807f800)    /* FLASH��������2K */
/* �洢����������Ϣ�ı����� */
#define PARAM_BACKUP_START_ADDR ((uint32_t)0x807f000)   /* FLASH�����������ڶ���2K */
#else /* ��С����оƬ */
/* �洢����������Ϣ */
#define PARAM_START_ADDR		((uint32_t)0x803f800)   /* FLASH��������2K */
/* �洢����������Ϣ�ı����� */
#define PARAM_BACKUP_START_ADDR ((uint32_t)0x803f000)   /* FLASH�����������ڶ���2K */
#endif

//  #define PARAM_START_ADDR		ADDR_FLASH_SECTOR_3			/* 0x0800C000 �м��16KB����������Ų��� */


//#define PARAM_START_ADDR	 ADDR_FLASH_SECTOR_11		/* 0x080E0000 Flash���128K����������Ų��� */
#endif

#define  FLASH_DATA_BEGIN_FLAG      0x55555555          //FLASH ��������ʼ��־
#define  FLASH_DATA_END_FLAG        0xaaaaaaaa          //FLASH ������������־

typedef struct
{
	uint8_t ip[4];         //IP��ַ
	uint16_t port;          //�˿�
}IpAddr_TypeDef; //����ͷ�������ص�����(IP,�˿�,�ϴ����ݼ��ʱ��)

typedef struct
{
	uint8_t ip[4];         //IP��ַ
	uint16_t port;          //�˿�
	uint16_t rtd_interval;  //�ϴ����ݵ�ʱ����
	struct uip_conn *uip_conn;      //uip���ӵľ��
	uint16_t gprs_id;       //GPRS�����Ӻ�
	uint16_t heartbeart_period;      //��������
}Srv_TypeDef; //����ͷ�������ص�����(IP,�˿�,�ϴ����ݼ��ʱ��)

/* ȫ�ֲ���(������4�ֽڵı���,����Ӧ�����ֺͰ���д��FLASH) */
typedef struct struct_param
{
	uint32_t Version;               /* �������汾���ƣ������ڳ�������ʱ�������Ƿ�Բ��������������� */

	/* ������У׼���� */
	char DevNO[16];                 /* �����豸���,Ŀǰֻ��15λ */
	uint32_t Pwd;                   /* ����6λ���� */

	uint8_t u8_ST;                  /* ��ȾԴ��� */
	uint8_t NTP_En;                 /* 0:����NTP��ʱ����;1:ʹ��NTP��ʱ���� */
	uint16_t u16_SaveRtdInterval;   /* ����AD����ֵ��ʱ��������λ�� */

	Srv_TypeDef Server[4];          /* �����������ص�����(IP,�˿�,�ϴ����ݼ��ʱ���) */

	uint8_t local_ip[4];            /* ����ip */

	char apn[32];                   /* ������������APN */

	uint32_t SystemFlag;            /* bit0:(0-��ֹ�ɼ�;1-ʹ�ܲɼ�).ע��:bit0=0����ֲɼ����ݺ��ϴ����ݶ���0 */

	uint32_t threshold;             /* �����������������ֵuA/uV */

	uint8_t FanChannel;             /* �����ͨ�� */
	uint8_t CleanChannel;           /* ��������ͨ�� */
	uint16_t uc;					/* ��� */

	uint16_t Heardbeat[8];          /* ����������/�� */
}
PARAM_T;

extern __align(8) PARAM_T g_tParam;

uint8_t LoadParam(void);
uint8_t SaveParam(void);
uint8_t SetDefaultParam(void);

#endif /* __PARAM_H */
