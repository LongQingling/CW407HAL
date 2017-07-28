/****************** Copyright(c) ��������Ƽ���չ���޹�˾ ***********************/
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __PROTOCOL_H__
#define __PROTOCOL_H__

/* Includes ----------------------------------------------------------------- */
#include "bsp.h"
/* Private macros ----------------------------------------------------------- */
/* Exported types ----------------------------------------------------------- */
/* Exported constants ------------------------------------------------------- */
//#define CONNECT_BUFFER_SIZE 1032
/* Exported macros ---------------------------------------------------------- */
/* Exported variables ------------------------------------------------------- */
//extern uint8_t  USART1_RecData[CONNECT_BUFFER_SIZE];//����USART1���յ������ݣ��ȴ�����
//extern uint16_t USART1_RecDataCnt; //������յ����ֽ���
//extern uint8_t  USART1_RecDataOver;//1:USART1�������ݱ������������δʹ�ã�
//
//extern uint8_t  USART1_SendBuffer[CONNECT_BUFFER_SIZE];//USART1 send buffer
//extern uint16_t USART1_SendBufferCnt;
//
//extern uint8_t  USART1_SendState;//0��δ����״̬;1:���ڷ���״̬ ��δʹ��
//
//extern uint16_t USART1_RecTimeOutCnt;//�������ݰ�ʱ��ʱ��ʱ��������һ��ֵ��ԭ�������ݰ�
//                                        //���㣬���½���
//extern uint8_t  USART1_RecFrameDone; //1:������һ֡���ݰ�;0:δ���յ����ݰ�

extern char g_u8_PkgBuf[];

/* Exported functions ------------------------------------------------------- */
uint32_t ConvertNum(char *str,unsigned char length);
void UpLoadData(uint8_t conn_id);
void QnReturn(uint8_t conn_id, char *PtQn);
void ExeReturn(uint8_t conn_id, char * PtQn, uint8_t ExeRtn);
void DateAnalyzer(uint8_t conn_id, const char *str);
uint8_t HJ212_CheckData(char dat[], uint16_t len);
uint8_t HJ212_DataHandle(uint8_t conn_id, char dat[]);
void QR_Update(void);

#endif /*__PROTOCOL_H__*/

/********* (C) COPYRIGHT 2015-2025 ��������Ƽ���չ���޹�˾ *******END OF FILE****/

