/****************** Copyright(c) ��������Ƽ���չ���޹�˾ ***********************
* 
*------File Info--------------------------------------------------------------
* File Name:            Protocol.c
* Latest modified Date: 
* Latest Version:       
* Description:          ʵ��212Э��
* compiler:             MDK V4.73
* MCU:                  STM32F103VE
* Oscillator Crystal Frequency:    18.432MHz
*-----------------------------------------------------------------------------
* Created By:    �����
* Created date:  2015-07-15
* Version:       v0.1
* Descriptions:  ʵ��212Э��
*
*-----------------------------------------------------------------------------
* Modified by:   �޸��˵�����
* Modified date: �ļ����޸����ڣ�YYYY-MM-DD��
* Version:       �ļ��޶��İ汾��
* Description:   �ļ��޶��ļ�Ҫ����
*
******************************************************************************/

/* Includes -----------------------------------------------------------------*/
#include "app.h"

/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private variables --------------------------------------------------------*/
static const uint16_t CRC212_Tab[256] = { //HJ212 crc���
    0x0000, 0xc0c1, 0xc181, 0x0140, 0xc301, 0x03c0, 0x0280, 0xc241,
    0xc601, 0x06c0, 0x0780, 0xc741, 0x0500, 0xc5c1, 0xc481, 0x0440,
    0xcc01, 0x0cc0, 0x0d80, 0xcd41, 0x0f00, 0xcfc1, 0xce81, 0x0e40,
    0x0a00, 0xcac1, 0xcb81, 0x0b40, 0xc901, 0x09c0, 0x0880, 0xc841,
    0xd801, 0x18c0, 0x1980, 0xd941, 0x1b00, 0xdbc1, 0xda81, 0x1a40,
    0x1e00, 0xdec1, 0xdf81, 0x1f40, 0xdd01, 0x1dc0, 0x1c80, 0xdc41,
    0x1400, 0xd4c1, 0xd581, 0x1540, 0xd701, 0x17c0, 0x1680, 0xd641,
    0xd201, 0x12c0, 0x1380, 0xd341, 0x1100, 0xd1c1, 0xd081, 0x1040,
    0xf001, 0x30c0, 0x3180, 0xf141, 0x3300, 0xf3c1, 0xf281, 0x3240,
    0x3600, 0xf6c1, 0xf781, 0x3740, 0xf501, 0x35c0, 0x3480, 0xf441,
    0x3c00, 0xfcc1, 0xfd81, 0x3d40, 0xff01, 0x3fc0, 0x3e80, 0xfe41,
    0xfa01, 0x3ac0, 0x3b80, 0xfb41, 0x3900, 0xf9c1, 0xf881, 0x3840,
    0x2800, 0xe8c1, 0xe981, 0x2940, 0xeb01, 0x2bc0, 0x2a80, 0xea41,
    0xee01, 0x2ec0, 0x2f80, 0xef41, 0x2d00, 0xedc1, 0xec81, 0x2c40,
    0xe401, 0x24c0, 0x2580, 0xe541, 0x2700, 0xe7c1, 0xe681, 0x2640,
    0x2200, 0xe2c1, 0xe381, 0x2340, 0xe101, 0x21c0, 0x2080, 0xe041,
    0xa001, 0x60c0, 0x6180, 0xa141, 0x6300, 0xa3c1, 0xa281, 0x6240,
    0x6600, 0xa6c1, 0xa781, 0x6740, 0xa501, 0x65c0, 0x6480, 0xa441,
    0x6c00, 0xacc1, 0xad81, 0x6d40, 0xaf01, 0x6fc0, 0x6e80, 0xae41,
    0xaa01, 0x6ac0, 0x6b80, 0xab41, 0x6900, 0xa9c1, 0xa881, 0x6840,
    0x7800, 0xb8c1, 0xb981, 0x7940, 0xbb01, 0x7bc0, 0x7a80, 0xba41,
    0xbe01, 0x7ec0, 0x7f80, 0xbf41, 0x7d00, 0xbdc1, 0xbc81, 0x7c40,
    0xb401, 0x74c0, 0x7580, 0xb541, 0x7700, 0xb7c1, 0xb681, 0x7640,
    0x7200, 0xb2c1, 0xb381, 0x7340, 0xb101, 0x71c0, 0x7080, 0xb041,
    0x5000, 0x90c1, 0x9181, 0x5140, 0x9301, 0x53c0, 0x5280, 0x9241,
    0x9601, 0x56c0, 0x5780, 0x9741, 0x5500, 0x95c1, 0x9481, 0x5440,
    0x9c01, 0x5cc0, 0x5d80, 0x9d41, 0x5f00, 0x9fc1, 0x9e81, 0x5e40,
    0x5a00, 0x9ac1, 0x9b81, 0x5b40, 0x9901, 0x59c0, 0x5880, 0x9841,
    0x8801, 0x48c0, 0x4980, 0x8941, 0x4b00, 0x8bc1, 0x8a81, 0x4a40,
    0x4e00, 0x8ec1, 0x8f81, 0x4f40, 0x8d01, 0x4dc0, 0x4c80, 0x8c41,
    0x4400, 0x84c1, 0x8581, 0x4540, 0x8701, 0x47c0, 0x4680, 0x8641,
    0x8201, 0x42c0, 0x4380, 0x8341, 0x4100, 0x81c1, 0x8081, 0x4040
};
/* Private function prototypes ----------------------------------------------*/
/* Private functions --------------------------------------------------------*/

/* Forward Declaration of local functions -----------------------------------*/
/* Declaration of extern functions ------------------------------------------*/
/* Global variables ---------------------------------------------------------*/

char Connect0_RecBuf[CONNECT_BUFFER_SIZE]; //��·0������Ϣ��buffer
uint16_t Connect0_RecBufCnt = 0;

char Connect1_RecBuf[CONNECT_BUFFER_SIZE]; //��·1������Ϣ��buffer
uint16_t Connect1_RecBufCnt = 0;

char Connect2_RecBuf[CONNECT_BUFFER_SIZE]; //��·2������Ϣ��buffer
uint16_t Connect2_RecBufCnt = 0;

char Connect3_RecBuf[CONNECT_BUFFER_SIZE]; //��·3������Ϣ��buffer
uint16_t Connect3_RecBufCnt = 0;

char g_u8_PkgBuf[UART1_TX_BUF_LEN];

/*---------------------------------------------------------------------------*/
/*                          Private functions                                */
/*---------------------------------------------------------------------------*/

/*---------------------------------------------------------------------------*/
/*                            function code                                  */
/*---------------------------------------------------------------------------*/
/******************************************************************************
  Function:     cal_crc
  Description:  ����212Э�飬����CRC
  Input:        unsigned char *ptr ָ����Ҫ����CRC�������ַ
                int len ��Ҫ������ֽ���
  Return:       CRC(Hex)
  Others:       
******************************************************************************/
/*
uint16_t cal_crc(char *ptr, int len)
{
   unsigned int crc;
   int i;
   unsigned int hib;
   crc = 0xffff;

   while (len-- != 0)
   {
      hib = crc >> 8;
      crc = (hib ^ *ptr) & 0x00ff;

      for (i = 0; i < 8; i++)
      {
         if (crc & 0x0001)
         {
            crc >>= 1;
            crc = crc ^ 0xa001;
         }
         else
         {
            crc >>= 1;
         }
      }
      ptr++;
   }
   return (crc);
}
*/
uint16_t cal_crc(char *ptr, int len)
{
    unsigned int crc = 0xffff;
    unsigned char crc_H8;

    while (len--)
    {
        crc_H8 = (unsigned char)(crc >> 8);
        crc >>= 8;
        crc = CRC212_Tab[crc_H8 ^ *ptr];
        ptr++;
    }
    return crc;
}

/******************************************************************************
  Function:     ConvertNum
  Description:  �ѱ�ʾ�޷���16�������ݵ��ַ���ת��Ϊunsigned int
  Input:        char *str ָ���ַ�����ָ��
                unsigned char length �ַ���
  Return:       unsigned int ת�����16������
  Others:       
******************************************************************************/
uint32_t ConvertNum(char *str, unsigned char length)
{
    uint8_t i;
    uint32_t sum;

    sum = 0;
    for (i = 0; i < length; i++)
    {
        if (*str >= '0' && *str <= '9')
        {
            sum *= 16;
            sum += *str - '0';
            str++;
        }
        else if (*str >= 'a' && *str <= 'f')
        {
            sum *= 16;
            sum += *str - 'a' + 10;
            str++;
        }
        else if (*str >= 'A' && *str <= 'F')
        {
            sum *= 16;
            sum += *str - 'A' + 10;
            str++;
        }
    }
    return sum;
}

/******************************************************************************
  Function:     UpLoadData
  Description:  �ϴ�����ʵʱ���ݵ�������
  Input:        uint8_t conn_id �ϴ��������õ���·ID�� 0-3(Ŀǰ��֧��4������)
  Return:       none
  Others:       none
******************************************************************************/
void UpLoadData(uint8_t conn_id)
{
    uint16_t j, k;
    char str_tmp[30];
    uint8_t res;

    //�ϴ��ֳ���ʱ��
    //time_now = Time_GetCalendarTime();  //��ȡ��ǰʱ��

    switch (g_tParam.CleanChannel)
    {
    case 0:
        sprintf(g_u8_PkgBuf, "ST=%02d;CN=2011;PW=%6d;MN=%s;Flag=0;CP=&&DataTime=%04d%02d%02d%02d%02d%02d;"
							 "SB1-Status=0,SB1-Rtd=%.1f,SB1-Conc1=%.1f,SB1-Temp1=%.1f,SB1-RH1=%.1f,SB1-AirP1=%.1f,"
							 "SB1-Conc2=%.1f,SB1-Temp2=%.1f,SB1-RH2=%.1f,SB1-AirP2=%.1f,SB1-Flag=N;"
							 "SB2-Status=0,SB2-Rtd=%.1f,SB2-Flag=N;SB3-Status=0,SB3-Rtd=%.1f,SB3-Flag=N;"
							 "SB4-Status=0,SB4-Rtd=%.1f,SB4-Flag=N;SB5-Status=0,SB5-Rtd=%.1f,SB5-Flag=N;"
							 "SB6-Status=0,SB6-Rtd=%.1f,SB6-Flag=N;SB7-Status=0,SB7-Rtd=%.1f,SB7-Flag=N;"
							 "SB8-Status=0,SB8-Rtd=%.1f,SB8-Flag=N&&",\
               g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, time_now.tm_year, time_now.tm_mon + 1, time_now.tm_mday,\
			   time_now.tm_hour, time_now.tm_min, time_now.tm_sec, v[0], nongdu1, wendu1, shidu1, airpressure1,\
			   nongdu2, wendu2, shidu2, airpressure2, v[1], v[2], v[3], v[4], v[5], v[6], v[7]);
        break;
    case 1:
        sprintf(g_u8_PkgBuf, "ST=%02d;CN=2011;PW=%6d;MN=%s;Flag=0;CP=&&DataTime=%04d%02d%02d%02d%02d%02d;"
							 "SB1-Status=0,SB1-Rtd=%.1f,SB1-Flag=N;SB2-Status=0,SB2-Rtd=%.1f,"
							 "SB2-Conc1=%.1f,SB2-Temp1=%.1f,SB2-RH1=%.1f,SB2-AirP1=%.1f,"
							 "SB2-Conc2=%.1f,SB2-Temp2=%.1f,SB2-RH2=%.1f,SB2-AirP2=%.1f,SB2-Flag=N;"
							 "SB3-Status=0,SB3-Rtd=%.1f,SB3-Flag=N;SB4-Status=0,SB4-Rtd=%.1f,SB4-Flag=N;"
							 "SB5-Status=0,SB5-Rtd=%.1f,SB5-Flag=N;SB6-Status=0,SB6-Rtd=%.1f,SB6-Flag=N;"
							 "SB7-Status=0,SB7-Rtd=%.1f,SB7-Flag=N;SB8-Status=0,SB8-Rtd=%.1f,SB8-Flag=N&&",\
                g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, time_now.tm_year, time_now.tm_mon + 1, time_now.tm_mday,\
				time_now.tm_hour, time_now.tm_min, time_now.tm_sec, v[0], v[1], nongdu1, wendu1, shidu1, airpressure1,\
				nongdu2, wendu2, shidu2, airpressure2, v[2], v[3], v[4], v[5], v[6], v[7]);
        break;
    case 2:
        sprintf(g_u8_PkgBuf, "ST=%02d;CN=2011;PW=%6d;MN=%s;Flag=0;CP=&&DataTime=%04d%02d%02d%02d%02d%02d;"
							 "SB1-Status=0,SB1-Rtd=%.1f,SB1-Flag=N;SB2-Status=0,SB2-Rtd=%.1f,SB2-Flag=N;"
							 "SB3-Status=0,SB3-Rtd=%.1f,SB3-Conc1=%.1f,SB3-Temp1=%.1f,SB3-RH1=%.1f,SB3-AirP1=%.1f,"
							 "SB3-Conc2=%.1f,SB3-Temp2=%.1f,SB3-RH2=%.1f,SB3-AirP2=%.1f,SB3-Flag=N;"
							 "SB4-Status=0,SB4-Rtd=%.1f,SB4-Flag=N;SB5-Status=0,SB5-Rtd=%.1f,SB5-Flag=N;"
							 "SB6-Status=0,SB6-Rtd=%.1f,SB6-Flag=N;SB7-Status=0,SB7-Rtd=%.1f,SB7-Flag=N;"
							 "SB8-Status=0,SB8-Rtd=%.1f,SB8-Flag=N&&",\
                g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, time_now.tm_year, time_now.tm_mon + 1, time_now.tm_mday,\
				time_now.tm_hour, time_now.tm_min, time_now.tm_sec, v[0], v[1], v[2], nongdu1, wendu1, shidu1, airpressure1,\
				nongdu2, wendu2, shidu2, airpressure2, v[3], v[4], v[5], v[6], v[7]);
        break;
    case 3:
        sprintf(g_u8_PkgBuf, "ST=%02d;CN=2011;PW=%6d;MN=%s;Flag=0;CP=&&DataTime=%04d%02d%02d%02d%02d%02d;"
							 "SB1-Status=0,SB1-Rtd=%.1f,SB1-Flag=N;SB2-Status=0,SB2-Rtd=%.1f,SB2-Flag=N;"
							 "SB3-Status=0,SB3-Rtd=%.1f,SB3-Flag=N;"
							 "SB4-Status=0,SB4-Rtd=%.1f,SB4-Conc1=%.1f,SB4-Temp1=%.1f,SB4-RH1=%.1f,SB4-AirP1=%.1f,"
							 "SB4-Conc2=%.1f,SB4-Temp2=%.1f,SB4-RH2=%.1f,SB4-AirP2=%.1f,SB4-Flag=N;"
							 "SB5-Status=0,SB5-Rtd=%.1f,SB5-Flag=N;SB6-Status=0,SB6-Rtd=%.1f,SB6-Flag=N;"
							 "SB7-Status=0,SB7-Rtd=%.1f,SB7-Flag=N;SB8-Status=0,SB8-Rtd=%.1f,SB8-Flag=N&&",\
                g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, time_now.tm_year, time_now.tm_mon + 1, time_now.tm_mday,\
				time_now.tm_hour, time_now.tm_min, time_now.tm_sec, v[0], v[1], v[2], v[3],\
				nongdu1, wendu1, shidu1, airpressure1, nongdu2, wendu2, shidu2, airpressure2, v[4], v[5], v[6], v[7]);
        break;
    case 4:
        sprintf(g_u8_PkgBuf, "ST=%02d;CN=2011;PW=%6d;MN=%s;Flag=0;CP=&&DataTime=%04d%02d%02d%02d%02d%02d;"
							 "SB1-Status=0,SB1-Rtd=%.1f,SB1-Flag=N;SB2-Status=0,SB2-Rtd=%.1f,SB2-Flag=N;"
							 "SB3-Status=0,SB3-Rtd=%.1f,SB3-Flag=N;SB4-Status=0,SB4-Rtd=%.1f,SB4-Flag=N;"
							 "SB5-Status=0,SB5-Rtd=%.1f,SB5-Conc1=%.1f,SB5-Temp1=%.1f,SB5-RH1=%.1f,SB5-AirP1=%.1f,"
							 "SB5-Conc2=%.1f,SB5-Temp2=%.1f,SB5-RH2=%.1f,SB5-AirP2=%.1f,SB5-Flag=N;"
							 "SB6-Status=0,SB6-Rtd=%.1f,SB6-Flag=N;SB7-Status=0,SB7-Rtd=%.1f,SB7-Flag=N;"
							 "SB8-Status=0,SB8-Rtd=%.1f,SB8-Flag=N&&",\
                g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, time_now.tm_year, time_now.tm_mon + 1, time_now.tm_mday,\
				time_now.tm_hour, time_now.tm_min, time_now.tm_sec, v[0], v[1], v[2], v[3], v[4],\
				nongdu1, wendu1, shidu1, airpressure1, nongdu2, wendu2, shidu2, airpressure2, v[5], v[6], v[7]);
        break;
    case 5:
        sprintf(g_u8_PkgBuf, "ST=%02d;CN=2011;PW=%6d;MN=%s;Flag=0;CP=&&DataTime=%04d%02d%02d%02d%02d%02d;"
							 "SB1-Status=0,SB1-Rtd=%.1f,SB1-Flag=N;SB2-Status=0,SB2-Rtd=%.1f,SB2-Flag=N;"
							 "SB3-Status=0,SB3-Rtd=%.1f,SB3-Flag=N;SB4-Status=0,SB4-Rtd=%.1f,SB4-Flag=N;"
							 "SB5-Status=0,SB5-Rtd=%.1f,SB5-Flag=N;"
							 "SB6-Status=0,SB6-Rtd=%.1f,SB6-Conc1=%.1f,SB6-Temp1=%.1f,SB6-RH1=%.1f,SB6-AirP1=%.1f,"
							 "SB6-Conc2=%.1f,SB6-Temp2=%.1f,SB6-RH2=%.1f,SB6-AirP2=%.1f,SB6-Flag=N;"
							 "SB7-Status=0,SB7-Rtd=%.1f,SB7-Flag=N;SB8-Status=0,SB8-Rtd=%.1f,SB8-Flag=N&&",\
               g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, time_now.tm_year, time_now.tm_mon + 1, time_now.tm_mday,\
			   time_now.tm_hour, time_now.tm_min, time_now.tm_sec, v[0], v[1], v[2], v[3], v[4], v[5],\
				nongdu1, wendu1, shidu1, airpressure1, nongdu2, wendu2, shidu2, airpressure2, v[6], v[7]);
        break;
    case 6:
        sprintf(g_u8_PkgBuf, "ST=%02d;CN=2011;PW=%6d;MN=%s;Flag=0;CP=&&DataTime=%04d%02d%02d%02d%02d%02d;"
							 "SB1-Status=0,SB1-Rtd=%.1f,SB1-Flag=N;SB2-Status=0,SB2-Rtd=%.1f,SB2-Flag=N;"
							 "SB3-Status=0,SB3-Rtd=%.1f,SB3-Flag=N;SB4-Status=0,SB4-Rtd=%.1f,SB4-Flag=N;"
							 "SB5-Status=0,SB5-Rtd=%.1f,SB5-Flag=N;SB6-Status=0,SB6-Rtd=%.1f,SB6-Flag=N;"
							 "SB7-Status=0,SB7-Rtd=%.1f,SB7-Conc1=%.1f,SB7-Temp1=%.1f,SB7-RH1=%.1f,SB7-AirP1=%.1f,"
							 "SB7-Conc2=%.1f,SB7-Temp2=%.1f,SB7-RH2=%.1f,SB7-AirP2=%.1f,SB7-Flag=N;"
							 "SB8-Status=0,SB8-Rtd=%.1f,SB8-Flag=N&&",\
                g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, time_now.tm_year, time_now.tm_mon + 1, time_now.tm_mday,\
				time_now.tm_hour, time_now.tm_min, time_now.tm_sec, v[0], v[1], v[2], v[3], v[4], v[5], v[6],\
				nongdu1, wendu1, shidu1, airpressure1, nongdu2, wendu2, shidu2, airpressure2, v[7]);
        break;
    case 7:
        sprintf(g_u8_PkgBuf, "ST=%02d;CN=2011;PW=%6d;MN=%s;Flag=0;CP=&&DataTime=%04d%02d%02d%02d%02d%02d;"
							 "SB1-Status=0,SB1-Rtd=%.1f,SB1-Flag=N;SB2-Status=0,SB2-Rtd=%.1f,SB2-Flag=N;"
							 "SB3-Status=0,SB3-Rtd=%.1f,SB3-Flag=N;SB4-Status=0,SB4-Rtd=%.1f,SB4-Flag=N;"
							 "SB5-Status=0,SB5-Rtd=%.1f,SB5-Flag=N;SB6-Status=0,SB6-Rtd=%.1f,SB6-Flag=N;"
							 "SB7-Status=0,SB7-Rtd=%.1f,SB7-Flag=N;"
							 "SB8-Status=0,SB8-Rtd=%.1f,SB8-Conc1=%.1f,SB8-Temp1=%.1f,SB8-RH1=%.1f,SB8-AirP1=%.1f,"
							 "SB8-Conc2=%.1f,SB8-Temp2=%.1f,SB8-RH2=%.1f,SB8-AirP2=%.1f,SB8-Flag=N&&",\
               g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, time_now.tm_year, time_now.tm_mon + 1, time_now.tm_mday,\
			   time_now.tm_hour, time_now.tm_min, time_now.tm_sec, v[0], v[1], v[2], v[3], v[4], v[5], v[6], v[7],\
				nongdu1, wendu1, shidu1, airpressure1, nongdu2, wendu2, shidu2, airpressure2);
        break;
    default:
        break;
    }

//  sprintf(g_u8_PkgBuf, "ST=%02d;CN=2011;PW=%6d;MN=%s;Flag=0;CP=&&DataTime=%04d%02d%02d%02d%02d%02d;SB1-Status=0,SB1-Rtd=%.1f,SB1-Conc1=%.1f,SB1-Temp1=%.1f,SB1-RH1=%.1f,SB1-AirP1=%.1f,SB1-Conc2=%.1f,SB1-Temp2=%.1f,SB1-RH2=%.1f,SB1-AirP2=%.1f,SB1-Flag=N;SB2-Status=0,SB2-Rtd=%.1f,SB2-Flag=N;SB3-Status=0,SB3-Rtd=%.1f,SB3-Flag=N;SB4-Status=0,SB4-Rtd=%.1f,SB4-Flag=N;SB5-Status=0,SB5-Rtd=%.1f,SB5-Flag=N;SB6-Status=0,SB6-Rtd=%.1f,SB6-Flag=N;SB7-Status=0,SB7-Rtd=%.1f,SB7-Flag=N;SB8-Status=0,SB8-Rtd=%.1f,SB8-Flag=N&&",\
//              g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, time_now.tm_year, time_now.tm_mon + 1, time_now.tm_mday, time_now.tm_hour, time_now.tm_min, time_now.tm_sec, v[0], nongdu1, wendu1, shidu1, airpressure1, nongdu2, wendu2, shidu2, airpressure2, v[1], v[2], v[3], v[4], v[5], v[6], v[7]);

    k = strlen(g_u8_PkgBuf); //���ݶγ���
    j = cal_crc(g_u8_PkgBuf, strlen(g_u8_PkgBuf));
    sprintf(str_tmp, "%04X", j); //CRC
//  sprintf(Uart1_TxBuf.buf, "##%04d%s%s\r\n", k, g_u8_PkgBuf, str_tmp); //������ݰ�

#ifdef DEBUG
    sprintf(str_tmp, "send to server%d :\n", conn_id + 1);
    USARTx_SendString(USART_DEBUG, str_tmp);
    USARTx_SendString(USART_DEBUG, Uart1_TxBuf.buf);
#endif

//  res = GU900_TCPSendString(conn_id, Uart1_TxBuf.buf, 100);
#ifdef DEBUG
    if (res == 0)
    {
        sprintf(str_tmp, "connect:%d send ok\n", conn_id);
        USARTx_SendString(USART_DEBUG, str_tmp);
    }
    else
    {
        sprintf(str_tmp, "connect:%d send fail\n", conn_id);
        USARTx_SendString(USART_DEBUG, str_tmp);
    }
#endif
}

/******************************************************************************
  Function:     QnReturn
  Description:  212Э��Ӧ����
  Input:        uint8_t conn_id ����ID��
                char * PtQn ָ��QN�ŵ��ַ���
  Return:       none
  Others:       none
******************************************************************************/
void QnReturn(uint8_t conn_id, char *PtQn)
{
    uint16_t j, k;
    char str_tmp[5];

    sprintf(g_u8_PkgBuf, "ST=%02d;CN=9011;PW=%6d;MN=%s;Flag=0;CP=&&QN=%s;QnRtn=1&&", g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, PtQn);
    k = strlen(g_u8_PkgBuf); //���ݶγ���
    j = cal_crc(g_u8_PkgBuf, strlen(g_u8_PkgBuf));
    sprintf(str_tmp, "%04X", j); //CRC
    sprintf(Uart1_TxBuf.buf, "##%04d%s%s\r\n", k, g_u8_PkgBuf, str_tmp); //������ݰ�
#ifdef DEBUG
    USARTx_SendString(USART_DEBUG, Uart1_TxBuf.buf);
#endif
    GU900_TCPSendString(conn_id, Uart1_TxBuf.buf, 100);
}

/******************************************************************************
  Function:     ExeReturn
  Description:  212Э��Ӧ����
  Input:        uint8_t conn_id ����ID��
                char * PtQn ָ��QN�ŵ��ַ���
                uint8_t ExeRtn 1:ִ�гɹ�;0:ִ��ʧ��;100:û������
  Return:       none
  Others:       none
******************************************************************************/
void ExeReturn(uint8_t conn_id, char *PtQn, uint8_t ExeRtn)
{
    uint16_t j, k;
    char str_tmp[5];

    sprintf(g_u8_PkgBuf, "ST=%02d;CN=9012;PW=%6d;MN=%s;Flag=0;CP=&&QN=%s;ExeRtn=%d&&",
			g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, PtQn, ExeRtn);
    k = strlen(g_u8_PkgBuf); //���ݶγ���
    j = cal_crc(g_u8_PkgBuf, strlen(g_u8_PkgBuf));
    sprintf(str_tmp, "%04X", j); //CRC
    sprintf(Uart1_TxBuf.buf, "##%04d%s%s\r\n", k, g_u8_PkgBuf, str_tmp); //������ݰ�
#ifdef DEBUG
    USARTx_SendString(USART_DEBUG, Uart1_TxBuf.buf);
#endif
    GU900_TCPSendString(conn_id, Uart1_TxBuf.buf, 100);
}

/*************************************************
  Function:     HJ212_CheckData
  Description:  ����HJ212Э��������,�������н���
                ע�����ݶ��ް�ͷ��β�������ַ������ݳ��ȶα���Ϊ0~9���ַ�
                   ��ͷ��β�����������ݰ����ȱ�������
  Input:        char dat        Ҫ������������
                uint16_t len    ���ݳ���
  Output:       none
  Return:       0: �����ɹ�,���ݷ���HJ212Э��
                1: ���ݳ��Ȳ�����
                2: ���ݳ���
                3: crc����
                4: ��ͷ��β����
  Others:       
*************************************************/
uint8_t HJ212_CheckData(char dat[], uint16_t len)
{
    uint8_t i;
    uint16_t num, crc;
    char tmp[5];
    char *p;

    //��ǰ�����Ҫ�ȼ��һ�����ݰ��Ƿ�С����С����
    if (len < 12)
    {
#ifdef DEBUG
        USARTx_SendString(USART_DEBUG, "HJ212_CheckData: len error.\n");
#endif
        return 1; //���ݶγ��Ȳ��Ϸ�
    }
    //���հ�ͷ
    if (dat[0] != '#' && dat[1] != '#') //��ͷ
    {
#ifdef DEBUG
        USARTx_SendString(USART_DEBUG, "HJ212_CheckData: pack head error.\n");
#endif
        return 4; //��ͷ����
    }

    for (i = 0,p = &dat[2]; i < 4; i++) //�������ݰ��ܳ���
    {
        if (*p >= '0' && *p <= '9') //�Ϸ��ַ�
        {
            tmp[i] = *p++;
        }
        else
        {
#ifdef DEBUG
            USARTx_SendString(USART_DEBUG, "HJ212_CheckData: char error1.\n");
#endif
            return 2; //���ݳ���
        }
    }
    tmp[4] = 0;
    num = atoi(tmp);
    if (num >= 1024)
    {
#ifdef DEBUG
        USARTx_SendString(USART_DEBUG, "HJ212_CheckData: length over.\n");
#endif
        return 1; //���ݶγ��Ȳ��Ϸ�
    }

    if (num + 12 != len)
    {
#ifdef DEBUG
        USARTx_SendString(USART_DEBUG, "HJ212_CheckData: length error.\n");
#endif
        return 1; //���ݶγ��Ȳ��Ϸ�
    }

    //У��CRC
    for (i = 0,p = &dat[num + 6]; i < 4; i++) //��ȡ�����е�crcֵ
    {
        if ((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')) //�Ϸ��ַ�
        {
            tmp[i] = *p++;
        }
        else
        {
#ifdef DEBUG
            USARTx_SendString(USART_DEBUG, "HJ212_CheckData: char error2.\n");
#endif
            return 2; //���ݳ���
        }
    }
    tmp[4] = '\0';
    crc = ConvertNum(tmp, 4); //��ȡ�����е�CRC

    if (cal_crc(&dat[6], num) != crc) //���㱨�ĵ�CRC
    {
#ifdef DEBUG
        USARTx_SendString(USART_DEBUG, "HJ212_CheckData: CRC error.\n");
#endif
        return 3; //CRC error
    }

    //����β
    if (dat[len - 2] != '\r' || dat[len - 1] != '\n') //��ͷ
    {
#ifdef DEBUG
        USARTx_SendString(USART_DEBUG, "HJ212_CheckData: pack tail error.\n");
#endif
        return 4; //��β����
    }
    GPRS_State = GPRS_OK; //��ȷ���յ����ݰ���˵��������ͨ
    return 0;
}

/*************************************************
  Function:     HJ212_DataHandle
  Description:  ��HJ212Э������ݽ��н����ʹ���
  Input:        uint8_t conn_id ����ID��
                char dat[]      ��Ҫ�������������
  Return:       0:����
                1:�������������
                2:MN����
                3:
                4:�޺Ϸ�����CN
                5:��QN��
  Others:       Ŀǰû��У�������ST
*************************************************/
uint8_t HJ212_DataHandle(uint8_t conn_id, char *dat)
{
    uint16_t i, j, k;
    char *p;
    char str_tmp[21];
    char str1[21], str2[200];
    char str_search[21];
    char qn_tmp[18];
    uint16_t cmd;
    uint8_t ExeRtn = 0;
    uint8_t IpInfoChange = 0;
    uint32_t begin_time, end_time;
    uint32_t begin_day;
    uint32_t end_day;
    uint32_t temp;

    //У���豸���MN
    p = strstr(dat, "MN=");
    if (p != NULL)
    {
        p += 3;
        i = 0;
        while (( (*p >= '0' && *p <= '9')||(*p >= 'a' && *p <= 'z')||(*p >= 'A' && *p <= 'Z') ) && i < 15)
        {
            str_tmp[i++] = *p++;
        }
        str_tmp[i] = '\0';
    }
    if (strcmp(str_tmp, g_tParam.DevNO) != 0) //�ж��Ƿ񱾻�MN��.
    {
        /* ����ǲ��ǹ㲥�� */
        while (i>0)
        {
            if(str_tmp[i--] != 0)
            {
#ifdef DEBUG
                USARTx_SendString(USART_DEBUG, "MN error.\n");
#endif
                return 1;//MN���󣬷��ش������
            }
        }

    }

    //====== ������յ�������============================================
    //��ȡ����CN
    p = strstr(dat, "CN=");
    if (p == NULL)
    {
        return 4; //�޺Ϸ�����
    }
    p += 3;
    str_tmp[0] = *p++;
    str_tmp[1] = *p++;
    str_tmp[2] = *p++;
    str_tmp[3] = *p;
    str_tmp[4] = '\0';
    cmd = atoi(str_tmp);

    //��ȡQN
    p = strstr(dat, "QN=");
    if (p == NULL)
    {
        return 5; //��QN
    }
    p += 3;
    i = 0;
    while (*p >= '0' && *p <= '9' && i < 17)
    {
        qn_tmp[i++] = *p++;
    }
    qn_tmp[i] = '\0';

    QnReturn(conn_id, qn_tmp); //��Ӧ����

    //����Ĵ���
    switch (cmd)
    {
    case 1007: //�����ֳ���
        ExeReturn(conn_id, qn_tmp, 1);  //����ִ�н��
//      Delay_N10ms(300);               //�ȴ���Ϣ������

        /* ����MCU */
//      __disable_fault_irq();          //��ֹfault�ж�
//      NVIC_SystemReset();             //����MCU

        break;
    case 1008: //��ȡϵͳ����
        sprintf(g_u8_PkgBuf, "ST=%02d;CN=1008;PW=%6d;MN=%s;Flag=0;CP=&&QN=%s;SaveRtdInterval=%d;"
							 "Srv1-IP=%d.%d.%d.%d;Srv1-Port=%d;Srv1-RtdInterval=%d;"
							 "Srv2-IP=%d.%d.%d.%d;Srv2-Port=%d;Srv2-RtdInterval=%d;"
							 "Srv3-IP=%d.%d.%d.%d;Srv3-Port=%d;Srv3-RtdInterval=%d;"
							 "Srv4-IP=%d.%d.%d.%d;Srv4-Port=%d;Srv4-RtdInterval=%d;"
							 "NTP-Enable=%d;System-Flag=0x%08X;NetSignal=%d;FanChannel=%d;CleanChannel=%d;Version=%d;"
							 "Conc1_k=%.3f;Conc1_b=%.1f;Conc1_Rss=%.3f;Conc2_k=%.3f;Conc2_b=%.1f;Conc2_Rss=%.3f&&",\
                    g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, qn_tmp, g_tParam.u16_SaveRtdInterval,\
                    g_tParam.Srv1.ip[0], g_tParam.Srv1.ip[1], g_tParam.Srv1.ip[2], g_tParam.Srv1.ip[3], g_tParam.Srv1.port, g_tParam.Srv1.rtd_interval,\
                    g_tParam.Srv2.ip[0], g_tParam.Srv2.ip[1], g_tParam.Srv2.ip[2], g_tParam.Srv2.ip[3], g_tParam.Srv2.port, g_tParam.Srv2.rtd_interval,\
                    g_tParam.Srv3.ip[0], g_tParam.Srv3.ip[1], g_tParam.Srv3.ip[2], g_tParam.Srv3.ip[3], g_tParam.Srv3.port, g_tParam.Srv3.rtd_interval,\
                    g_tParam.Srv4.ip[0], g_tParam.Srv4.ip[1], g_tParam.Srv4.ip[2], g_tParam.Srv4.ip[3], g_tParam.Srv4.port, g_tParam.Srv4.rtd_interval,\
                    g_tParam.NTP_En, g_tParam.SystemFlag, NetSignal,g_tParam.FanChannel,g_tParam.CleanChannel,g_tParam.Version,\
                    g_tParam.Conc1_k, g_tParam.Conc1_b, g_tParam.sensor_ri1,  g_tParam.Conc2_k, g_tParam.Conc2_b, g_tParam.sensor_ri2 //���Ũ��k,b,Rss;����Ũ��k,b,Rss
                );
        k = strlen(g_u8_PkgBuf); //���ݶγ���
        j = cal_crc(g_u8_PkgBuf, strlen(g_u8_PkgBuf));
        sprintf(str_tmp, "%04X", j); //CRC
        sprintf(Uart1_TxBuf.buf, "##%04d%s%s\r\n", k, g_u8_PkgBuf, str_tmp); //������ݰ�
#ifdef DEBUG
        USARTx_SendString(USART_DEBUG, Uart1_TxBuf.buf);
#endif
//      GU900_TCPSendString(conn_id, Uart1_TxBuf.buf, 100);
        ExeReturn(conn_id, qn_tmp, 1); //����ִ�н��
        break;
    case 1009: //����ϵͳ����
               //ִ������ϵͳ����
        ExeRtn = 0;
        IpInfoChange = 0;
        p = strstr(dat, "CP=&&"); //ֻȡָ�����CP�ֶ�
        dat = p;

        p = strstr(dat, "MN=");
        if (p != NULL)
        {
            p += 3;
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 15) //MN��Ϊ����С��15������
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            strcpy(g_tParam.DevNO, str_tmp);
        }

        p = strstr(dat, "ST=");
        if (p != NULL)
        {
            p += 3;
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 2)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.u8_ST = atoi(str_tmp);
        }

        p = strstr(dat, "PW=");
        if (p != NULL)
        {
            p += 3;
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 6)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Pwd = atoi(str_tmp);
        }

        //�������ݵ�ʱ�����̶���1���ӣ������޸ġ���Ϊ�鿴��ʷ��¼��Ҫ�鿴�������ݡ�
//      p = strstr(dat,"SaveRtdInterval=");
//      if (p!=NULL)
//      {
//          p += 16;
//          i = 0;
//          while (*p >= '0' && *p <= '9')
//          {
//              str_tmp[i++] = *p++;
//          }
//          str_tmp[i] = '\0';
//          g_tParam.u16_SaveRtdInterval = atoi(str_tmp);
//      }

        // ������1
        p = strstr(dat, "Srv1-IP=");
        if (p != NULL)
        {
            IpInfoChange = 1;  //IP��Ϣ�б䶯
            p += 8;
            for (j = 0; j < 4; j++)
            {
                i = 0;
                while (*p >= '0' && *p <= '9' && i < 3)
                {
                    str_tmp[i++] = *p++;
                }
                str_tmp[i] = '\0';
                g_tParam.Srv1.ip[j] = atoi(str_tmp);
                p++;
            }
        }
        p = strstr(dat, "Srv1-Port=");
        if (p != NULL)
        {
            IpInfoChange = 1;  //IP��Ϣ�б䶯
            p += 10;
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 5)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Srv1.port = atoi(str_tmp);
        }
        p = strstr(dat, "Srv1-RtdInterval=");
        if (p != NULL)
        {
            p += 17;
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 5)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Srv1.rtd_interval = atoi(str_tmp);
        }

        // ������2
        p = strstr(dat, "Srv2-IP=");
        if (p != NULL)
        {
            IpInfoChange = 1;  //IP��Ϣ�б䶯
            p += 8;
            for (j = 0; j < 4; j++)
            {
                i = 0;
                while (*p >= '0' && *p <= '9' && i < 3)
                {
                    str_tmp[i++] = *p++;
                }
                str_tmp[i] = '\0';
                g_tParam.Srv2.ip[j] = atoi(str_tmp);
                p++;
            }
        }
        p = strstr(dat, "Srv2-Port=");
        if (p != NULL)
        {
            IpInfoChange = 1;  //IP��Ϣ�б䶯
            p += 10;
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 5)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Srv2.port = atoi(str_tmp);
        }
        p = strstr(dat, "Srv2-RtdInterval=");
        if (p != NULL)
        {
            p += 17;
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 5)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Srv2.rtd_interval = atoi(str_tmp);
        }

        // ������3
        p = strstr(dat, "Srv3-IP=");
        if (p != NULL)
        {
            IpInfoChange = 1;  //IP��Ϣ�б䶯
            p += 8;
            for (j = 0; j < 4; j++)
            {
                i = 0;
                while (*p >= '0' && *p <= '9' && i < 3)
                {
                    str_tmp[i++] = *p++;
                }
                str_tmp[i] = '\0';
                g_tParam.Srv3.ip[j] = atoi(str_tmp);
                p++;
            }
        }
        p = strstr(dat, "Srv3-Port=");
        if (p != NULL)
        {
            IpInfoChange = 1;  //IP��Ϣ�б䶯
            p += 10;
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 5)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Srv3.port = atoi(str_tmp);
        }
        p = strstr(dat, "Srv3-RtdInterval=");
        if (p != NULL)
        {
            p += 17;
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 5)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Srv3.rtd_interval = atoi(str_tmp);
        }

        // ������4
        p = strstr(dat, "Srv4-IP=");
        if (p != NULL)
        {
            IpInfoChange = 1;  //IP��Ϣ�б䶯
            p += 8;
            for (j = 0; j < 4; j++)
            {
                i = 0;
                while (*p >= '0' && *p <= '9' && i < 3)
                {
                    str_tmp[i++] = *p++;
                }
                str_tmp[i] = '\0';
                g_tParam.Srv4.ip[j] = atoi(str_tmp);
                p++;
            }
        }
        p = strstr(dat, "Srv4-Port=");
        if (p != NULL)
        {
            IpInfoChange = 1;  //IP��Ϣ�б䶯
            p += 10;
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 5)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Srv4.port = atoi(str_tmp);
        }
        p = strstr(dat, "Srv4-RtdInterval=");
        if (p != NULL)
        {
            p += 17;
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 5)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Srv4.rtd_interval = atoi(str_tmp);
        }

        p = strstr(dat, "NTP-Enable=");
        if (p != NULL)
        {
            p += 11;
            i = 0;
            while (*p >= '0' && *p <= '9' && i < 1)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.NTP_En = atoi(str_tmp);
        }

        p = strstr(dat, "System-Flag=");
        if (p != NULL)
        {
            p += 12;
            i = 0;
            while (((*p >= '0' && *p <= '9') || (*p >= 'a' && *p <= 'f') || (*p >= 'A' && *p <= 'F')) && i < 8)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.SystemFlag = ConvertNum(str_tmp, i);
        }

        /*----------------------------- Ũ�ȷ��� -------------------------------------------*/
        p = strstr(dat, "Conc1_k=");//���Ũ��ϵ��kֵ(����)
        if (p != NULL)
        {
            p += 8;
            i = 0;
            while (( (*p >= '0' && *p <= '9') || (*p == '.') || (*p == '+') || (*p == '-') ) && i < 10)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Conc1_k = atof(str_tmp);
        }

        p = strstr(dat, "Conc1_b=");//���Ũ��bֵ(���궨ֵ)
        if (p != NULL)
        {
            p += 8;
            i = 0;
            while (( (*p >= '0' && *p <= '9') || (*p == '.') || (*p == '+') || (*p == '-') ) && i < 10)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Conc1_b = atof(str_tmp);
        }

        p = strstr(dat, "Conc1_Rss=");//���Ũ�����궨ֵ
        if (p != NULL)
        {
            p += 10;
            i = 0;
            while (( (*p >= '0' && *p <= '9') || (*p == '.') || (*p == '+') || (*p == '-') ) && i < 10)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.sensor_ri1 = atof(str_tmp);
        }

        p = strstr(dat, "Conc2_k=");//����Ũ��ϵ��kֵ(����)
        if (p != NULL)
        {
            p += 8;
            i = 0;
            while (( (*p >= '0' && *p <= '9') || (*p == '.') || (*p == '+') || (*p == '-') ) && i < 10)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Conc2_k = atof(str_tmp);
        }

        p = strstr(dat, "Conc2_b=");//����Ũ��bֵ(���궨ֵ)
        if (p != NULL)
        {
            p += 8;
            i = 0;
            while (( (*p >= '0' && *p <= '9') || (*p == '.') || (*p == '+') || (*p == '-') ) && i < 10)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.Conc2_b = atof(str_tmp);
        }

        p = strstr(dat, "Conc2_Rss=");//����Ũ�����궨ֵ
        if (p != NULL)
        {
            p += 10;
            i = 0;
            while (( (*p >= '0' && *p <= '9') || (*p == '.') || (*p == '+') || (*p == '-') ) && i < 10)
            {
                str_tmp[i++] = *p++;
            }
            str_tmp[i] = '\0';
            g_tParam.sensor_ri2 = atof(str_tmp);
        }

        if (SaveParam() == 0)
        {
            ExeRtn = 1;
        }

        ExeReturn(conn_id, qn_tmp, ExeRtn); //����ִ�н��
        if (IpInfoChange)
        {
            Delay_N10ms(300); //�ȴ���Ϣ������
                              //���³�ʼ��GPRS
            GPRS_Init();
        }
        break;
    case 1011: //��ȡ�ֳ���ʱ��
               //�ϴ��ֳ���ʱ��
        time_now = Time_GetCalendarTime();  //��ȡ��ǰʱ�����������
        sprintf(g_u8_PkgBuf, "ST=%02d;CN=1011;PW=%6d;MN=%s;Flag=0;CP=&&QN=%s;SystemTime=%04d%02d%02d%02d%02d%02d&&",\
                    g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, qn_tmp, time_now.tm_year, time_now.tm_mon + 1, time_now.tm_mday, time_now.tm_hour, time_now.tm_min, time_now.tm_sec);
        k = strlen(g_u8_PkgBuf); //���ݶγ���
        j = cal_crc(g_u8_PkgBuf, strlen(g_u8_PkgBuf));
        sprintf(str_tmp, "%04X", j); //CRC
        sprintf(Uart1_TxBuf.buf, "##%04d%s%s\r\n", k, g_u8_PkgBuf, str_tmp); //������ݰ�
#ifdef DEBUG
        USARTx_SendString(USART_DEBUG, Uart1_TxBuf.buf);
#endif
        GU900_TCPSendString(conn_id, Uart1_TxBuf.buf, 100);
        ExeReturn(conn_id, qn_tmp, 1); //����ִ�н��
        break;
    case 1012:
        //ִ������ʱ��
        ExeRtn = 0;
        p = strstr(dat, "SystemTime=");
        if (p != NULL)
        {
            ExeRtn = 1;
            p += 11;
            str_tmp[0] = *p++;
            str_tmp[1] = *p++;
            str_tmp[2] = *p++;
            str_tmp[3] = *p++;
            str_tmp[4] = '\0';
            j = atoi(str_tmp);
            time_now.tm_year = j;
            str_tmp[0] = *p++;
            str_tmp[1] = *p++;
            str_tmp[2] = '\0';
            j = atoi(str_tmp);
            time_now.tm_mon = --j;
            str_tmp[0] = *p++;
            str_tmp[1] = *p++;
            str_tmp[2] = '\0';
            j = atoi(str_tmp);
            time_now.tm_mday = j;
            str_tmp[0] = *p++;
            str_tmp[1] = *p++;
            str_tmp[2] = '\0';
            j = atoi(str_tmp);
            time_now.tm_hour = j;
            str_tmp[0] = *p++;
            str_tmp[1] = *p++;
            str_tmp[2] = '\0';
            j = atoi(str_tmp);
            time_now.tm_min = j;
            str_tmp[0] = *p++;
            str_tmp[1] = *p++;
            str_tmp[2] = '\0';
            j = atoi(str_tmp);
            time_now.tm_sec = j;

            RTC_Config();
            Time_SetCalendarTime(time_now);
        }
        ExeReturn(conn_id, qn_tmp, ExeRtn); //����ִ�н��
        break;
    case 1101: //���Ũ�ȱ궨
        g_tParam.sensor_ri1 = sensor_r1;
        if (SaveParam() == 0)
        {
            ExeRtn = 1;
        }
        ExeReturn(conn_id, qn_tmp, ExeRtn); //����ִ�н��
        break;
    case 1102: //����Ũ�ȱ궨
        g_tParam.sensor_ri2 = sensor_r2;
        if (SaveParam() == 0)
        {
            ExeRtn = 1;
        }
        ExeReturn(conn_id, qn_tmp, ExeRtn); //����ִ�н��
        break;
    case 3015: //��ȡ�����״̬
               //�ϴ�����˿�״̬
        sprintf(g_u8_PkgBuf, "ST=%02d;CN=3015;PW=%6d;MN=%s;Flag=0;CP=&&QN=%s;InputData=%02x&&",\
                    g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, qn_tmp, (uint8_t)ReadInputState());
        k = strlen(g_u8_PkgBuf); //���ݶγ���
        j = cal_crc(g_u8_PkgBuf, strlen(g_u8_PkgBuf));
        sprintf(str_tmp, "%04X", j); //CRC
        sprintf(Uart1_TxBuf.buf, "##%04d%s%s\r\n", k, g_u8_PkgBuf, str_tmp); //������ݰ�
#ifdef DEBUG
        USARTx_SendString(USART_DEBUG, Uart1_TxBuf.buf);
#endif
        GU900_TCPSendString(conn_id, Uart1_TxBuf.buf, 100);
        ExeReturn(conn_id, qn_tmp, 1); //����ִ�н��
        break;

    case 3016: //��ȡ����˿�״̬(D0-D19)
        temp = ((uint16_t)DO2<<16)|DO1;
        sprintf(g_u8_PkgBuf, "ST=%02d;CN=3016;PW=%6d;MN=%s;Flag=0;CP=&&QN=%s;OutputData=%08x&&",\
                    g_tParam.u8_ST, g_tParam.Pwd, g_tParam.DevNO, qn_tmp, temp);
        k = strlen(g_u8_PkgBuf); //���ݶγ���
        j = cal_crc(g_u8_PkgBuf, strlen(g_u8_PkgBuf));
        sprintf(str_tmp, "%04X", j); //CRC
        sprintf(Uart1_TxBuf.buf, "##%04d%s%s\r\n", k, g_u8_PkgBuf, str_tmp); //������ݰ�
#ifdef DEBUG
        USARTx_SendString(USART_DEBUG, Uart1_TxBuf.buf);
#endif
        GU900_TCPSendString(conn_id, Uart1_TxBuf.buf, 100);
        ExeReturn(conn_id, qn_tmp, 1); //����ִ�н��
        break;
    case 3017: //��������˿�״̬
               //ִ���������(����D0~D19)
        ExeRtn = 0;
        p = strstr(dat, "OutputData=");
        if (p != NULL)
        {
            p += 11;
#if LEGACY_OUTPUT
            str_tmp[0] = *p++;
            str_tmp[1] = *p++;
            str_tmp[2] = '\0';
            DO2 = ConvertNum(str_tmp, 2);
            UpdateOutput(); //�������Ϣ�����74HC595
            ExeRtn = 1;
#else
            for (i = 0; i < 8;)
            {
                if ((*p >= '0' && *p <= '9') || (*p >= 'A' && *p <= 'F') || (*p >= 'a' && *p <= 'f'))
                {
                    str_tmp[i++] = *p++;
                }
                else
                {
                    break;
                }
            }
            str_tmp[i] = '\0';
            if (i > 0)
            {
                temp = ConvertNum(str_tmp, i);
                DO1 = temp;
                DO2 = temp >> 16;
                UpdateOutput(); //�������Ϣ�����74HC595
            }
            ExeRtn = 1;
#endif
        }
        ExeReturn(conn_id, qn_tmp, ExeRtn); //����ִ�н��
        break;
    case 3020: //ȡ��ʷ��������
        p = strstr(dat, "BeginTime=");
        if (p != NULL)
        {
            p += 10;
            for (i = 0; i < 8;) //ȡ������
            {
                if (*p >= '0' && *p <= '9') //����ַ��Ƿ�Ϸ�
                {
                    str_tmp[i++] = *p++;
                }
                else //�����ַ��Ƿ�,�˳�
                {
                    break;
                }
            }
            str_tmp[i] = '\0';
            begin_day = atoi(str_tmp); //��ʼ����

            for (i = 0; i < 4;) //ȡʱ��
            {
                if (*p >= '0' && *p <= '9') //����ַ��Ƿ�Ϸ�
                {
                    str_tmp[i++] = *p++;
                }
                else //�����ַ��Ƿ�,�˳�
                {
                    break;
                }
            }
            str_tmp[i] = '\0';
            begin_time = atoi(str_tmp); //��ʼʱ��
        }

        p = strstr(dat, "EndTime=");
        if (p != NULL)
        {
            p += 8;
            for (i = 0; i < 8;) //ȡ������
            {
                if (*p >= '0' && *p <= '9') //����ַ��Ƿ�Ϸ�
                {
                    str_tmp[i++] = *p++;
                }
                else //�����ַ��Ƿ�,�˳�
                {
                    break;
                }
            }
            str_tmp[i] = '\0';
            end_day = atoi(str_tmp); //��������

            for (i = 0; i < 4;) //ȡʱ��
            {
                if (*p >= '0' && *p <= '9') //����ַ��Ƿ�Ϸ�
                {
                    str_tmp[i++] = *p++;
                }
                else //�����ַ��Ƿ�,�˳�
                {
                    break;
                }
            }
            str_tmp[i] = '\0';
            end_time = atoi(str_tmp); //����ʱ��
        }
        //��ȡ����
        do
        {
            sprintf(str_search, "%08d,%04d", begin_day, begin_time); //������ʱ��ת��Ϊ�ַ���

            sprintf(str1, "%d.CSV", begin_day);
            if (f_open(&g_fs, str1, FA_READ) == FR_OK) //������������Ϣ����Ӧ���ļ�
            {
                do
                {
                    p = f_gets(str2, 200, &g_fs); //��ȡһ������
                    if (strstr(str2, str_search) != NULL)
                    {
#ifdef DEBUG
                        USARTx_SendString(USART_DEBUG, str2);
#endif
                        //��������,�ϴ�����
                        DateAnalyzer(conn_id, str2);
                    }
                } while (p != NULL);
            }
            begin_time++;
        }while (begin_time <= end_time);
        f_close(&g_fs);
        break;
    default:
        break;
    }
    return 0;
}

/************** (C) COPYRIGHT ��������Ƽ���չ���޹�˾ ********END OF FILE****/

