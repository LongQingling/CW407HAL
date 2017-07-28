/*
*********************************************************************************************************
*
*	ģ������ : ����ʽ����������ģ��
*	�ļ����� : task_gprs.c
*	��    �� : V1.0
*	˵    �� : MB_HVOC��gprs����Ӧ�ó���
*	�޸ļ�¼ :
*	�汾��   ����        ����    ˵��
*   v1.0    2016-02-23 �����  ST�̼���V3.6.1�汾��
*
*	Copyright (C), 2016-2026, ��������Ƽ� www.zhenghongkeji.com
*
*********************************************************************************************************
*/
#include "bsp.h"
#include "Task_ADC.h"
#include "ff.h"
#include "protocol.h"

/*
*********************************************************************************************************
*                               ADC��ض���
*********************************************************************************************************
*/


/*
*********************************************************************************************************
*                               ȫ�ֱ���
*********************************************************************************************************
*/
static uint8_t ucCnt1_t1s = 0;
static uint8_t ucCnt2_t1s = 0;
static uint8_t ucCnt3_t1s = 0;

/*
*********************************************************************************************************
*                               ���غ���
*********************************************************************************************************
*/


#if SD_CARD_EN
/******************************************************************************
  Function:     Write_Data
  Description:  �Ѳɼ�����д��SD��CSV�ļ��е����
  Input:        char *s ��Ҫд����ַ���
  Output:       none
  Return:       0���ɹ�������������
  Others:       ������Ϊ�ļ���
******************************************************************************/
char Write_Data(char *s)
{
    char tmp[200];
    uint8_t i;
    FRESULT g_fres;
    UINT bw;
    FIL fs;

    sprintf(tmp, "%04d%02d%02d.csv", DateCurrent.Year + 2000, DateCurrent.Month, DateCurrent.Date);

    //��¼��SD��
    g_fres = f_open(&fs, tmp, FA_OPEN_EXISTING | FA_WRITE);
    if (g_fres != 0)
    {
        i = 2;
        do
        {
            g_fres = f_open(&fs, tmp, FA_WRITE | FA_CREATE_NEW);     //�½�
            if (g_fres)
            {
                CheckSDCardSpace();                                  //�½�ʧ�ܣ������̿ռ䣬�����Ƿ���Ҫɾ�����������ļ�
            }
        }while (g_fres && i--);

        sprintf(tmp, "Date(YYYYMMDD),Time(HHMMSS),Channel1,Channel2,Chanel3,Channel4,Channel5,Channel6,Channel7,Channel8,Temperature,Humidity,AirPressure,Concentration\r\n");
        g_fres = f_write(&fs, tmp, strlen(tmp), &bw);
        if (g_fres)
        {
            return g_fres;                                           //д���������
        }
    }
    f_lseek(&fs, fs.fsize);                                          //���ļ�ָ���ƶ��ļ����
    g_fres = f_write(&fs, s, strlen(s), &bw);
    f_sync(&fs);
    f_close(&fs);
    return g_fres;
}
#endif

/******************************************************************************
  Function:     SaveAdValue
  Description:  ��ADC����ֵ������TF���ļ���
  Input:        none
  Return:       none
  Others:       none
******************************************************************************/
void SaveAdValue(void)
{
    char string_tmp[300] = { 0 };

    sprintf(string_tmp, "%04d%02d%02d,%02d%02d%02d,0.0,0.0,0.0,0.0,%d.%01d,%d.%01d,%d.%01d,%d.%01d\r\n",\
                2000 + DateCurrent.Year, DateCurrent.Month, DateCurrent.Date,\
                TimeCurrent.Hours, TimeCurrent.Minutes, TimeCurrent.Seconds,\
                AinValue[4] / 1000, AinValue[4] % 1000 / 100, AinValue[5] / 1000, AinValue[5] % 1000 / 100,\
                AinValue[6] / 1000, AinValue[6] % 1000 / 100, AinValue[7] / 1000, AinValue[7] % 1000 / 100);
#if SD_CARD_EN
    Write_Data(string_tmp);                                          //���浽CSV�ļ���
#endif
}

/**
  * @brief  Event_OneMinute function 
  *         �����¼�
  * @param  None
  * @retval None
  */
static void Event_OneMinute(void)
{
    static uint16_t save_rtd_cnt = 0;
    static uint16_t srv1_cnt = 0;
    static uint16_t srv2_cnt = 0;
    static uint16_t srv3_cnt = 0;
    static uint16_t srv4_cnt = 0;
    static __IO uint8_t last_hours = 0;                                   //������һ�η�����

    /* ��ȡʱ������ */
    HAL_RTC_GetTime(&hrtc, &TimeCurrent, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &DateCurrent, RTC_FORMAT_BIN);

    /* ����ADֵ */
    if (g_tParam.u16_SaveRtdInterval)
    {
        save_rtd_cnt++;
        if (save_rtd_cnt >= g_tParam.u16_SaveRtdInterval)
        {
            save_rtd_cnt = 0;
            SaveAdValue();                                           //����Rtdֵ��TF��
        }
    }

    /* ������1 */
    if (g_tParam.Server[0].rtd_interval && g_tParam.Server[0].port)
    {
        srv1_cnt++;
        if (srv1_cnt >= g_tParam.Server[0].rtd_interval)
        {
            srv1_cnt = 0;
            UpLoadData(0);                                           //�ϴ����ݵ�������1
        }
    }

    /* ������2 */
    if (g_tParam.Server[1].rtd_interval && g_tParam.Server[1].port)
    {
        srv2_cnt++;
        if (srv2_cnt >= g_tParam.Server[1].rtd_interval)
        {
            srv2_cnt = 0;
            UpLoadData(1);                                           //�ϴ����ݵ�������2
        }
    }

    /* ������3 */
    if (g_tParam.Server[2].rtd_interval && g_tParam.Server[2].port)
    {
        srv3_cnt++;
        if (srv3_cnt >= g_tParam.Server[2].rtd_interval)
        {
            srv3_cnt = 0;
            UpLoadData(2);                                           //�ϴ����ݵ�������3
        }
    }

    /* ������4 */
    if (g_tParam.Server[3].rtd_interval && g_tParam.Server[3].port)
    {
        srv4_cnt++;
        if (srv4_cnt >= g_tParam.Server[3].rtd_interval)
        {
            srv4_cnt = 0;
            UpLoadData(3);                                           //�ϴ����ݵ�������1
        }
    }

    QR_Update();                                                     /* ���¶�ά�� */

    if (last_hours != TimeCurrent.Hours)                             /* 1Сʱ�¼� */
    {
        last_hours = TimeCurrent.Hours;

        /* ÿ��0ʱ0������ϵͳ,��������ɱ�������ʱ��0:00�򲻶����� */
        if (TimeCurrent.Hours == 0 && TimeCurrent.Minutes == 0)
        {
#if DEBUG_EN
            DEBUG_PrintfString("Is time to reset system.\r\n");
            osDelay(100);
#endif
//            __set_FAULTMASK(1);                                          // �ر������ж�
////          __disable_fault_irq();                                       //��ֹfault�ж�
//            NVIC_SystemReset();                                          //����MCU

            IwdgRefreshEn = 0;
        }

        CheckSDCardSpace();
    }
}

/*
*********************************************************************************************************
*	�� �� ��: DTU_HeartbeartTmrCallback
*	����˵��: UCOS�����ʱ��DTU_HeartbeartTmr�Ļص�����
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void DTU_HeartbeartTmrCallback(void *ptmr, void *callback_arg)
{
    //  uint8_t i;
    //  uint8_t temp = g_ucConnectEnable;
    //
    //  for (i=0;i<8;i++)
    //  {
    //      if (temp & 0x01)
    //      {
    //          g_ucaHeartbeatCnt[i]++;
    //      }
    //      temp >>= 1;
    //  }
}

void UpLoadTestData(void)
{
    static uint32_t cnt = 0;
    char str_temp[30];
    sprintf(str_temp, "Test data: %010d\r\n", cnt++);                //Test data: 0000002975
    ME_TCPSendString(0, str_temp, 100);
}

/*
*********************************************************************************************************
*	�� �� ��: GprsTimer
*	����˵��: GPRS��ʱ��,����һ��,Ŀǰ�����������
*	��    �Σ���
*	�� �� ֵ: ��
*********************************************************************************************************
*/
void GprsTimer(void)
{
    ucCnt1_t1s++;
    ucCnt2_t1s++;
    ucCnt3_t1s++;
    ME_HeartbeartTmrCallback();
}

/*
*********************************************************************************************************
*	�� �� ��: Task_GPRS
*	����˵��: GPRS������
*	��    �Σ���
*	�� �� ֵ: ��
    �� �� ����5
*********************************************************************************************************
*/
void Task_GPRS(void const *argument)
{
    static uint8_t last_minutes = 0;
    uint32_t i; 

    //  static osTimerId gprs_timer = NULL;    //���涨ʱ�����
    //
    //  gprs_timer = osTimerCreate(osTimerDef_t(GprsTimerCallback),osTimerPeriodic,0);
    //  if (gprs_timer != NULL)
    //  {
    //      osTimerStart(gprs_timer, 1000);
    //  }

#if SD_CARD_EN

    SdCardInit();
    //  TestSdCard();
#endif

    DTU_Init();

    while (1)
    {
        /* ���ӵ����������� */
        ME_Heartbeart();

        /* ���������ļ��ʱ���� */
        if (ucCnt1_t1s > 10)
        {
            ucCnt1_t1s = 0;
            ME_CheckConnection();
            ME_Reconnection();
        }

        /* ����״̬����ʱ���� */
        if (ucCnt2_t1s > 30)
        {
            ucCnt2_t1s = 0;
            DTU_CheckState();                                        //���DTU�Ƿ�����
            DisplayNetState();                                       /* ��Ⲣ��ʾ��������������ź�ǿ�� */
        }

        /* �����¼� */
        if (ucCnt3_t1s > 4)
        {
            ucCnt3_t1s = 0;
            //          UpLoadTestData();                                        //����ʱ��������
        }

        if (last_minutes != TimeCurrent.Minutes)
        {
            last_minutes = TimeCurrent.Minutes;
            Event_OneMinute();
        }
#if GPRS_EN
        /* ����GPRSģ����Ϣ */
        DTU_HandleRecData();
#endif

        /* �������³�ʼ�����ӵ����� */
        if (GprsInitRequest)
        {
            GprsInitRequest = 0;
            for (i=0;i<4;i++)
            {
                ME_Disconnect(1,i);
            }
            osDelay(100);
            for (i=0;i<4;i++)
            {
                if (g_tParam.Server[i].port != 0)
                {
                    ME_ConnectSever(i);
                }
            }
        }
        osDelay(100);
    }
}



/*
*********************************************************************************************************
*	�� �� ��: GPIO_Configuration
*	����˵��: ����STM32 GPIO
*	��    ��:  ��
*	�� �� ֵ: ��
*********************************************************************************************************
*/


/* ************************* ��������Ƽ� www.zhenghongkeji.com/ (END OF FILE) ************************* */
