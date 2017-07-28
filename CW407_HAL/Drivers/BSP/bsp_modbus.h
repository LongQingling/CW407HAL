/*
*********************************************************************************************************
*
*	ģ������ : modbus�ײ���������
*	�ļ����� : bsp_modbus.h
*	��    �� : V1.0
*	˵    �� : ͷ�ļ�
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#ifndef __BSP_MODBUS_H
#define __BSP_MODBUS_H

/* ����KEY_FIFO ��Ϣ֪ͨӦ�ó��� */
#define MSG_485_RX			0xFE	/* �˴���ֵ��Ҫ�Ͱ������롢����ң�ش���ͬ����룬����Ψһ�� */

/* �����жϳ���Ĺ���ģʽ -- ModbusInitVar() �������β� */
#define WKM_NO_CRC			0		/* ��У��CRC�����ݳ��ȡ�����ASCIIЭ�顣��ʱ��ֱ�Ӹ�Ӧ�ò㴦�� */
#define WKM_MODBUS_HOST		1		/* У��CRC,��У���ַ�� ��Modbus����Ӧ�� */
#define WKM_MODBUS_DEVICE	2		/* У��CRC����У�鱾����ַ�� ��Modbus�豸���ӻ���Ӧ�� */

#define SADDR485	1
#define SBAUD485	UART3_BAUD

/* 01H ��ǿ�Ƶ���Ȧ */
/* 05H дǿ�Ƶ���Ȧ */
#define REG_D01		100
#define REG_D02		101
#define REG_D03		102
#define REG_D04		103
#define REG_D05		104
#define REG_D06		105
#define REG_D07		106
#define REG_D08		107
#define REG_DXX 	REG_D08

/* 02H ��ȡ����״̬ */
#define REG_T01		200
#define REG_T02		201
#define REG_T03		202
#define REG_T04		203
#define REG_T05		204
#define REG_T06		205
#define REG_T07		206
#define REG_T08		207
#define REG_TXX		REG_T08

/* 03H �����ּĴ��� */
/* 06H д���ּĴ��� */
/* 10H д�������Ĵ��� */
#define SLAVE_REG_P01		0x0301
#define SLAVE_REG_P02		0x0302

/* 04H ��ȡ����Ĵ���(ģ���ź�) */
#define REG_A01		300
#define REG_A02		301
#define REG_A03		302
#define REG_A04		303
#define REG_A05		304
#define REG_A06		305
#define REG_A07		306
#define REG_A08		307
#define REG_AXX		REG_A08

/* RTU Ӧ����� */
#define RSP_OK				0		/* �ɹ� */
#define RSP_ERR_CMD			0x01	/* ��֧�ֵĹ����� */
#define RSP_ERR_REG_ADDR	0x02	/* �Ĵ�����ַ���� */
#define RSP_ERR_VALUE		0x03	/* ����ֵ����� */
#define RSP_ERR_WRITE		0x04	/* д��ʧ�� */

#define MODBUS_RX_SIZE		128
#define MODBUS_TX_SIZE      128

typedef struct
{
	uint8_t RxBuf[MODBUS_RX_SIZE];
	uint8_t RxCount;
	uint8_t RxStatus;
	uint8_t RxNewFlag;

	uint8_t AppRxBuf[MODBUS_RX_SIZE];
	uint8_t AppRxCount;
	uint8_t AppRxAddr;					/* ���յ�������֡����豸��ַ */
	
	uint8_t  DevieAddr;				    /* �豸��ַ */
	uint32_t Baud;
	
	uint8_t RspCode;

	uint8_t TxBuf[MODBUS_TX_SIZE];
	uint8_t TxCount;

	uint8_t WorkMode;	/* �����жϵĹ���ģʽ�� ASCII, MODBUS������ MODBUS �ӻ� */
}MODBUS_T;

/* DI DO(��Ȧ)��״̬ */
typedef enum
{
    OFF = 0,
    ON
}IO_SateTypedef;

void MODBUS_InitVar(uint32_t _Baud, uint8_t _WorkMode, uint8_t _DeviceAddr);
void MODBUS_Poll(void);
void MODBUS_RxTimeOut(void);
void MODBUS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen);
void MODBUS_ReciveNew(uint8_t _byte);		/* �����ڽ����жϷ��������� */

extern MODBUS_T g_tModbus;

#endif

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
