/******************* Copyright(c) ��������Ƽ���չ���޹�˾ *******************
* 
*------File Info--------------------------------------------------------------
* File Name:            gprs.c
*   					ME909s-821����
*   					ME909�����Ӻ�Ϊ1-5,��Ϊ��ͳһ���,��Ϊ0-4
* Latest modified Date: 
* Latest Version:       
* Description:          gprsͨѶ
* compiler:             MDK V5.22
* MCU:                  STM32F103VE
* Oscillator Crystal Frequency:    8MHz
*-----------------------------------------------------------------------------
* Created By:    �����
* Created date:  2015-07-27
* Version:       v0.1
* Descriptions:  
*
*-----------------------------------------------------------------------------
* Modified by:   �޸��˵�����
* Modified date: �ļ����޸����ڣ�YYYY-MM-DD��
* Version:       �ļ��޶��İ汾��
* Description:   �ļ��޶��ļ�Ҫ����
*
******************************************************************************/

/* Includes -----------------------------------------------------------------*/
#include "bsp.h"
#include "app.h"
#include <stdlib.h>
#include "protocol.h"

/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
#define LINK_MAX	4								/* ���������������� */
/* Private macro ------------------------------------------------------------*/
#define CONNECT_BUFFER_SIZE 1055                    /* ����GPRS�����ӵ����ݻ������Ĵ�С */
/* Private variables --------------------------------------------------------*/
char Connect_RecBuf[CONNECT_BUFFER_SIZE];                            //��·0������Ϣ��buffer
uint16_t Connect0_RecBufCnt = 0;

//char g_u8_PkgBuf[UART1_TX_BUF_LEN];
/* Private function prototypes ----------------------------------------------*/
static uint8_t DTU_PowerOn(void);
static void DTU_PowerOff(void);
/* Private functions --------------------------------------------------------*/

/* Forward Declaration of local functions -----------------------------------*/
/* Declaration of extern functions ------------------------------------------*/
/* Global variables ---------------------------------------------------------*/
//GPRS���ƴ��ڵ���һ��������,����GPRS�����Ӧ����,�����ڷ�������ǰ���
char GPRS_RxBufTmp[GPRS_RX_BUF_TMP_LEN];
volatile uint16_t GPRS_RxBufTmp_cnt;

NetTypedef NetType = NOSERVICE;                                      //��������
GPRS_StateTypeDef GPRS_State = NO_INIT;                              //gprs״̬
uint8_t Isp;                                                         //ISPʶ����(0:�й��ƶ�;1:�й���ͨ;2:�й�����;3:����)
char IMSI[16] = { 0 };                                               //����15λIMSI��
uint8_t g_ucConnectEnable = 0x00;                                    //8·����ʹ�ܱ�־ 0:�����ӱ���ֹ;1:�����ӱ�ʹ��
uint8_t g_ucConnectState = 0x00;                                     //8·���ӵ�״̬,bit0~bit7����0~7·���� 0:���ӷ�����; 1:δ���ӷ�����
uint8_t g_ucReconnectCnt = 0x00;                                     //��������,������Ӻ�������GPRSģ��.//��ʱûʹ��
uint8_t g_ucaHeartbeatCnt[LINK_MAX] = { 0 };                         //�����ӵ�������ʱ�����,Ϊ0����������
uint8_t GprsSignalLevel = 0;                                         //���������ź�ǿ��
uint8_t GprsInitRequest = 0;                                         //�������³�ʼ����������
/*---------------------------------------------------------------------------*/
/*                          Private functions                                */
/*---------------------------------------------------------------------------*/

/*
*********************************************************************************************************
*	�� �� ��: DTU_CleanRxBufTmp
*	����˵��: ���GPRS��Ϣ��ʱ������
*   ��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void DTU_CleanRxBufTmp(void)
{
    //  uint16_t i;
    //  for (i = 0; i < GPRS_RX_BUF_TMP_LEN; i++)
    //  {
    //      GPRS_RxBufTmp[i++] = 0;
    //  }
    memset(GPRS_RxBufTmp, '\0', GPRS_RX_BUF_TMP_LEN);
    GPRS_RxBufTmp_cnt = 0;
}

/*
*********************************************************************************************************
*	�� �� ��: ME_SendAtCmdNoResponse
*	����˵��: ��GSMģ�鷢��AT��� �������Զ���AT�ַ���������<CR>�ַ�
*	��    ��: _Str : AT�����ַ�����������ĩβ�Ļس�<CR>. ���ַ�0����
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ME_SendAtCmdNoResponse(char *_Cmd)
{
    comSendBuf(GPRS_COM, (uint8_t *)_Cmd, strlen(_Cmd));
    comSendBuf(GPRS_COM, "\r", 1);
}

/*
*********************************************************************************************************
*	�� �� ��: ME_Connect
*	����˵��: ��ѯ��ǰ�����ź�ǿ��
*   ��    ��: uint8_t _ucConnect ���Ӻ� 0-3 �Զ�ת��ΪME909��1-5
*            uint8_t _ucType  �������� 0:TCP; 1:UDP; 2:FTP; 3:FTPS
*            uint8_t _ucIp[4] ����ip������
*            uint16_t _uiPort ���Ӷ˿�
*	�� �� ֵ: 0: ���ӳɹ�; 1:�����Ѵ���; 2:����ʧ��
*********************************************************************************************************
*/
static uint8_t ME_Connect(uint8_t _ucConnect, uint8_t _ucType, uint8_t _ucIp[4], uint16_t _uiPort)
{
    char str_tmp[45];
    uint16_t i;

    /* Check the parameters */
    assert_param(IS_CONNECT_ID(_ucConnect));

    g_ucConnectEnable |= (uint8_t)(1 << _ucConnect);
    g_ucaHeartbeatCnt[_ucConnect] = 0;                               //��������ʱ����

    {
        switch (_ucType)
        {
        case 0:                                                      //AT^IPOPEN=2,"TCP","120.197.159.424",9895
            sprintf(str_tmp, "AT^IPOPEN=%1d,\"TCP\",\"%d.%d.%d.%d\",%d", _ucConnect + 1, _ucIp[0], _ucIp[1], _ucIp[2], _ucIp[3], _uiPort);
            break;
        case 1:
            sprintf(str_tmp, "AT^IPOPEN=%1d,\"UDP\",\"%d.%d.%d.%d\",%d", _ucConnect + 1, _ucIp[0], _ucIp[1], _ucIp[2], _ucIp[3], _uiPort);
            break;
        default:
            break;
        }

        DTU_CleanRxBufTmp();                                         //���GPRS��Ϣ���յ���ʱ�洢��
        ME_SendAtCmdNoResponse(str_tmp);
        i = 100;
        do
        {
            if (strstr(GPRS_RxBufTmp, "OK") != NULL)
            {
#if GPRS_DEBUG_EN
                DEBUG_PrintfString(GPRS_RxBufTmp);
#endif
                return 0;                                            //���ӳɹ�
            }
            else if (strstr(GPRS_RxBufTmp, "ERROR") != NULL)
            {
                if (strstr(GPRS_RxBufTmp, "+CME ERROR: 1003") != NULL)
                {
#if GPRS_DEBUG_EN
                    DEBUG_PrintfString(GPRS_RxBufTmp);
#endif
                    return 1;                                        //�����Ѵ���
                }
                else if (strstr(GPRS_RxBufTmp, "+CME ERROR: 1012") != NULL) /* ����Ͽ� */
                {
                    ME909_Init();
                    return 2;
                }
                break;
            }
            osDelay(10);
        }while (i--);
    }
#if GPRS_DEBUG_EN
    DEBUG_PrintfString(GPRS_RxBufTmp);
#endif
    return 2;                                                        //����ʧ��
}

/* *****************************************************************************
  Function:     ME_SendAtCmd
  Description:  ����AT�����ͨ��ģ��
  Input:        uint8_t *cmd �����ַ���
                uint8_t *ack ��ҪӦ����ַ�������
                uint16_t wait_time ����ȴ���ʱ��n*10ms
  Return:       0: ����ͳɹ���Ӧ��ɹ�
                1: �ȴ�Ӧ��ʱ
                2: ����ERROR
  Others:       
***************************************************************************** */
static uint8_t ME_SendAtCmd(char *cmd, char *ack, uint16_t wait_time)
{
    /* ��Ҫ�ȴ�����������ʱ�� */
    if (wait_time)
    {
        DTU_CleanRxBufTmp();                                         //���GPRS��Ϣ���յ���ʱ�洢��
    }

    ME_SendAtCmdNoResponse(cmd);

    while (wait_time--)
    {
        osDelay(10);
        if (strstr(GPRS_RxBufTmp, ack) != NULL)
        {
#if  GPRS_DEBUG_EN
            DEBUG_PrintfString(GPRS_RxBufTmp);
#endif
            return 0;                                                //���ͳɹ�
        }
        else if (strstr(GPRS_RxBufTmp, "ERROR") != NULL)
        {
            return 2;
        }
    }
#if  GPRS_DEBUG_EN
    DEBUG_PrintfString(GPRS_RxBufTmp);
#endif
    return 1;                                                        //��ʱ,����
}

/*
*********************************************************************************************************
*	�� �� ��: ME_ErrorHandle
*	����˵��: ����DTU���صĴ�����Ϣ
*   ��    ��: str ������Ϣ����Ϣ�ַ���
*             conn_id ���Ӻ� 
*	�� �� ֵ: ��
*********************************************************************************************************
*/
static void ME_ErrorHandle(char *str, uint8_t conn_id)
{
    if (strstr(GPRS_RxBufTmp, "+CME ERROR: 1002") != NULL)           /* ����1002����(���ӶϿ�) */
    {
        ME_ConnectSever(conn_id);
    }
    else if (strstr(GPRS_RxBufTmp, "+CME ERROR: 1012") != NULL)      /* ����1012����(����Ͽ�) */
    {
        DTU_Init();
    }
    else
    {}
}
/*
*********************************************************************************************************
*	�� �� ��: DTU_PowerOff
*	����˵��: ����DTUģ��ػ�(ME909��Ӳ���ػ�����)
*	��    ��: ��
*	�� �� ֵ: ��
********************
*************************************************************************************
*/
static void DTU_PowerOff(void)
{
    ME_SendAtCmd("AT^MSO", "OK", 100);
}

/*---------------------------------------------------------------------------*/
/*                            function code                                  */
/*---------------------------------------------------------------------------*/
//�ڷ�͸��ģʽ�·���һ���ַ�
/******************************************************************************
  Function:     ME_TCPSendString
  Description:  ͨ��DTU��Ӧ����·����һ���ַ���
  Input:        uint8_t conn_id ��·ID�� 0-3 �Զ�ת��ΪME909��1-5
                char *str       �ַ���ָ��
                uint16_t wait_time  �ȴ���ʱ��,��λ1ms
  Return:       0: ����ͳɹ���Ӧ��ɹ�
                1: ����ʧ��
                2: ���ͳ�ʱ
  Others:       
******************************************************************************/
uint8_t ME_TCPSendString(uint8_t conn_id, char *str, uint16_t wait_time)
{
    assert_param(IS_CONNECT_ID(conn_id));
    char str_tmp[25];
    uint16_t length = strlen(str);
    uint8_t result;
//    char *ptr;
    uint8_t i;

    //   ME_SendAtCmd("AT^IPSEND=1,\"abc\"","OK",100);
    for (i = 0; i < 2; i++)
    {
        sprintf(str_tmp, "AT^IPSENDEX=%1d,2,%d", conn_id + 1, length); //����ģʽ2����
        result = ME_SendAtCmd(str_tmp, "OK", 50);

        if (result == 0)
        {
            DTU_CleanRxBufTmp();
            comSendBuf(GPRS_COM, (uint8_t *)str, length);

            while (wait_time--)
            {
                if (strstr(GPRS_RxBufTmp, "^IPSENDEX:") != NULL)
                {
#if GPRS_DEBUG_EN
                    DEBUG_PrintfString("Send data to connect :");
                    printf("%d OK : ", conn_id);
                    DEBUG_PrintfString(str);
                    DEBUG_PrintfBytes("\r\n", 3);
                    DEBUG_PrintfString(GPRS_RxBufTmp);
#endif
                    return 0;
                }
                else if (strstr(GPRS_RxBufTmp, "ERROR") != NULL)
                {

#if GPRS_DEBUG_EN
                    DEBUG_PrintfString("Send data to connect :");
                    printf("%d fail:", conn_id);
                    DEBUG_PrintfString(str);
                    DEBUG_PrintfBytes("\r\n", 3);
                    DEBUG_PrintfString(GPRS_RxBufTmp);
#endif
                    ME_ErrorHandle(GPRS_RxBufTmp, conn_id);
                    break;
                }
                osDelay(1);
            }                                                        //while
            break;
        }
        else
        {
            ME_ErrorHandle(GPRS_RxBufTmp, conn_id);
        }
    }
    return 1;                                                        /* ����ʧ�� */
}

/******************************************************************************
  Function:     ME909_Init
  Description:  ��ʼ��DTU
  Input:        none
  Return:       GPRS״̬
  Others:       
******************************************************************************/
GPRS_StateTypeDef ME909_Init(void)
{
    char str_tmp[50];
    uint8_t i, res;
    char *p;

    GPRS_State = NO_INIT;

    i = 120;
    while (ME_SendAtCmd("AT", "OK", 10) && i--)
    {
        osDelay(1000);
    }

    /* �رղ��������ϱ���ʾ���� */
    ME_SendAtCmd("AT^CURC=0", "OK", 10);

    for (i = 0; i < 20; i++)
    {
        res = ME_SendAtCmd("AT+CIMI", "OK", 100);                    //��ȡIMSI��
        if (res == 0)
        {
            if ((p = strstr(GPRS_RxBufTmp, "46001")) != NULL)        /* ��ͨ�� */
            {
                Isp = 1;
            }
            else if ((p = strstr(GPRS_RxBufTmp, "46003")) != NULL)   /* ���ſ� */
            {
                Isp = 2;
            }
            else if ((p = strstr(GPRS_RxBufTmp, "46005")) != NULL)   /* ���ſ� */
            {
                Isp = 2;
            }
            else                                                     /* �ƶ��� */
            {
                Isp = 0;
            }
            strncpy(IMSI, p, 15);
            break;
        }
        osDelay(1000);
    }
    if (i >= 20)
    {
        GPRS_State = NO_SIM_CARD;
        return GPRS_State;                                           // ��SIM��
    }

    i = 60;
    do
    {
        res = ME_SendAtCmd("AT+CPIN?", "READY", 200);                //��ѯSIM���Ƿ�����
        if (res == 0)
        {
            break;
        }
        osDelay(1000);
    }while (res && (i--));

    if (res && (i == 0))
    {
#if  GPRS_DEBUG_EN
        DEBUG_PrintfString("SIM card fail.\n");
#endif
        GPRS_State = INVALID_SIM_CARD;
        return GPRS_State;                                           //SIM����ʼ��ʧ��
    }

    i = 10;
    do
    {
        res = ME_SendAtCmd("AT^IPINIT?", "^IPINIT: 1,", 300);        //��ѯGPRSҵ��
        if (res == 0)
        {
            break;
        }
        else if (res == 2)
        {
            continue;
        }

        /* ��ʼ��������� */
        sprintf(str_tmp,"AT^IPINIT=\"%s\"",g_tParam.apn);
        ME_SendAtCmd(str_tmp, "OK", 300);              //�ֶ�����GPRSҵ��
        //ME_SendAtCmd("AT^IPINIT=\"3gnet\"", "OK", 300);              //�ֶ�����GPRSҵ��
        osDelay(1000);
    }while (i--);

    if (res && (i == 0))
    {
#if  GPRS_DEBUG_EN
        DEBUG_PrintfString("no GPRS.\n");
#endif
        GPRS_State = NO_GPRS;
        return GPRS_State;                                           //��GPRSҵ��
    }

    //����1
    if (g_tParam.Server[0].port != 0)
    {
        res = ME_Connect(0, 0, g_tParam.Server[0].ip, g_tParam.Server[0].port);

#if GPRS_DEBUG_EN
        if (res == 0)
        {
            printf("connect0 %d.%d.%d.%d,%d, connect ok.\n", g_tParam.Server[0].ip[0], g_tParam.Server[0].ip[1], g_tParam.Server[0].ip[2], g_tParam.Server[0].ip[3], g_tParam.Server[0].port);
        }
        else if (res == 1)
        {
            printf("connect0 %d.%d.%d.%d,%d, connect exit.\n", g_tParam.Server[0].ip[0], g_tParam.Server[0].ip[1], g_tParam.Server[0].ip[2], g_tParam.Server[0].ip[3], g_tParam.Server[0].port);
        }
        else
        {
            printf("connect0 %d.%d.%d.%d,%d, connect fail.\n", g_tParam.Server[0].ip[0], g_tParam.Server[0].ip[1], g_tParam.Server[0].ip[2], g_tParam.Server[0].ip[3], g_tParam.Server[0].port);
        }
#endif
    }

    //����2
    if (g_tParam.Server[1].port != 0)
    {
        res = ME_Connect(1, 0, g_tParam.Server[1].ip, g_tParam.Server[1].port);
#if GPRS_DEBUG_EN
        if (res == 0)
        {
            printf("connect1 %d.%d.%d.%d,%d, connect ok.\n", g_tParam.Server[1].ip[0], g_tParam.Server[1].ip[1], g_tParam.Server[1].ip[2], g_tParam.Server[1].ip[3], g_tParam.Server[1].port);
        }
        else if (res == 1)
        {
            printf("connect1 %d.%d.%d.%d,%d, connect exit.\n", g_tParam.Server[1].ip[0], g_tParam.Server[1].ip[1], g_tParam.Server[1].ip[2], g_tParam.Server[1].ip[3], g_tParam.Server[1].port);
        }
#endif
    }

    //����3
    if (g_tParam.Server[2].port != 0)
    {
        res = ME_Connect(2, 0, g_tParam.Server[2].ip, g_tParam.Server[2].port);
#if GPRS_DEBUG_EN
        if (res == 0)
        {
            printf("connect2 %d.%d.%d.%d,%d, connect ok.\n", g_tParam.Server[2].ip[0], g_tParam.Server[2].ip[1], g_tParam.Server[2].ip[2], g_tParam.Server[2].ip[3], g_tParam.Server[2].port);
        }
        else if (res == 1)
        {
            printf("connect2 %d.%d.%d.%d,%d, connect exit.\n", g_tParam.Server[2].ip[0], g_tParam.Server[2].ip[1], g_tParam.Server[2].ip[2], g_tParam.Server[2].ip[3], g_tParam.Server[2].port);
        }
#endif
    }

    //����4
    if (g_tParam.Server[3].port != 0)
    {
        res = ME_Connect(3, 0, g_tParam.Server[3].ip, g_tParam.Server[3].port);
#if GPRS_DEBUG_EN
        if (res == 0)
        {
            printf("connect3 %d.%d.%d.%d,%d, connect ok.\n", g_tParam.Server[3].ip[0], g_tParam.Server[3].ip[1], g_tParam.Server[3].ip[2], g_tParam.Server[3].ip[3], g_tParam.Server[3].port);
        }
        else if (res == 1)
        {
            printf("connect3 %d.%d.%d.%d,%d, connect exit.\n", g_tParam.Server[3].ip[0], g_tParam.Server[3].ip[1], g_tParam.Server[3].ip[2], g_tParam.Server[3].ip[3], g_tParam.Server[3].port);
        }
#endif

    }

    comClearRxFifo(GPRS_COM);                                        /* ��ս��ջ����� */
#if  GPRS_DEBUG_EN
    DEBUG_PrintfString("ME init success.\n");
#endif
    GPRS_State = GPRS_OK;                                            //�����ʼ���ɹ�

    return GPRS_State;
}

/******************************************************************************
  Function:     DTU_Init
  Description:  ��ʼ��GPRSģ��
  Input:        none
  Return:       none
  Others:       ���Գ�ʼ��GPRSģ��,���3��
******************************************************************************/
void DTU_Init(void)
{
    uint8_t i, res;

    LCD_Txt(0, 6, "������������...");
    for (i = 0; i < 3; i++)
    {
#if GPRS_DEBUG_EN
			printf("DTUD start time:%d.\n", i);
#endif
        if (DTU_PowerOn() != 0)
        {
            continue;
        }
        res = ME909_Init();
#if GPRS_DEBUG_EN
        printf("DTUD respond %d.\n", res);
#endif
        DisplayNetState();
        if (GPRS_State == GPRS_OK)
        {
            break;
        }
        //      if (i > 1)
        {
            //         ME_SendAtCmd("AT+MSO", "", 0);                           //���ڿ�����ͨ��10K����ӵ�,�ػ�����������DTUģ��
            osDelay(2000);
            LCD_Txt(0, 6, "��������...");
        }
    }
}

/*
*********************************************************************************************************
*	�� �� ��: ME_ReceiveOneLineInfo
*	����˵��: ��DTUģ�� GPRS���ڻ����л�ȡһ������,��0x0AΪ������־,���߳�ʱ����
*   ��    ��: char *_p_cBuf:ָ�򱣴����ݵĴ洢��ַ;
*            uint16_t _usBufSize:�洢���ĳ���/�ֽ�;�������1���ֽ�
*            uint16_t _usTimeOut:�ȴ���ʱ��/����
*	�� �� ֵ: 0:�����ݻ��߳�ʱ; >0:���ݳ���
*********************************************************************************************************
*/
static uint16_t ME_ReceiveOneLineInfo(char *_p_cBuf, uint16_t _usBufSize, uint16_t _usTimeOut)
{
    uint8_t ucData;
    uint16_t cnt = 0;

    /* �洢�����ȹ�С */
    if (_usBufSize <= 1)
    {
        return 0;
    }

    while (_usTimeOut && --_usBufSize)
    {
        if (comGetChar(GPRS_COM, &ucData))
        {
#if GPRS_DEBUG_EN
            DEBUG_PrintfBytes((char *)&ucData, 1);                   /* �����յ����ݴ�ӡ�����Դ���1 */
#endif
            *_p_cBuf++ = ucData;                                     /* ������յ������� */
            cnt++;

            /* ���յ�������'\n',�˳� */
            if (ucData == '\n')
            {
                break;
            }
        }
        else
        {
            osDelay(1);
            _usTimeOut--;
        }
    }                                                                //while

    *_p_cBuf = '\0';

    return cnt;
}

/******************************************************************************
  Function:     DTU_HandleRecData
  Description:  ����GPRSģ����շ��ص�����
  Input:        none
  Return:       0: ���쳣
                1: �����ݻ����ݳ���
  Others:       Ŀǰֻ֧�ֽ��շ��������ص���Ϣ,ÿ�ζ���ѽ��ջ������е����ݴ�����,
                ���δ��һ֡�Ļ���뱾����������֡��ʱ�洢��
                ������TCP����ͨѶ��������,TCPͨѶ���ͨѶ�쳣�����ݰ�����
******************************************************************************/
void DTU_HandleRecData(void)
{
    //char buf[15];                                                    //����֡��ʱ�洢��
    //uint16_t buf_cnt = 0;                                            //��ʱ�洢�����ݼ�����
    //uint8_t _plus_state = 0;                                         //1:�յ��ַ�+;2:�յ�IPD
    uint8_t res;
    //int16_t num;

    //uint16_t i, j;
    char *p;
    uint8_t connect_id;                                              //������յ������ݵ����Ӻ�
    uint16_t length;                                                 //������յ������ݶεĳ���

    while (ME_ReceiveOneLineInfo(Connect_RecBuf, CONNECT_BUFFER_SIZE, 100))
    {
        if ((p = strstr(Connect_RecBuf, "^IPDATA:")) != NULL)        /* ������յ��ķ�������Ϣ */
        {
            /* ��ȡ���Ӻ� */
            p += 9;
            if (*p < '0' || *p > '9')
            {
                continue;
            }
            connect_id = *p - '0';

            /* ��ȡ���ݶγ��� */
            p += 2;
            length = atoi(p);

            if (length == 0 || length > (1024+12))
            {
                continue;
            }

            /* ָ�����ݶ� */
            p = strchr(p, ',');
            p++;


#if GPRS_DEBUG_EN
            printf("connect%d receive: ", connect_id);
            DEBUG_PrintfBytes(p, length);
#endif

            res = HJ212_CheckData(p, length);                        //�����ݽ���HJ212Э����
            if (res == 0)
            {
                HJ212_DataHandle(connect_id - 1, p);
            }
        }
        else if (strstr(Connect_RecBuf, "^IPSTATE:") != NULL)        /* ��������״̬�仯֪ͨ��Ϣ */
        {
            ME_CheckConnection();
            ME_Reconnection();
        }
    }                                                                //while
}

/******************************************************************************
  Function:     GPRS_CSQ
  Description:  ��ѯGPRS�ź�ǿ��
  Input:        none
  Return:       GPRSģ����ź�ǿ��0-31
  Others:       none
******************************************************************************/
uint8_t GPRS_CSQ(void)
{
    char *p;
    uint8_t i;
    char str_tmp[3];

    ME_SendAtCmd("AT+CSQ", "+CSQ:", 50);
    p = strstr(GPRS_RxBufTmp, "+CSQ:");
    p += 6;
    i = 0;
    while (*p >= '0' && *p <= '9' && i < 2)
    {
        str_tmp[i++] = *p++;
    }
    str_tmp[i] = '\0';
    i = atoi(str_tmp);
    return i;
}

/******************************************************************************
  Function:     DTU_HCSQ
  Description:  ��ѯDTU�ź�ǿ��
  Input:        none
  Return:       �ź�ǿ��0-255
  Others:       none
******************************************************************************/
uint8_t DTU_HCSQ(void)
{
    char *p;
    uint8_t i;

    i = ME_SendAtCmd("AT^HCSQ?", "^HCSQ:", 50);
    if (i != 0)
    {
        return 255;                                                  /* ����,�ź�ǿ��δ֪ */
    }

    /* �ж��������� */
    if ((p = strstr(GPRS_RxBufTmp, "LTE")) != NULL)
    {
        NetType = LTE;
    }
    else if ((p = strstr(GPRS_RxBufTmp, "TD-SCDMA")) != NULL)
    {
        NetType = TDSCDMA;
    }
    else if ((p = strstr(GPRS_RxBufTmp, "WCDMA")) != NULL)
    {
        NetType = WCDMA;
    }
    else if ((p = strstr(GPRS_RxBufTmp, "GSM")) != NULL)
    {
        NetType = TDSCDMA;
    }
    else
    {
        NetType = NOSERVICE;
        return 255;                                                  /* δ֪����,�ź�ǿ��δ֪ */
    }

    //^HCSQ: "LTE",63,61,186,34
    /* �����LTE,����rsrpΪ�ź�ǿ�� */
    if (NetType == LTE)
    {
        p = strstr(p, ",");
        if (p == NULL)
        {
            return 255;                                              /* δ֪���� */
        }
        p++;
    }

    p = strstr(p, ",");
    if (p == NULL)
    {
        return 255;                                                  /* δ֪���� */
    }
    p++;
    i = atoi(p);
    return i;
}

/*
*********************************************************************************************************
*	�� �� ��: DTU_PowerOn
*	����˵��: ģ���ϵ�. �����ڲ����ж��Ƿ��Ѿ�����������ѿ�����ֱ�ӷ���1
*	��    ��: ��
*	�� �� ֵ: 0:��ʾ�ϵ�ɹ�; 1: ��ʾ�쳣
*********************************************************************************************************
*/
static uint8_t DTU_PowerOn(void)
{
    uint8_t i;
    uint8_t res;

    /* �ж��Ƿ񿪻� */
    for (i = 0; i < 3; i++)
    {
        res = ME_SendAtCmd("AT", "OK", 100);
        if (res == 0)
        {
            return 0;
        }
    }

#if GPRS_DEBUG_EN
    DEBUG_PrintfString("Restart ME909.\n");
#endif

    /* ͨ������ RESIN ����100ms��λģ�� */
    GPRS_PowerKeyEnable();
    osDelay(200);
    GPRS_PowerKeyDisable();

    /* ��ʼͬ��������: ��������AT��ֻ�����յ���ȷ��OK 
        ģ�鿪�������ӳ� 2 �� 3 ����ٷ���ͬ���ַ����û��ɷ��͡� AT�� (��д��Сд����)����ģ��
      ͬ�������ʣ��������յ�ģ�鷵�ء� OK����			
    */
    osDelay(2000);
    for (i = 0; i < 200; i++)
    {
        if (ME_SendAtCmd("AT", "OK", 100) == 0)
        {
            return 0;
        }
    }

    return 1;
}

/*
*********************************************************************************************************
*	�� �� ��: DTU_CheckState
*	����˵��: ���DTU״̬,���DTU�޻�Ӧ,��Ϊ����,����ģ���Դ
*	��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DTU_CheckState(void)
{
    uint8_t i;

    for (i = 3; i > 0; i--)
    {
        printf("check me909 state.\n");
        if (DTU_PowerOn() != 0)
        {
            /* ����ʧ��,��ʱ10���ٳ��� */
            osDelay(10000);
        }
        else
        {
            break;                                                   //�����ɹ�,����
        }
    }
}

/******************************************************************************
  Function:     DTU_Startup
  Description:  ����GPRSģ��,���GPRS�Ѿ����������ȹر�1���Ӻ������ϵ�
  Input:        none
  Return:       none
  Others:       none
******************************************************************************/
void DTU_Startup(void)
{
    DTU_PowerOff();
    LCD_Txt(0, 6, "���ر�GPRSģ��...");
    osDelay(30000);                                                  //�ȴ�30��
    DTU_Init();
}

/*
*********************************************************************************************************
*	�� �� ��: ME_Disconnect
*	����˵��: �Ͽ�ĳ������
*   ��    ��: _type 0:����; 1:���  (ME909��ʹ��)
*            _connect_id ���Ӻ�0-3
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ME_Disconnect(uint8_t _type, uint8_t _connect_id)
{
    char str_tmp[15];

    g_ucConnectState &= ~(uint8_t)(1 << _connect_id);
    g_ucConnectEnable &= ~(uint8_t)(1 << _connect_id);
    sprintf(str_tmp, "AT^IPCLOSE=%d", _connect_id + 1);
    ME_SendAtCmd(str_tmp, "OK", 100);
}

/*
*********************************************************************************************************
*	�� �� ��: ME_Reconnection
*	����˵��: �ָ��Ͽ�������
*   ��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ME_Reconnection(void)
{
    uint8_t temp;
    uint8_t i;

    /* �����ӶϿ� */
    if (g_ucConnectEnable != g_ucConnectState)
    {
        temp = g_ucConnectEnable & (uint8_t)(~g_ucConnectState);     //��öϿ������Ӻ�
        for (i = 0; i < 8; i++)
        {
            if (temp & 0x01)
            {
                ME_ConnectSever(i);
            }
            temp >>= 1;
        }

        temp = g_ucConnectState & (uint8_t)(~g_ucConnectEnable);     /* ��ȡ��Ҫ�رյ����Ӻ� */
        for (i = 0; i < 8; i++)
        {
            if (temp & 0x01)
            {
                ME_Disconnect(1, i);
            }
            temp >>= 1;
        }

    }
}

/*
*********************************************************************************************************
*	�� �� ��: ME_ConnectSever
*	����˵��: ���ӵ�N#������
*   ��    ��: uint8_t _ucServerNo: ���ӵ�0#~3#����������������ַ������ȫ�ֲ����ṹ��g_tParam��           
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ME_ConnectSever(uint8_t _ucServerNo)
{
    /* Check the parameters */
    switch (_ucServerNo)
    {
    case 0:
        ME_Connect(0, 0, g_tParam.Server[0].ip, g_tParam.Server[0].port); //����1#������
        break;
    case 1:
        ME_Connect(1, 0, g_tParam.Server[1].ip, g_tParam.Server[1].port); //����2#������
        break;
    case 2:
        ME_Connect(2, 0, g_tParam.Server[2].ip, g_tParam.Server[2].port); //����3#������
        break;
    case 3:
        ME_Connect(3, 0, g_tParam.Server[3].ip, g_tParam.Server[3].port); //����4#������
        break;
    case 4:
        //      ME_Connect(4, 0, g_tParam.Server[3].ip, g_tParam.Server[3].port); //����5#������
        break;
    default:
        break;
    }
}

/*
*********************************************************************************************************
*	�� �� ��: ME_HeartbeartTmrCallback
*	����˵��: �����ʱ��ÿ�����һ�δ˺���,���¸�����������ʱ
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ME_HeartbeartTmrCallback(void)
{
    uint8_t i;
    uint8_t temp = g_ucConnectEnable;

    for (i = 0; i < LINK_MAX; i++)
    {
        if (temp & 0x01)
        {
            g_ucaHeartbeatCnt[i]++;
        }
        temp >>= 1;
    }
}

/*
*********************************************************************************************************
*	�� �� ��: ME_Heartbeart
*	����˵��: ���������ܺ���
*   ��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ME_Heartbeart(void)
{
    uint8_t i;
    uint8_t temp = g_ucConnectEnable;

    for (i = 0; i < 8; i++)
    {
        if (temp & 0x01)
        {
            /* ʱ�䵽,�������� */
//          printf("connect%d cnt: %d\r\n", i, g_ucaHeartbeatCnt[i]);
            if (g_ucaHeartbeatCnt[i] >= g_tParam.Server[i].heartbeart_period)
            {
                g_ucaHeartbeatCnt[i] = 0;
                DEBUG_PrintfString("Send heartbeat data.\r\n");
                ME_TCPSendString(i, "##0000FFFF\r\n", 0);            //�ڴ��޸���������ʽ
            }
        }
        temp >>= 1;
    }
}

/*
*********************************************************************************************************
*	�� �� ��: ME_CheckConnection
*	����˵��: �������״̬,�ѶϿ���������������
*   ��    ��: ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void ME_CheckConnection(void)
{
    uint8_t i;
    uint8_t mask = 0x01;
    char *ptr;
    static uint8_t ucOver = 0;

    ME_SendAtCmd("AT^IPCLOSE?", "^IPCLOSE:", 300);                   //^IPCLOSE: 0,0,0,0,0
    ptr = strstr(GPRS_RxBufTmp, "^IPCLOSE:");
    if (ptr != NULL)
    {
        /*  ����·����״̬ */
        for (ptr += 10, i = 0; i < LINK_MAX; i++)
        {
            if (*ptr == '0')
            {
                g_ucConnectState &= (uint8_t)(~mask);                /* ���ӶϿ� */
            }
            else
            {
                g_ucConnectState |= mask;                            /* ���������� */
            }
            ptr += 2;
            mask <<= 1;
        }

        /* ��ʹ������,��ȫ������ */
        if (g_ucConnectState == 0 && g_ucConnectEnable != 0)
        {
            ucOver++;

            /* ȫ������Ͽ�,�Ͽ�10��*30=5���Ӷ�δ������,����ͨ��ģ�� */
            if (ucOver > 30)
            {
                ucOver = 0;
                DTU_Startup();                                       /*����ͨ��ģ�� */
            }
        }
        else
        {
            ucOver = 0;
        }
    }
}

/*
*********************************************************************************************************
*	�� �� ��: DTU_GetSimNum
*	����˵��: ��ȡDTU SIM������
*   ��    ��: s ָ��洢13λ����ĵ�ַ,�ռ���14�ֽ�
*	�� �� ֵ: ��
*********************************************************************************************************
*/
uint8_t DTU_GetSimNum(char *s)
{
    char *p;
    if (ME_SendAtCmd("AT+CNUM", "OK", 100) == 0)                     /* ��ȡSIM������ */
    {
        if ((p = strstr(GPRS_RxBufTmp, "+CNUM:")) != NULL)           /* ��ͨ�� */
        {
            p += 6; 
            p = strchr(p, '+');
            p++;
            strncpy(s, p, 13);                                       //+CNUM: "","+8618620747949",145
            return 0;                                                /* ��ȡ�ɹ�*/
        }
    }
    return 1;                                                        /* ��ȡʧ�� */
}

/************** (C) COPYRIGHT ��������Ƽ���չ���޹�˾ ********END OF FILE****/

