/******************* Copyright(c) ��������Ƽ���չ���޹�˾ *******************
* 
*------File Info--------------------------------------------------------------
* File Name:            LCD.c
* Latest modified Date: 
* Latest Version:       
* Description:          ����Һ����ʾ��DC80480B070_03��غ���
* compiler:             MDK V4.73
* MCU:                  STM32F103VE
* Oscillator Crystal Frequency:    8MHz
*-----------------------------------------------------------------------------
* Created By:    �����
* Created date:  2015-08-26
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
#include "Task_ADC.h"
#include <stdlib.h>

/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
#define LCD_SET_PSW (123456)                      //������ʾ���������ý��������(���ܳ���9������)
/* Private macro ------------------------------------------------------------*/
const uint8_t LCD_CMD_BUF_MAX = 100;
/* Private variables --------------------------------------------------------*/
static char Lcd_CmdBuf[LCD_CMD_BUF_MAX];        //������ʾ���������
static uint8_t Lcd_CmdBufCnt = 0;               //������ʾ�������������������
volatile uint8_t LCD_ScreenId = 0;              //������ʾ����ǰ����ID,���ڸ�����ʾ
uint8_t fan_last_state = 0;                     //�����һ�εĿ���״̬
uint8_t clean_last_state = 0;                   //��������һ�εĿ���״̬
static uint8_t Lcd_ScreenSwitch = 0;            //�����л���־λ 0:����û�л�; 1:�������л�


static osMutexId osMutexLcd;            /* LCD�ӿڻ����ź��� */

/* Private function prototypes ----------------------------------------------*/
static void LCD_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen);
/* Declaration of extern functions ------------------------------------------*/
/* Global variables ---------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                          Private functions                                */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                            function code                                  */
/*---------------------------------------------------------------------------*/

/******************************************************************************
  Function:     LCD_Icon
  Description:  ����LCDͼ��״̬
  Input:        uint8_t screen_id,  ��ĻID��0~255
                uint8_t icon_id,    ͼ��ID��0~255
                uint8_t state       ָ��״̬0~255
  Return:       none
  Others:       none
******************************************************************************/
void LCD_Icon(uint8_t screen_id, uint8_t icon_id, uint8_t state)
{
    char buf[12] = { 0xEE, 0xB1, 0x23, 0, 0, 0, 0, 0, 0xFF, 0xFC, 0xFF, 0xFF };
    buf[4] = screen_id;
    buf[6] = icon_id;
    buf[7] = state;
    LCD_SendBuf((uint8_t*)buf, 12);
}

/******************************************************************************
  Function:     LCD_Gif
  Description:  LCD�����ؼ�����,Ŀǰ��֧�ֿ��ƶ������ź�ֹͣ
  Input:        uint8_t screen_id,  ��ĻID��0~255
                uint8_t control_id, �ؼ�ID��0~255
                uint8_t state       ����״̬0:ֹͣ; 1:����
  Return:       none
  Others:       none
******************************************************************************/
void LCD_Gif(uint8_t screen_id, uint8_t control_id, uint8_t state)
{
    char buf[11] = { 0xEE, 0xB1, 0x21, 0, 0, 0, 0, 0xFF, 0xFC, 0xFF, 0xFF };
    if (state == 0)
    {
        buf[2] = 0x21;
    }
    else
    {
        buf[2] = 0x20;
    }

    buf[4] = screen_id;
    buf[6] = control_id;

    LCD_SendBuf((uint8_t *)buf, 11);
}

/******************************************************************************
  Function:     LCD_Txt
  Description:  �����ı��ؼ���ʾ����
  Input:        uint8_t screen_id,  ��ĻID��0~255
                uint8_t control_id, �ؼ�ID��0~255
                uint8_t str         ��ʾ�ַ���
  Return:       none
  Others:       none
******************************************************************************/
void LCD_Txt(uint8_t screen_id, uint8_t icon_id, char *str)
{
//  char buf[12] = { 0xEE, 0xB1, 0x10, 0, 0, 0, 0 };//Ӧ�ò���Ҫ12��ô��
    char buf[7] = { 0xEE, 0xB1, 0x10, 0, 0, 0, 0 };
    const char buf1[] = { 0xFF, 0xFC, 0xFF, 0xFF };
    buf[4] = screen_id;
    buf[6] = icon_id;
  LCD_SendBuf( (uint8_t*)buf, 7);
  LCD_SendBuf((uint8_t*)str,strlen(str));
  LCD_SendBuf( (uint8_t*)buf1, 4);

//    osMutexWait(osMutexLcd, 60000);
//    comSendBuf(LCD_COM, buf, 7);
//    comSendString(LCD_COM, str);
//    comSendBuf(LCD_COM, (uint8_t *)buf1, 4);
//    osMutexRelease(osMutexLcd);
}

/******************************************************************************
  Function:     LCD_SetScreen
  Description:  ���ô�����ʾ���л�������Ļ���
  Input:        none
  Return:       none
  Others:       none
******************************************************************************/
void LCD_SetScreen(uint8_t screen_id)
{
    char buf[9] = { 0xEE, 0xB1, 0x00, 0, 0, 0xFF, 0xFC, 0xFF, 0xFF };

    buf[4] = screen_id;
    LCD_ScreenId = screen_id;
    Lcd_ScreenSwitch = 1;                   //�����л�,��λ��־λ

    LCD_SendBuf((uint8_t*)buf, 9);
}

/******************************************************************************
  Function:     UpdateLcdDisplay
  Description:  ���´�����ʾ������ʾ����
  Input:        none
  Return:       none
  Others:       none
******************************************************************************/
void UpdateLcdDisplay(void)
{
    static uint8_t fan_last_state   = 0;                    //�����һ�εĿ���״̬
    static uint8_t clean_last_state = 0;                    //��������һ�εĿ���״̬
    uint8_t i;
    char string_tmp[20];

    switch (LCD_ScreenId)
    {
    case 0: //���´�����ʾ����Ӧ�������ʾ��Ϣ
        UpdateLcdTime();    //������ʾ����ʱ��
        if (fan_last_state != ChannelState[g_tParam.FanChannel])
        {
            fan_last_state = ChannelState[g_tParam.FanChannel];
            if (ChannelState[g_tParam.FanChannel] == 1)
            {
                LCD_Icon(0, 7, 1);                             //��ʾ�������ͼ��
                LCD_Gif(0, 5, 1);                              //���ŷ�����ж���
            }
            else
            {
                LCD_Icon(0, 7, 0);                             //��ʾ���ֹͣ����ͼ��
                LCD_Gif(0, 5, 0);                              //ֹͣ������ж���
            }
        }

        if (clean_last_state != ChannelState[g_tParam.CleanChannel])
        {
            clean_last_state = ChannelState[g_tParam.CleanChannel];
            if (ChannelState[g_tParam.CleanChannel] == 1)
            {
                LCD_Icon(0, 8, 1);                             //��ʾ����������ͼ��
                LCD_Gif(0, 4, 1);                              //���ž��������ж���
            }
            else
            {
                LCD_Icon(0, 8, 0);                             //��ʾ������ֹͣ����ͼ��
                LCD_Gif(0, 4, 0);                              //ֹͣ���������ж���
            }
        }
        break;
    case 2:
#if ADC_EN
        /* ǰ4��ͨ��Ϊ����ѹ,��ʾΪ0 */
        for (i = 0; i < 4; i++)
        {
            LCD_Txt(2, i + 3, "0.0");
        }

        for (i = 4; i < 8; i++)
        {
//          sprintf(string_tmp, "%d.%03d", AinValue[i] / 1000, AinValue[i] % 1000);//����С����3λ,mA
            sprintf(string_tmp, "%d.%01d", AinValue[i] / 1000, AinValue[i] % 1000/100);//����С����1λ
            LCD_Txt(2, i + 3, string_tmp);
        }
#endif
        break;
    case 3:
        if (Lcd_ScreenSwitch)
        {
            Lcd_ScreenSwitch = 0;

            LCD_Txt(3, 2, g_tParam.DevNO); //��ʾ�豸��
            sprintf(string_tmp, "%d.%d.%d.%d", g_tParam.Server[0].ip[0], g_tParam.Server[0].ip[1], g_tParam.Server[0].ip[2], g_tParam.Server[0].ip[3]);
            LCD_Txt(3, 3, string_tmp);                          //��ʾ������1��IP
            sprintf(string_tmp, "%d", g_tParam.Server[0].port);
            LCD_Txt(3, 4, string_tmp);                          //��ʾ������1�Ķ˿�
            sprintf(string_tmp, "%d", g_tParam.u16_SaveRtdInterval);
            LCD_Txt(3, 6, string_tmp);                          //�������ݵ�ʱ����

            sprintf(string_tmp, "%04d%02d%02d%", DateCurrent.Year + 2000, DateCurrent.Month, DateCurrent.Date);
            LCD_Txt(3, 7, string_tmp);                          //��ʾ����

            sprintf(string_tmp, "%02d%02d%02d%", TimeCurrent.Hours, TimeCurrent.Minutes, TimeCurrent.Seconds);
            LCD_Txt(3, 8, string_tmp);                          //��ʾʱ��

            LCD_Txt(3, 5, g_tParam.apn);                        //��ʾAPN
            
            /* ��ʾ������ */
            switch (Isp)
            {
            case 0:
                LCD_Txt(3, 15, "�й��ƶ�");
                break;
            case 1:
                LCD_Txt(3, 15, "�й���ͨ");
                break;
            case 2:
                LCD_Txt(3, 15, "�й���ͨ");
                break;
            default:
                break;
            }
            
            sprintf(string_tmp, "%d", g_tParam.CleanChannel);
            LCD_Txt(3, 9, string_tmp);                          //��ʾ��������ͨ��ֵ
            sprintf(string_tmp, "%d", g_tParam.FanChannel);
            LCD_Txt(3, 10, string_tmp);                         //��ʾ�����ͨ��ֵ
        }
        break;
    default:
        break;
    }
}

/******************************************************************************
  Function:     UpdateLcdTime
  Description:  ���´�����ʾ����ʱ��
  Input:        none
  Return:       none
  Others:       none
******************************************************************************/
void UpdateLcdTime(void)
{
    static int last_sec = 0;            //������һ����Ķ���,���ڼ��RTC�Ƿ�ֹͣ
    static uint8_t cnt = 0;             //���ڼ��RTC�Ƿ�ֹͣ

    char string_tmp[30];//Ӧ��ֻ��Ҫ20B

    HAL_RTC_GetTime(&hrtc, &TimeCurrent, RTC_FORMAT_BIN);
    if (last_sec == TimeCurrent.Seconds)
    {
        if (++cnt > 5)
        {
            //��������RTC
//          RTC_Configuration();
//          //������ɺ���󱸼Ĵ�����д�����ַ�0xA5A5
//          BKP_WriteBackupRegister(BKP_DR1, 0xA5A5);
//          Time_SetCalendarTime(time_now);

//          HAL_RTC_Init(&hrtc);
            cnt = 0;
        }
    }
    else
    {
        cnt = 0; //������ӵ�,ԭ�����з���
    }
    last_sec = TimeCurrent.Seconds;

    sprintf(string_tmp, "%4d-%02d-%02d %02d:%02d:%02d", DateCurrent.Year + 2000, DateCurrent.Month,\
                DateCurrent.Date, TimeCurrent.Hours, TimeCurrent.Minutes, TimeCurrent.Seconds);
    
    LCD_Txt(0, 10, string_tmp);                             //����ʱ����ʾ
}

/******************************************************************************
  Function:     GetLcdCmd
  Description:  �Ӵ��ڻ������л�ȡһ֡����Һ����ʾ������,����Lcd_CmdBuf[]
  Input:        none
  Return:       0:      ��ȡʧ��
                1~255:  ��ȡ�ɹ����������
  Others:       none
******************************************************************************/
uint8_t GetLcdCmd(void)
{
    static uint8_t Lcd_CmdState = 0;                //������ʾ������֡����״̬
    uint8_t _data = 0;

    while (comGetChar(LCD_COM, &_data))
    {
        //ȡһ������
//      comGetChar(LCD_COM, &_data);

        if (Lcd_CmdBufCnt == 0 && _data != 0xEE) //֡ͷ��������
            continue;

        //���������,���������,�˳�
        if (Lcd_CmdBufCnt >= LCD_CMD_BUF_MAX)
        {
            Lcd_CmdBufCnt = 0;
            Lcd_CmdState = 0;
            return 0;
        }
        Lcd_CmdBuf[Lcd_CmdBufCnt++] = _data;

        //�ж�֡β
        if (_data == 0xFF)
        {
            switch (Lcd_CmdState)
            {
            case 2:
                Lcd_CmdState = 3;
                break; //FF FC FF ?? (���һ���ֽڲ���)
            case 3:
                Lcd_CmdState = 4;
                break; //FF FC FF FF ��ȷ��֡β
            default:
                Lcd_CmdState = 1;
                break; //FF ?? ?? ??(��������ֽڲ���)
            }
        }
        else if (_data == 0xFC)
        {
            switch (Lcd_CmdState)
            {
            case 1:
                Lcd_CmdState = 2;
                break; //FF FC ?? ??(�������ֽڲ���)
            case 3:
                Lcd_CmdState = 2;
                break; //FF FC FF FC ��ȷ��֡β
            default:
                Lcd_CmdState = 0;
                break; //?? ?? ?? ??(ȫ���ֽڲ���)
            }
        }
        else
            Lcd_CmdState = 0;

        //�õ�������֡β
        if (Lcd_CmdState == 4)
        {
            Lcd_CmdState = 0;
            Lcd_CmdBufCnt = 0;
            return 1;
        }
    }

    return 0; //û���γ�������һ֡
}

/******************************************************************************
  Function:     LCD_CmdHandle
  Description:  ��������ʾ���ķ��ص�����
  Input:        none
  Return:       none
  Others:       none
******************************************************************************/
uint8_t LCD_CmdHandle(void)
{
    static uint32_t password = 0;                   //����������ý��������
    uint16_t response_id;                            //��8λ�ǻ���id,��8λ�ǿؼ�id
    char *p;
    char str_tmp[10];
    uint8_t i;
    uint16_t j;

    if (GetLcdCmd() == 0)                              //δ���յ�������ʾ������֡
    {
        return 0;
    }

    //��������֡
    response_id = (uint16_t)(Lcd_CmdBuf[4] << 8);     //��ȡ����id
    response_id |= (uint8_t)Lcd_CmdBuf[6];          //��ȡ�ؼ�id

    if (Lcd_CmdBuf[7] == 0x11)                      //�����ı�����Ϣ
    {
        switch (response_id)
        {
        case 0x0101:                                //�����ı���
            password = atol(&Lcd_CmdBuf[8]);
            break;
        case 0x0302:                                //�����豸��
            p = &Lcd_CmdBuf[8];
            if (strlen(p) > 16)                      //��������ַ����Ƿ���ȷ
            {
                LCD_Txt(3, 17, "�豸�Ź��������ܴ���16���ַ���");
            }
            else
            {
                strcpy(g_tParam.DevNO, p);
                SaveParam();
                LCD_Txt(3, 17, "�豸�����óɹ�");
            }
            break;
        case 0x0303:                                //���������1��ַ
            p = &Lcd_CmdBuf[8];
            for (j = 0; j < 4; j++)
            {
                i = 0;
                while (*p >= '0' && *p <= '9' && i < 3)
                {
                    str_tmp[i++] = *p++;
                }
                str_tmp[i] = '\0';
                g_tParam.Server[0].ip[j] = atoi(str_tmp);
                p++;                                //�����ַ�'.'
            }
            SaveParam();
            LCD_Txt(3, 17, "������IP��ַ���óɹ�");
            GprsInitRequest = 1;                    //�������³�ʼ��GPRS
            break;
        case 0x0304:                                //�������1�˿ں�
            p = &Lcd_CmdBuf[8];
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 5)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Server[0].port = atoi(str_tmp);
            SaveParam();
            LCD_Txt(3, 17, "�˿ں����óɹ�");
            GprsInitRequest = 1;                    //�������³�ʼ��GPRS
            break;
        case 0x0305:                                //����APN
            p = &Lcd_CmdBuf[8];
            if (strlen(p) > 31)                       //��������ַ����Ƿ���ȷ
            {
                LCD_Txt(3, 17, "APN��������31���ַ�");
            }
            else
            {
                i = 0;
                while (*p > 0 && *p <= 128 && i < 31)
                {
                    g_tParam.apn[i++] = *p++;
                }
                g_tParam.apn[i] = '\0';
                SaveParam();
                LCD_Txt(3, 17, "APN���óɹ�");
                GprsInitRequest = 1;                    //�������³�ʼ��GPRS
            }
            break;
        case 0x0306:                                    //����ʱ����(�ͷ�����1�ϴ����ݵ�ʱ������ͬ)
            p = &Lcd_CmdBuf[8];
            if (strlen(p) > 5)                          //��������ַ����Ƿ���ȷ
            {
                LCD_Txt(3, 17, "����ʱ�����");
            }
            else
            {
                i = 0;
                while (*p >= '0' && *p <= '9' && i < 5)
                {
                    str_tmp[i++] = *p++;
                }
                str_tmp[i] = '\0';
                g_tParam.Server[0].rtd_interval = g_tParam.u16_SaveRtdInterval = atoi(str_tmp);
                SaveParam();
                LCD_Txt(3, 17, "ʱ�������óɹ�");
            }
            break;
        case 0x0307:                                //��������
            p = &Lcd_CmdBuf[8];
            if (strlen(p) == 8)                      //��������ַ����Ƿ���ȷ
            {
                str_tmp[0] = *p++;
                str_tmp[1] = *p++;
                str_tmp[2] = *p++;
                str_tmp[3] = *p++;
                str_tmp[4] = '\0';
                j = atoi(str_tmp);
                if (j < 2000)
                {
                    LCD_Txt(3, 17, "������ݴ���");
                    break;
                }
                DateCurrent.Year = j - 2000;
                str_tmp[0] = *p++;
                str_tmp[1] = *p++;
                str_tmp[2] = '\0';
                j = atoi(str_tmp);
                if (j > 13)
                {
                    LCD_Txt(3, 17, "�����·ݴ���");
                    break;
                }
                DateCurrent.Month = j;
                str_tmp[0] = *p++;
                str_tmp[1] = *p++;
                str_tmp[2] = '\0';
                j = atoi(str_tmp);
                if (j > 31)
                {
                    LCD_Txt(3, 17, "���ڴ���");
                    break;
                }
                DateCurrent.Date = j;

                HAL_RTC_SetDate(&hrtc, &DateCurrent, RTC_FORMAT_BIN);
                RTC_BackupDate(&hrtc,&DateCurrent);
                LCD_Txt(3, 17, "�������óɹ�");
            }
            else
            {
                LCD_Txt(3, 17, "ʱ���ʽ����");
            }
            break;
        case 0x0308:                                //����ʱ��
            p = &Lcd_CmdBuf[8];
            if (strlen(p) == 6)                      //��������ַ����Ƿ���ȷ
            {
                str_tmp[0] = *p++;
                str_tmp[1] = *p++;
                str_tmp[2] = '\0';
                j = atoi(str_tmp);
                if (j > 23)
                {
                    LCD_Txt(3, 17, "Сʱ����");
                    break;
                }
                TimeCurrent.Hours = j;
                str_tmp[0] = *p++;
                str_tmp[1] = *p++;
                str_tmp[2] = '\0';
                j = atoi(str_tmp);
                if (j > 59)
                {
                    LCD_Txt(3, 17, "���Ӵ���");
                    break;
                }
                TimeCurrent.Minutes = j;
                str_tmp[0] = *p++;
                str_tmp[1] = *p++;
                str_tmp[2] = '\0';
                j = atoi(str_tmp);
                if (j > 59)
                {
                    LCD_Txt(3, 17, "�����");
                    break;
                }
                TimeCurrent.Seconds = j;
                HAL_RTC_SetTime(&hrtc, &TimeCurrent, RTC_FORMAT_BIN);
                LCD_Txt(3, 17, "ʱ�����óɹ�");
            }
            else
            {
                LCD_Txt(3, 17, "ʱ���ʽ����");
            }
            break;
        case 0x0309:                                //���뾻����ͨ��
            g_tParam.CleanChannel = atoi(&Lcd_CmdBuf[8]);
            SaveParam();
            LCD_Txt(3, 17, "������ͨ�����óɹ�");
            break;
        case 0x030a:                                //������ͨ��
            g_tParam.FanChannel = atoi(&Lcd_CmdBuf[8]);
            SaveParam();
            LCD_Txt(3, 17, "���ͨ�����óɹ�");
            break;
        default:
            break;
        }
    }
    else if (Lcd_CmdBuf[7] == 0x10)                 //���ص��ǰ����ؼ���Ϣ
    {
        switch (response_id)
        {
        case 0x0001:                                //����0�ġ����ü���
            LCD_SetScreen(1);                       //�л��������������
            break;
        case 0x0102:                                //����ȷ�ϼ�
            if (password == LCD_SET_PSW)
            {
                LCD_SetScreen(2);                   //�������ý���
                password = 0;
            }
            break;
        case 0x0103:                                //����ȡ����
            password = 0;
            LCD_SetScreen(0);                       //�л�������0
            break;
        case 0x0201:                                //����2�ġ�������ҳ����
            LCD_SetScreen(0);                       //�л���0����
            break;
        case 0x020F:                                //����2�ġ���һҳ��
            LCD_SetScreen(3);                       //�л�������3
            LCD_Txt(3, 17, "");
            break;
        case 0x030E:                                //����3�ġ���һҳ��
            LCD_SetScreen(2);                       //�л�������2
            break;
        case 0x0310:                                //����3�ġ�������ҳ����
            LCD_SetScreen(0);                       //�л�������0
            break;
        case 0x0402:                                //����4�ġ�������ҳ����
            LCD_SetScreen(0);                       //�л�������0
            break;
        default:
            break;
        }
    }
    return 1;
}

/******************************************************************************
  Function:     DisplayNetState
  Description:  ��LCD����ʾ����״̬
  Input:        none
  Return:       none
  Others:       none
******************************************************************************/
void DisplayNetState(void)
{
    char str_tmp[50];

    if (GPRS_State == GPRS_OK)
    {
        GprsSignalLevel = DTU_HCSQ();                            //��ѯ�ź�ǿ��
        printf("Net type is %d, level %d.\r\n", NetType, GprsSignalLevel);
        if (GprsSignalLevel < 255)
        {
            sprintf(str_tmp, "����������   �ź�ǿ��:%2d", (uint8_t)(GprsSignalLevel / 10));
            SetOutputBit(0, ON);
        }
        else
        {
            sprintf(str_tmp, "�����ѶϿ�");
            SetOutputBit(0, OFF);
        }
        LCD_Txt(0, 6, str_tmp);
    }
    else if (GPRS_State == NO_SIM_CARD)
    {
        LCD_Txt(0, 6, "��������ʧ��: ��SIM��");
    }
    else
    {
        LCD_Txt(0, 6, "��������ʧ��");
    }
}


/******************************************************************************
  Function:     LCD_SendBuf
  Description:  ������Ϣ��������
  Input:        none
  Return:       none
  Others:       none
******************************************************************************/
static void LCD_SendBuf(uint8_t *_ucaBuf, uint16_t _usLen)
{
    osMutexWait(osMutexLcd, 0);
    comSendBuf(LCD_COM, _ucaBuf, _usLen);
    osMutexRelease(osMutexLcd);
}

/******************************************************************************
  Function:     LCD_Init
  Description:  ��������ʼ��
  Input:        none
  Return:       none
  Others:       none
******************************************************************************/
void LCD_Init(void)
{
    osMutexLcd = osMutexCreate(NULL);
}
/************** (C) COPYRIGHT ��������Ƽ���չ���޹�˾ ********END OF FILE****/

