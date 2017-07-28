/*
*********************************************************************************************************
*
*	ģ������ : modbus�ײ���������
*	�ļ����� : bsp_modbus.c
*	��    �� : V1.0
*	˵    �� : Modbus���������ṩ�շ��ĺ�����
*
*	Copyright (C), 2014-2015, ���������� www.armfly.com
*
*********************************************************************************************************
*/
/* Includes -----------------------------------------------------------------*/
#include "bsp.h"
/* Private function prototypes ----------------------------------------------*/
static void MODBUS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen);
static void MODBUS_SendAckErr(uint8_t _ucErrCode);
static void MODBUS_SendAckOk(void);
/* Private variables --------------------------------------------------------*/
static uint8_t g_rtu_timeout = 0;
/* Global variables ---------------------------------------------------------*/
MODBUS_T g_tModbus;
/* Declaration of extern functions ------------------------------------------*/
extern void MODBUS_AnalyzeApp(void);

extern __IO uint16_t AdcValue[];//��ʱ
/*
*********************************************************************************************************
*	�� �� ��: MODBUS_SendWithCRC
*	����˵��: ����һ������, �Զ�׷��2�ֽ�CRC
*	��    ��: _pBuf ���ݣ�
*			  _ucLen ���ݳ��ȣ�����CRC��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODBUS_SendWithCRC(uint8_t *_pBuf, uint8_t _ucLen)
{
   uint16_t crc;
   uint8_t buf[MODBUS_TX_SIZE];

   memcpy(buf, _pBuf, _ucLen);
   crc = CRC16_Modbus(_pBuf, _ucLen);
   buf[_ucLen++] = crc >> 8;
   buf[_ucLen++] = crc;
   RS485_SendBuf(buf, _ucLen);
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_SendAckErr
*	����˵��: ���ʹ���Ӧ��
*	��    ��: _ucErrCode : �������
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODBUS_SendAckErr(uint8_t _ucErrCode)
{
   uint8_t txbuf[3];

   txbuf[0] = g_tModbus.RxBuf[0];                  /* 485��ַ */
   txbuf[1] = g_tModbus.RxBuf[1] | 0x80;               /* �쳣�Ĺ����� */
   txbuf[2] = _ucErrCode;                          /* �������(01,02,03,04) */

   MODBUS_SendWithCRC(txbuf, 3);
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_SendAckOk
*	����˵��: ������ȷ��Ӧ��.
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODBUS_SendAckOk(void)
{
   uint8_t txbuf[6];
   uint8_t i;

   for (i = 0; i < 6; i++)
   {
      txbuf[i] = g_tModbus.RxBuf[i];
   }
   MODBUS_SendWithCRC(txbuf, 6);
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_RxTimeOut
*	����˵��: ����3.5���ַ�ʱ���ִ�б������� ����ȫ�ֱ��� g_rtu_timeout = 1; ֪ͨ������ʼ���롣
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODBUS_RxTimeOut(void)
{
   g_rtu_timeout = 1;
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_InitVar
*	����˵��: ��ʼ��Modbus�ṹ����
*   ��    ��: _Baud ͨ�Ų����ʣ��Ĳ���������RTUЭ�����ĳ�ʱʱ�䡣3.5���ַ���us*            
*             _WorkMode �����жϴ���ģʽ1. RXM_NO_CRC   RXM_MODBUS_HOST   RXM_MODBUS_DEVICE
*             _DeviceAddr  վ��ַ,�������Ϊ����,�ò���Ϊ0
*
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODBUS_InitVar(uint32_t _Baud, uint8_t _WorkMode, uint8_t _DeviceAddr)
{
   g_rtu_timeout = 0;
   g_tModbus.RxCount = 0;

   g_tModbus.Baud = _Baud;

   g_tModbus.WorkMode = _WorkMode; /* ��������֡������CRCУ�� */

   g_tModbus.DevieAddr = _DeviceAddr;
   //bsp_Set485Baud(_Baud);
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_Poll
*	����˵��: �������ݰ�. �����������������á�
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODBUS_Poll(void)
{
   uint16_t crc1;

   if (g_rtu_timeout == 0)
   {
      /* û�г�ʱ���������ա���Ҫ���� g_tModbus.RxCount */
      return;
   }
   g_rtu_timeout = 0;              /* ���־ */

   /* �յ�����
       05 06 00 88 04 57 3B70 (8 �ֽ�)
           05    :  ��������ĺ�վ��
           06    :  ָ��
           00 88 :  �����������ʾ�Ĵ���
           04 57 :  ����,,,ת���� 10 ������ 1111.��λ��ǰ,
           3B70  :  �����ֽ� CRC ��	��05�� 57��У��
   */


   switch (g_tModbus.WorkMode)
   {
   case WKM_NO_CRC:    /* ��������֡������CRCУ��. ����ASCIIЭ�� */
      {
         /* �����յ����ݸ��Ƶ�����һ�����������ȴ�APP�����ȡ */
         memcpy(g_tModbus.AppRxBuf, g_tModbus.RxBuf, g_tModbus.RxCount);
         g_tModbus.AppRxCount = g_tModbus.RxCount;
//  			bsp_PutKey(MSG_485_RX);		/* ���ð���FIFO������һ���յ�485����֡����Ϣ */
      }
      break;

   case WKM_MODBUS_HOST:           /* Modbus ����ģʽ */
      if (g_tModbus.RxCount < 4)
      {
         goto err_ret;
      }

      /* ����CRCУ��� */
      crc1 = CRC16_Modbus(g_tModbus.RxBuf, g_tModbus.RxCount);
      if (crc1 != 0)
      {
         goto err_ret;
      }

      /* վ��ַ (1�ֽڣ� */
      g_tModbus.AppRxAddr = g_tModbus.RxBuf[0];   /* ��1�ֽ� վ�� */

      /* �����յ����ݸ��Ƶ�����һ�����������ȴ�APP�����ȡ */
      memcpy(g_tModbus.AppRxBuf, g_tModbus.RxBuf, g_tModbus.RxCount);
      g_tModbus.AppRxCount = g_tModbus.RxCount;
      break;

   case WKM_MODBUS_DEVICE:         /* Modbus �ӻ�ģʽ */
      if (g_tModbus.RxCount < 4)
      {
         goto err_ret;
      }

      /* ����CRCУ��� */
      crc1 = CRC16_Modbus(g_tModbus.RxBuf, g_tModbus.RxCount);
      if (crc1 != 0)
      {
         goto err_ret;
      }

      /* վ��ַ (1�ֽڣ� */
      g_tModbus.AppRxAddr = g_tModbus.RxBuf[0];           /* ��1�ֽ� վ�� */
      if (g_tModbus.AppRxAddr != g_tModbus.DevieAddr)     /* �ж��������͵������ַ�Ƿ���� */
      {
         goto err_ret;
      }

      /* ����Ӧ�ò�Э�� */
      MODBUS_AnalyzeApp();
      break;

   default:
      break;
   }

err_ret:
   g_tModbus.RxCount = 0;  /* ��������������������´�֡ͬ�� */
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_ReciveNew
*	����˵��: ���ڽ����жϷ���������ñ����������յ�һ���ֽ�ʱ��ִ��һ�α�������
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODBUS_ReciveNew(uint8_t _byte)
{
   /*
       3.5���ַ���ʱ������ֻ������RTUģʽ���棬��ΪRTUģʽû�п�ʼ���ͽ�������
       �������ݰ�֮��ֻ�ܿ�ʱ���������֣�Modbus�����ڲ�ͬ�Ĳ������£����ʱ���ǲ�һ���ģ�
       ���Ծ���3.5���ַ���ʱ�䣬�����ʸߣ����ʱ������С�������ʵͣ����ʱ������Ӧ�ʹ�

       4800  = 7.297ms
       9600  = 3.646ms
       19200  = 1.771ms
       38400  = 0.885ms
   */
   uint32_t timeout;

   timeout = 35000000 / g_tModbus.Baud;        /* ���㳬ʱʱ�䣬��λus */

   /* Ӳ����ʱ�жϣ���ʱ����us ��ʱ��4����Modbus */
//	bsp_StartHardTimer(4, timeout, (void *)MODBUS_RxTimeOut);

   htim5.Instance->CNT = 0;
//	htim5.Instance = TIM5;
//	htim5.Init.Prescaler = HAL_RCC_GetHCLKFreq()/1000000 - 1;/* ms*/
//	htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
   htim5.Init.Period = timeout;
//	htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
   HAL_TIM_Base_Init(&htim5);

   /* ������ʱ�� 500ms�����жϹر�LED,���رն�ʱ�� */

   HAL_TIM_Base_Start_IT(&htim5);


   if (g_tModbus.RxCount < MODBUS_RX_SIZE)
   {
      g_tModbus.RxBuf[g_tModbus.RxCount++] = _byte;
   }
}



/* Modbus Ӧ�ò����ʾ��������Ĵ���������������� */

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_01H
*	����˵��: ��ȡ��Ȧ״̬����ӦԶ�̿���D01/D02/D03��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODBUS_01H(void)
{
   /*
    ������
       ��������:
           11 �ӻ���ַ
           01 ������
           00 �Ĵ�����ʼ��ַ���ֽ�
           13 �Ĵ�����ʼ��ַ���ֽ�
           00 �Ĵ����������ֽ�
           25 �Ĵ����������ֽ�
           0E CRCУ����ֽ�
           84 CRCУ����ֽ�

       �ӻ�Ӧ��: 	1����ON��0����OFF�������ص���Ȧ����Ϊ8�ı�����������������ֽ�δβʹ��0����. BIT0��Ӧ��1��
           11 �ӻ���ַ
           01 ������
           05 �����ֽ���
           CD ����1(��Ȧ0013H-��Ȧ001AH)
           6B ����2(��Ȧ001BH-��Ȧ0022H)
           B2 ����3(��Ȧ0023H-��Ȧ002AH)
           0E ����4(��Ȧ0032H-��Ȧ002BH)
           1B ����5(��Ȧ0037H-��Ȧ0033H)
           45 CRCУ����ֽ�
           E6 CRCУ����ֽ�

       ����:
           01 01 10 01 00 03   29 0B	--- ��ѯD01��ʼ��3���̵���״̬
           01 01 10 03 00 01   09 0A   --- ��ѯD03�̵�����״̬
   */
   uint16_t reg;
   uint16_t num;
   uint16_t i;
   uint16_t m;
   uint8_t status[10];

   g_tModbus.RspCode = RSP_OK;

   /* û���ⲿ�̵�����ֱ��Ӧ����� */
   if (g_tModbus.RxCount != 8)
   {
      g_tModbus.RspCode = RSP_ERR_VALUE;                /* ����ֵ����� */
      return;
   }

   reg = BEBufToUint16(&g_tModbus.RxBuf[2]);             /* �Ĵ����� */
   num = BEBufToUint16(&g_tModbus.RxBuf[4]);             /* �Ĵ������� */

   m = (num + 7) / 8;

   if ((reg >= REG_D01) && (num > 0) && (reg + num <= REG_DXX + 1))
   {
      for (i = 0; i < m; i++)
      {
         status[i] = 0;
      }
      for (i = 0; i < num; i++)
      {
//			if (bsp_IsLedOn(i + 1 + reg - REG_D01))		/* ��LED��״̬��д��״̬�Ĵ�����ÿһλ */
//			{
//				status[i / 8] |= (1 << (i % 8));
//			}
      }
   }
   else
   {
      g_tModbus.RspCode = RSP_ERR_REG_ADDR;             /* �Ĵ�����ַ���� */
   }

   status[0] = 0xaa; //����

   if (g_tModbus.RspCode == RSP_OK)                      /* ��ȷӦ�� */
   {
      g_tModbus.TxCount = 0;
      g_tModbus.TxBuf[g_tModbus.TxCount++] = g_tModbus.RxBuf[0];
      g_tModbus.TxBuf[g_tModbus.TxCount++] = g_tModbus.RxBuf[1];
      g_tModbus.TxBuf[g_tModbus.TxCount++] = m;           /* �����ֽ��� */

      for (i = 0; i < m; i++)
      {
         g_tModbus.TxBuf[g_tModbus.TxCount++] = status[i];   /* �̵���״̬ */
      }
      MODBUS_SendWithCRC(g_tModbus.TxBuf, g_tModbus.TxCount);
   }
   else
   {
      MODBUS_SendAckErr(g_tModbus.RspCode);               /* ��������������� */
   }
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_02H
*	����˵��: ��ȡ����״̬����ӦT01��T18��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODBUS_02H(void)
{
   /*
       ��������:
           11 �ӻ���ַ
           02 ������
           00 �Ĵ�����ַ���ֽ�
           C4 �Ĵ�����ַ���ֽ�
           00 �Ĵ����������ֽ�
           16 �Ĵ����������ֽ�
           BA CRCУ����ֽ�
           A9 CRCУ����ֽ�

       �ӻ�Ӧ��:  ��Ӧ����ɢ����Ĵ���״̬���ֱ��Ӧ�������е�ÿλֵ��1 ����ON��0 ����OFF��
                  ��һ�������ֽڵ�LSB(����ֽ�)Ϊ��ѯ��Ѱַ��ַ����������ڰ�˳���ڸ��ֽ����ɵ��ֽ�
                  ����ֽ����У�ֱ�������8λ����һ���ֽ��е�8������λҲ�Ǵӵ��ֽڵ����ֽ����С�
                  �����ص�����λ������8�ı������������������ֽ��е�ʣ��λ�����ֽڵ����λʹ��0��䡣
           11 �ӻ���ַ
           02 ������
           03 �����ֽ���
           AC ����1(00C4H-00CBH)
           DB ����2(00CCH-00D3H)
           35 ����3(00D4H-00D9H)
           20 CRCУ����ֽ�
           18 CRCУ����ֽ�

       ����:
       01 02 20 01 00 08  23CC  ---- ��ȡT01-08��״̬
       01 02 20 04 00 02  B3CA  ---- ��ȡT04-05��״̬
       01 02 20 01 00 12  A207   ---- �� T01-18
   */

   uint16_t reg;
   uint16_t num;
   uint16_t i;
   uint16_t m;
   uint8_t status[10];

   g_tModbus.RspCode = RSP_OK;

   if (g_tModbus.RxCount != 8)
   {
      g_tModbus.RspCode = RSP_ERR_VALUE;              /* ����ֵ����� */
      return;
   }

   reg = BEBufToUint16(&g_tModbus.RxBuf[2]);           /* �Ĵ����� */
   num = BEBufToUint16(&g_tModbus.RxBuf[4]);           /* �Ĵ������� */

   m = (num + 7) / 8;
   if ((reg >= REG_T01) && (num > 0) && (reg + num <= REG_TXX + 1))
   {
      for (i = 0; i < m; i++)
      {
         status[i] = 0;
      }
      for (i = 0; i < num; i++)
      {
         if (ReadDI(reg - REG_T01 + i) == ON)
         {
            status[i / 8] |= (1 << (i % 8));
         }
      }
   }
   else
   {
      g_tModbus.RspCode = RSP_ERR_REG_ADDR;               /* �Ĵ�����ַ���� */
   }

   if (g_tModbus.RspCode == RSP_OK)                        /* ��ȷӦ�� */
   {
      g_tModbus.TxCount = 0;
      g_tModbus.TxBuf[g_tModbus.TxCount++] = g_tModbus.RxBuf[0];
      g_tModbus.TxBuf[g_tModbus.TxCount++] = g_tModbus.RxBuf[1];
      g_tModbus.TxBuf[g_tModbus.TxCount++] = m;           /* �����ֽ��� */

      for (i = 0; i < m; i++)
      {
         g_tModbus.TxBuf[g_tModbus.TxCount++] = status[i];   /* T01-02״̬ */
      }
      MODBUS_SendWithCRC(g_tModbus.TxBuf, g_tModbus.TxCount);
   }
   else
   {
      MODBUS_SendAckErr(g_tModbus.RspCode);               /* ��������������� */
   }
}
/*
*********************************************************************************************************
*	�� �� ��: MODBUS_03H
*	����˵��: ��ȡ���ּĴ��� ��һ���������ּĴ�����ȡ�õ�ǰ�Ķ�����ֵ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODBUS_03H(void)
{
   /*
       �ӻ���ַΪ11H�����ּĴ�������ʼ��ַΪ006BH��������ַΪ006DH���ôβ�ѯ�ܹ�����3�����ּĴ�����

       ��������:
           11 �ӻ���ַ
           03 ������
           00 �Ĵ�����ַ���ֽ�
           6B �Ĵ�����ַ���ֽ�
           00 �Ĵ����������ֽ�
           03 �Ĵ����������ֽ�
           76 CRC���ֽ�
           87 CRC���ֽ�

       �ӻ�Ӧ��: 	���ּĴ����ĳ���Ϊ2���ֽڡ����ڵ������ּĴ������ԣ��Ĵ������ֽ������ȱ����䣬
                   ���ֽ����ݺ󱻴��䡣���ּĴ���֮�䣬�͵�ַ�Ĵ����ȱ����䣬�ߵ�ַ�Ĵ����󱻴��䡣
           11 �ӻ���ַ
           03 ������
           06 �ֽ���
           00 ����1���ֽ�(006BH)
           6B ����1���ֽ�(006BH)
           00 ����2���ֽ�(006CH)
           13 ����2 ���ֽ�(006CH)
           00 ����3���ֽ�(006DH)
           00 ����3���ֽ�(006DH)
           38 CRC���ֽ�
           B9 CRC���ֽ�

       ����:
           01 03 30 06 00 01  6B0B      ---- �� 3006H, ��������
           01 03 4000 0010 51C6         ---- �� 4000H ������1����ӿ��¼ 32�ֽ�
           01 03 4001 0010 0006         ---- �� 4001H ������1����ӿ��¼ 32�ֽ�

           01 03 F000 0008 770C         ---- �� F000H ������1���澯��¼ 16�ֽ�
           01 03 F001 0008 26CC         ---- �� F001H ������2���澯��¼ 16�ֽ�

           01 03 7000 0020 5ED2         ---- �� 7000H ������1�����μ�¼��1�� 64�ֽ�
           01 03 7001 0020 0F12         ---- �� 7001H ������1�����μ�¼��2�� 64�ֽ�

           01 03 7040 0020 5F06         ---- �� 7040H ������2�����μ�¼��1�� 64�ֽ�
   */
   uint16_t reg;
   uint16_t num;
   uint16_t i;
   uint8_t reg_value[64];

   g_tModbus.RspCode = RSP_OK;

   if (g_tModbus.RxCount != 8)                             /* 03H���������8���ֽ� */
   {
      g_tModbus.RspCode = RSP_ERR_VALUE;                  /* ����ֵ����� */
      goto err_ret;
   }

   reg = BEBufToUint16(&g_tModbus.RxBuf[2]);               /* �Ĵ����� */
   num = BEBufToUint16(&g_tModbus.RxBuf[4]);                   /* �Ĵ������� */
   if (num > sizeof(reg_value) / 2)
   {
      g_tModbus.RspCode = RSP_ERR_VALUE;                  /* ����ֵ����� */
      goto err_ret;
   }

   for (i = 0; i < num; i++)
   {
//		if (MODBUS_ReadRegValue(reg, &reg_value[2 * i]) == 0)	/* �����Ĵ���ֵ����reg_value */
//		{
//			g_tModbus.RspCode = RSP_ERR_REG_ADDR;				/* �Ĵ�����ַ���� */
//			break;
//		}
      reg++;
   }

err_ret:
   if (g_tModbus.RspCode == RSP_OK)                            /* ��ȷӦ�� */
   {
      g_tModbus.TxCount = 0;
      g_tModbus.TxBuf[g_tModbus.TxCount++] = g_tModbus.RxBuf[0];
      g_tModbus.TxBuf[g_tModbus.TxCount++] = g_tModbus.RxBuf[1];
      g_tModbus.TxBuf[g_tModbus.TxCount++] = num * 2;         /* �����ֽ��� */

      for (i = 0; i < num; i++)
      {
         g_tModbus.TxBuf[g_tModbus.TxCount++] = reg_value[2 * i];
         g_tModbus.TxBuf[g_tModbus.TxCount++] = reg_value[2 * i + 1];
      }
      MODBUS_SendWithCRC(g_tModbus.TxBuf, g_tModbus.TxCount); /* ������ȷӦ�� */
   }
   else
   {
      MODBUS_SendAckErr(g_tModbus.RspCode);                   /* ���ʹ���Ӧ�� */
   }
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_04H
*	����˵��: ��ȡ����Ĵ�������ӦA01/A02�� SMA
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODBUS_04H(void)
{
   /*
       ��������:
           11 �ӻ���ַ
           04 ������
           00 �Ĵ�����ʼ��ַ���ֽ�
           08 �Ĵ�����ʼ��ַ���ֽ�
           00 �Ĵ����������ֽ�
           02 �Ĵ����������ֽ�
           F2 CRC���ֽ�
           99 CRC���ֽ�

       �ӻ�Ӧ��:  ����Ĵ�������Ϊ2���ֽڡ����ڵ�������Ĵ������ԣ��Ĵ������ֽ������ȱ����䣬
               ���ֽ����ݺ󱻴��䡣����Ĵ���֮�䣬�͵�ַ�Ĵ����ȱ����䣬�ߵ�ַ�Ĵ����󱻴��䡣
           11 �ӻ���ַ
           04 ������
           04 �ֽ���
           00 ����1���ֽ�(0008H)
           0A ����1���ֽ�(0008H)
           00 ����2���ֽ�(0009H)
           0B ����2���ֽ�(0009H)
           8B CRC���ֽ�
           80 CRC���ֽ�

       ����:

           01 04 2201 0006 2BB0  --- �� 2201H A01ͨ��ģ���� ��ʼ��6������
           01 04 2201 0001 6A72  --- �� 2201H

   */
   uint16_t reg;
   uint16_t num;
   uint16_t i;
   uint16_t status[10];

   memset(status, 0, 10);

   g_tModbus.RspCode = RSP_OK;

   if (g_tModbus.RxCount != 8)
   {
      g_tModbus.RspCode = RSP_ERR_VALUE;  /* ����ֵ����� */
      goto err_ret;
   }

   reg = BEBufToUint16(&g_tModbus.RxBuf[2]);   /* �Ĵ����� */
   num = BEBufToUint16(&g_tModbus.RxBuf[4]);   /* �Ĵ������� */

   if ((reg >= REG_A01) && (num > 0) && (reg + num <= REG_AXX + 1))
   {
      for (i = 0; i < num; i++)
      {
         switch (reg)
         {
            /* ���Բ��� */
         case REG_A01:
            status[i] = AdcValue[0];
            break;
         case REG_A02:
            status[i] = AdcValue[1];
            break;
         case REG_A03:
            status[i] = AdcValue[2];
            break; 
         case REG_A04:
            status[i] = AdcValue[3];
            break;
         case REG_A05:
            status[i] = AdcValue[4];
            break;
         case REG_A06:
            status[i] = AdcValue[5];
            break;
         case REG_A07:
            status[i] = AdcValue[6];
            break;
         case REG_A08:
            status[i] = AdcValue[7];
            break;

         default:
            status[i] = 0;
            break;
         }
         reg++;
      }
   }
   else
   {
      g_tModbus.RspCode = RSP_ERR_REG_ADDR;       /* �Ĵ�����ַ���� */
   }

err_ret:
   if (g_tModbus.RspCode == RSP_OK)        /* ��ȷӦ�� */
   {
      g_tModbus.TxCount = 0;
      g_tModbus.TxBuf[g_tModbus.TxCount++] = g_tModbus.RxBuf[0];
      g_tModbus.TxBuf[g_tModbus.TxCount++] = g_tModbus.RxBuf[1];
      g_tModbus.TxBuf[g_tModbus.TxCount++] = num * 2;         /* �����ֽ��� */

      for (i = 0; i < num; i++)
      {
         g_tModbus.TxBuf[g_tModbus.TxCount++] = status[i] >> 8;
         g_tModbus.TxBuf[g_tModbus.TxCount++] = status[i] & 0xFF;
      }
      MODBUS_SendWithCRC(g_tModbus.TxBuf, g_tModbus.TxCount);
   }
   else
   {
      MODBUS_SendAckErr(g_tModbus.RspCode);   /* ��������������� */
   }
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_05H
*	����˵��: ǿ�Ƶ���Ȧ����ӦD01/D02/D03��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODBUS_05H(void)
{
   /*
       ��������: д������Ȧ�Ĵ�����FF00Hֵ������Ȧ����ON״̬��0000Hֵ������Ȧ����OFF״̬
       ��05Hָ�����õ�����Ȧ��״̬��15Hָ��������ö����Ȧ��״̬��
           11 �ӻ���ַ
           05 ������
           00 �Ĵ�����ַ���ֽ�
           AC �Ĵ�����ַ���ֽ�
           FF ����1���ֽ�
           00 ����2���ֽ�
           4E CRCУ����ֽ�
           8B CRCУ����ֽ�

       �ӻ�Ӧ��:
           11 �ӻ���ַ
           05 ������
           00 �Ĵ�����ַ���ֽ�
           AC �Ĵ�����ַ���ֽ�
           FF �Ĵ���1���ֽ�
           00 �Ĵ���1���ֽ�
           4E CRCУ����ֽ�
           8B CRCУ����ֽ�

       ����:
       01 05 10 01 FF 00   D93A   -- D01��
       01 05 10 01 00 00   98CA   -- D01�ر�

       01 05 10 02 FF 00   293A   -- D02��
       01 05 10 02 00 00   68CA   -- D02�ر�

       01 05 10 03 FF 00   78FA   -- D03��
       01 05 10 03 00 00   390A   -- D03�ر�
   */
   uint16_t reg;
   uint16_t value;

   g_tModbus.RspCode = RSP_OK;

   if (g_tModbus.RxCount != 8)
   {
      g_tModbus.RspCode = RSP_ERR_VALUE;      /* ����ֵ����� */
      goto err_ret;
   }

   reg = BEBufToUint16(&g_tModbus.RxBuf[2]);   /* �Ĵ����� */
   value = BEBufToUint16(&g_tModbus.RxBuf[4]); /* ���� */

   if (value != 0 && value != 0xFF00)
   {
      g_tModbus.RspCode = RSP_ERR_VALUE;      /* ����ֵ����� */
      goto err_ret;
   }

   if (value == 0xFF00)
   {
      value = 1;
   }

   if (reg == REG_D01)
   {
      SetDO1((IO_SateTypedef)value);
   }
   else if (reg == REG_D02)
   {
      SetDO2((IO_SateTypedef)value);
   }
   else if (reg == REG_D03)
   {
      SetDO3((IO_SateTypedef)value);
   }
   else if (reg == REG_D04)
   {
      SetDO4((IO_SateTypedef)value);
   }
   else if (reg == REG_D05)
   {
      SetDO5((IO_SateTypedef)value);
   }
   else if (reg == REG_D06)
   {
      SetDO6((IO_SateTypedef)value);
   }
   else if (reg == REG_D07)
   {
      SetDO7((IO_SateTypedef)value);
   }
   else if (reg == REG_D08)
   {
      SetDO8((IO_SateTypedef)value);
   }
   else
   {
      g_tModbus.RspCode = RSP_ERR_REG_ADDR;       /* �Ĵ�����ַ���� */
   }
err_ret:
   if (g_tModbus.RspCode == RSP_OK)                /* ��ȷӦ�� */
   {
      MODBUS_SendAckOk();
   }
   else
   {
      MODBUS_SendAckErr(g_tModbus.RspCode);       /* ��������������� */
   }
}

/*
*********************************************************************************************************
*	�� �� ��: MODBUS_06H
*	����˵��: д�����Ĵ���
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODBUS_06H(void)
{

}


/*
*********************************************************************************************************
*	�� �� ��: MODBUS_10H
*	����˵��: ����д����Ĵ���.  �����ڸ�дʱ��
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void MODBUS_10H(void)
{

}


/*
*********************************************************************************************************
*	�� �� ��: MODBUS_AnalyzeApp
*	����˵��: ����Ӧ�ò�Э��
*	��    ��:
*		     _DispBuf  �洢����������ʾ����ASCII�ַ�����0����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void MODBUS_AnalyzeApp(void)
{
   /* Modbus�ӻ� */
   switch (g_tModbus.RxBuf[1])         /* ��2���ֽ� ������ */
   {
   case 0x01:  /* ��ȡ��Ȧ״̬����ӦԶ�̿���D01/D02/D03�� */
      MODBUS_01H();
      break;

   case 0x02:  /* ��ȡ����״̬����ӦT01��T18�� */
      MODBUS_02H();
      break;

   case 0x03:  /* ��ȡ���ּĴ��� ��һ���������ּĴ�����ȡ�õ�ǰ�Ķ�����ֵ */
      MODBUS_03H();
      break;

   case 0x04:  /* ��ȡ����Ĵ�������ӦA01/A02�� �� */
      MODBUS_04H();
      break;

   case 0x05:  /* ǿ�Ƶ���Ȧ����ӦD01/D02/D03�� */
      MODBUS_05H();
      break;

   case 0x06:  /* д�����Ĵ��� (�洢��EEPROM�еĲ���) */
      MODBUS_06H();
      break;

   case 0x10:  /* д����Ĵ��� ����дʱ�ӣ� */
      MODBUS_10H();
      break;

   default:
      g_tModbus.RspCode = RSP_ERR_CMD;
      //MODBUS_SendAckErr(g_tModbus.RspCode);	/* ��������������� */
      break;
   }
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
