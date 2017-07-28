/****************** Copyright(c) ��������Ƽ���չ���޹�˾ ***********************/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __TASK_ADC_H__
#define __TASK_ADC_H__
/* Includes ----------------------------------------------------------------- */
#include <stdint.h>
#include "stm32f1xx_hal.h"
/* Private macros ----------------------------------------------------------- */

/* Exported types and constants --------------------------------------------- */
/* Exported types ----------------------------------------------------------- */
/* Exported constants ------------------------------------------------------- */
/* Exported macros ---------------------------------------------------------- */
typedef enum {AIN1,AIN2,AIN3,AIN4,AIN5,AIN6,AIN7,AINT8} AdcChannelTypeDef;




/* Exported variables ------------------------------------------------------- */
extern __IO int32_t  AinValue[];                                      /* ����ADCͨ���������˲���ת�����ֵ,Ϊʵ��ֵ�Ŵ�1000000��,��λuV��uA*/
extern __IO uint8_t ChannelState[8];                                  /* ��ͨ��������״̬(ͨ����ֵg_tParam.threshold�ж�) */

/* Exported functions ------------------------------------------------------- */
void Task_ADC(void const *argument);
void AdcSample(void);

#endif /*__TASK_ADC_H__*/

/************** (C) COPYRIGHT ��������Ƽ���չ���޹�˾ ********END OF FILE****/
