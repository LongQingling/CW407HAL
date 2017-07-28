/****************** Copyright(c) ��������Ƽ���չ���޹�˾ ***********************/

/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __APP_H__
#define __APP_H__
/* Includes ----------------------------------------------------------------- */
#include <stdint.h>
#include "stm32f4xx_hal.h"
/* Private macros ----------------------------------------------------------- */

/* Exported types and constants --------------------------------------------- */
/* Exported types ----------------------------------------------------------- */
/* Exported constants ------------------------------------------------------- */
/* Exported macros ---------------------------------------------------------- */
#define DATA_PACKAGE_SIZE       1050    /* ���ڴ洢һ��HJT212���ݰ�(Ŀǰ��̫����GPRS����,ͬһ����������ѯ����,��˲����ͻ) */
/* Exported variables ------------------------------------------------------- */
/** main.c���� */
extern UART_HandleTypeDef huart1;
extern UART_HandleTypeDef huart3;
extern ETH_HandleTypeDef heth;
//extern TIM_HandleTypeDef htim4;
//extern TIM_HandleTypeDef htim5;
extern RTC_HandleTypeDef hrtc;
//extern IWDG_HandleTypeDef hiwdg;
//extern uint8_t IwdgRefreshEn;                                           /* ʹ�ܶ������Ź�ˢ�¹��� */

//extern void Error_Handler(void);
extern SD_HandleTypeDef hsd;
extern DMA_HandleTypeDef hdma_sdio_rx;
extern DMA_HandleTypeDef hdma_sdio_tx;
extern HAL_SD_CardInfoTypeDef SDCardInfo;

#if SPI_FLASH_EN || SD_CARD_EN
//extern SPI_HandleTypeDef hspi1;
#endif

#if ADC_EN
extern ADC_HandleTypeDef hadc1;
#endif

#if EEPROM_EN
extern I2C_HandleTypeDef hi2c1;
#endif

void MX_SDIO_SD_Init(void);

/* app */
extern RTC_TimeTypeDef TimeCurrent;            /* ���浱ǰʱ�� ���������и���ʱ�� */
extern RTC_DateTypeDef DateCurrent;            /* ���浱ǰ���� */
extern uint8_t DataPackage[];                  /* ���ڴ洢һ��HJT212���ݰ�(Ŀǰ��̫����GPRS����,ͬһ����������ѯ����,��˲����ͻ) */

//extern TIM_HandleTypeDef htim7;
extern volatile uint32_t ulHighFrequencyTimerTicks;
/* Exported functions ------------------------------------------------------- */
void Task_Default(void const *argument);
void TestSdram(void);
//void PrintTaskList(void);
//void Timer_OneSecondCallback(void);
void SdCardInit(void);
HAL_StatusTypeDef RTC_Config(RTC_HandleTypeDef *hrtc);
void CheckSDCardSpace(void);



#endif /*__APP_H__*/

/************** (C) COPYRIGHT ��������Ƽ���չ���޹�˾ ********END OF FILE****/
