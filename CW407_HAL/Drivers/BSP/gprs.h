/****************** Copyright(c) ��������Ƽ���չ���޹�˾ ***********************/
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __GPRS_H__
#define __GPRS_H__

/* Includes ----------------------------------------------------------------- */
/* Private macros ----------------------------------------------------------- */
/** @addtogroup GPRS_Private_Macros
  * @{
  */
//GU900S PWRKEY
#define GPRS_PowerKeyEnable()    HAL_GPIO_WritePin(GPRS_PWRON_GPIO,GPRS_PWRON_PIN,GPIO_PIN_SET)
#define GPRS_PowerKeyDisable()   HAL_GPIO_WritePin(GPRS_PWRON_GPIO,GPRS_PWRON_PIN,GPIO_PIN_RESET)

#define IS_ME909_CONNECT_ID(ID)	((ID >= 1) && (ID <= 5))
/* Exported types ----------------------------------------------------------- */
#define GPRS_COM    COM3    /* ����GPRS��Ӧ�Ĵ��� */

typedef enum {
    GPRS_OK = 0,                                                     //GPRS��ʼ���ɹ�
    NO_INIT,                                                         //GPRSδ��ʼ��
    NO_SIM_CARD,                                                     //��SIM��
    INVALID_SIM_CARD,                                                //SIM����Ч
    REGISTER_FAIL,                                                   //ע������ʧ��
    NO_GPRS                                                          //��GPRSҵ��(����ͨѶ��Ƿ��)
}GPRS_StateTypeDef;

/* �������� */
typedef enum {
    NOSERVICE,                                                       //�޷���
    GSM,
    WCDMA,
    TDSCDMA,
    LTE
}NetTypedef;
/* Exported constants ------------------------------------------------------- */
/* Exported macros ---------------------------------------------------------- */
#define GPRS_RX_BUF_TMP_LEN  200    /* ����GPRS��������ʱ��������С */
/* Exported variables ------------------------------------------------------- */
extern NetTypedef NetType;                                           //��������
extern GPRS_StateTypeDef GPRS_State;                                 //gprs״̬
extern uint8_t Isp;                                                  //ISPʶ����(0:�й��ƶ�;1:�й���ͨ)
extern char GPRS_RxBufTmp[];
extern volatile uint16_t GPRS_RxBufTmp_cnt;
extern uint8_t GprsSignalLevel;                                      //���������ź�ǿ��
extern uint8_t g_ucConnectEnable;                                    //8·����ʹ�ܱ�־ 0:�����ӱ���ֹ;1:�����ӱ�ʹ��
extern char IMSI[16];                                                //����15λIMSI��
extern uint8_t GprsInitRequest;                                      //�������³�ʼ����������

/* Exported functions ------------------------------------------------------- */
uint8_t GU900_SendCmd(char *cmd, char *ack, uint16_t wait_time);
uint8_t ME909_Init(void);
void DTU_Init(void);
uint8_t ME_TCPSendString(uint8_t conn_id, char *str, uint16_t wait_time);
void DTU_HandleRecData(void);
uint8_t GPRS_CSQ(void);
void DTU_Startup(void);
void ME_Disconnect(uint8_t _type, uint8_t _connect_id);
void ME_Reconnection(void);
void ME_ConnectSever(uint8_t _ucServerNo);
void ME_HeartbeartTmrCallback(void);
void ME_CheckConnection(void);
uint8_t DTU_HCSQ(void);
uint8_t DTU_GetSimNum(char *s);
void ME_Heartbeart(void);
void DTU_CheckState(void);

#endif /*__GPRS_H__*/

/********* (C) COPYRIGHT 2015-2025 ��������Ƽ���չ���޹�˾ ****END OF FILE****/

