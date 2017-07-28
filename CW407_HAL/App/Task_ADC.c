/****************** Copyright(c) ��������Ƽ���չ���޹�˾ ***********************
* 
*------File Info--------------------------------------------------------------
* File Name: Task_ADC
* Latest modified Date: �����޸����ڣ�YYYY-MM-DD��
* Latest Version:       �����޶��汾��
* Description:          �ļ��ļ�Ҫ������Ϣ
* compiler:             MDK v4.74
* MCU:                  STM32F103VE
* Oscillator Crystal Frequency:    8MHz
*-----------------------------------------------------------------------------
* Created By:    �ļ��Ĵ���������
* Created date:  �ļ��Ĵ������ڣ�YYYY-MM-DD��
* Version:       �ļ�����ʱ�İ汾��
* Descriptions:  �ļ�����ʱ�ļ�Ҫ����
*
*-----------------------------------------------------------------------------
* Modified by:   �޸��˵�����
* Modified date: �ļ����޸����ڣ�YYYY-MM-DD��
* Version:       �ļ��޶��İ汾��
* Description:   �ļ��޶��ļ�Ҫ����
*
******************************************************************************/


/* Includes -----------------------------------------------------------------*/
#include "stdint.h"                    /* data type definitions and macro*/
#include "stm32f1xx_hal.h"
#include "string.h"
#include "bsp.h"
#include "cmsis_os.h"
#include "config.h"
#include "lcd.h"
#include "Task_GPRS.h"

#if ADC_EN

/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
#define AIN_CHANNEL_NUM				8	/* ģ��ͨ������ */
/* Private macro ------------------------------------------------------------*/
/* Private constants ------------------------------------------------------- */
/* Private variables --------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/
static void Timer4Config(uint16_t _period);
/* Forward Declaration of local functions -----------------------------------*/
/* Declaration of extern functions ------------------------------------------*/
/* Global variables ---------------------------------------------------------*/
/* ADC���� */
__IO uint16_t AdcValue[AIN_CHANNEL_NUM];                             /* ADC����,DMA�Զ�����ñ��� */
__IO uint16_t AdcValueFifo[AIN_CHANNEL_NUM][ADC_FIFO_SIZE];          /* ADCֵFIFO������ */
__IO uint16_t AdcValueFifoCnt = 0;                                   /* ָ��ADCֵFIFO������λ�� */
__IO int32_t AinValue[8];                                            /* ����ADCͨ���������˲���ת�����ֵ,Ϊʵ��ֵ�Ŵ�1000000��,��λuV��uA*/

__IO uint8_t ChannelState[8];                                        /* ��ͨ��������״̬(ͨ����ֵg_tParam.threshold�ж�) */

/* ģ��ͨ��������� */
const uint8_t AIN_Range[AIN_CHANNEL_NUM] =
{
    AIN1_RANGE, AIN2_RANGE, AIN3_RANGE, AIN4_RANGE, AIN5_RANGE, AIN6_RANGE, AIN7_RANGE, AIN8_RANGE
};


/** @defgroup Application_Functions
  * @{
  */
/**
  * @brief  Timer4Config function 
  *         ��ʹ��CubeMX���ɵĶ�ʱ����ʼ�����������ô˺������г�ʼ��
  * @param  _period ��ʱ����/ms(1-6000ms)
  * @retval None
  */
static void Timer4Config(uint16_t _period)
{
    htim4.Instance = TIM4;
    htim4.Init.Prescaler = HAL_RCC_GetHCLKFreq() / 10000 - 1;         /* 100us*/
    htim4.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim4.Init.Period = _period*10 - 1;
    htim4.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&htim4) != HAL_OK)
    {
        Error_Handler();
    }

    /* ��Ӳ��� */
    //HAL_TIM_GenerateEvent(&htim5,TIM_EVENTSOURCE_UPDATE);
    __HAL_TIM_CLEAR_IT(&htim4, TIM_IT_UPDATE);                       /* �ؼ����� Ҫ�����ж��йرն�ʱ��,������������ʱ��ǰҪ�������־λ,��ʱ��֪��ԭ�� */
    HAL_TIM_Base_Start_IT(&htim4);
}

/**
  * @brief This function is called to increment  a global variable "uwTick"
  *        used as application time base.
  * @note In the default implementation, this variable is incremented each 1ms
  *       in Systick ISR.
  * @note ���ϵͳ�ĺ���,�����Լ�������
  * @retval None
  */
void Task_AdcInit(void)
{
    /* �����ڶ�ʱ��4����ADCֵ */
    Timer4Config(ADC_SAMPLE_PERIOD_MS);
}

/**
  * @brief This function is called to sample adc vaule.
  * @note �ڶ�ʱ��4�ж��е��ô˺���. 
  * @retval None
  */
void AdcSample(void)
{
    uint8_t i;
    static uint8_t cnt = 0;
    static uint16_t tmp[8] = { 0, 0, 0, 0, 0, 0, 0, 0 };


    for (i = 4; i < 8; i++)
    {
        //ȡ20ms���������ֵ
        if (AdcValue[i] > tmp[i])
        {
            tmp[i] = AdcValue[i];
        }
    }

    if (++cnt >= 20)
    {
        cnt = 0;

        /* ��ADCֵ����FIFO�� */
        for (i = 4; i < AIN_CHANNEL_NUM; i++)
        {
//          AdcValueFifo[i][AdcValueFifoCnt] = AdcValue[i];
            AdcValueFifo[i][AdcValueFifoCnt] = tmp[i];               //��20ms���������ֵ����
            tmp[i] = 0;
        }

        if (++AdcValueFifoCnt >= ADC_FIFO_SIZE)
        {
            AdcValueFifoCnt = 0;
        }
    }
}

/**
  * @brief  ADCֵת��Ϊ����ֵ���ܺ���
  *         ת�����ֵΪʵ��ֵ�Ŵ�1000000��,��λuV��uA
  * @param  None
  * @retval None
  */
int32_t AdcConvert(uint8_t cmd, uint16_t val)
{
    int32_t rst;

    switch (cmd)
    {
    case 1:                                                          // 0~+10V ��ʽ:Value/4095*2.5(��׼��ѹ)/�˷ŷŴ�ϵ��(1/4)*1000000(�Ŵ�ΪuV)
        rst = val * 2442;                                            //uV
        break;
    case 2:                                                          // -10V~+10V
        rst = 4882 * ((int16_t)val - 2048);
        break;
    case 3:                                                          // 0~5V
        rst = val * 1221;                                            //uV
        break;
    case 4:                                                          // -5V~+5V
        rst = 2441 * ((int16_t)val - 2048);
        break;
    case 5:                                                          // 0~+20mA
//      rst = 20000 * (uint32_t)val / 4095;
        rst = 14140 * (uint32_t)val / 4095;/* ������Чֵ */
        break;
    case 6:                                                          // -20mA~+20mA
        rst = 9765 * ((int16_t)val - 2048);
        break;
    default:
        rst = 0;
        break;
    }

    return rst;
}

/**
  * @brief  ADC sample task function 
  *         ������ÿ��ִ��һ�� 
  * @param  None
  * @retval None
  */
void Task_ADC(void const *argument)
{
    static uint8_t i, j;
    static uint32_t xLastWakeTime;
    static uint32_t sum;
    static uint16_t array_temp[ADC_FIFO_SIZE];                       /* ������ʱ�洢�� */

#if ADC_DEBUG_EN
    static char str_tmp[200];
#endif

    /* Run the ADC calibration */
    if (HAL_ADCEx_Calibration_Start(&hadc1) != HAL_OK)
    {
        /* Calibration Error */
        Error_Handler();
    }

    HAL_ADC_Start_DMA(&hadc1, (uint32_t *)AdcValue, 8);

    Task_AdcInit();

    /* ����xLastWakeTime��Ҫ����ʼ��Ϊ��ǰ��������ֵ��˵��һ�£����Ǹñ���Ψһһ�α���ʽ��ֵ��֮��
        xLastWakeTime���ں���vTaskDelayUntil()���Զ����¡� */
    xLastWakeTime = xTaskGetTickCount();

#if LCD_EN
    /* LCD��ʾ��ʼ�� */
    LCD_SetScreen(0);                                                //�л�������0
    LCD_Gif(0, 5, 0);                                                //ֹͣ������ж���
    LCD_Gif(0, 4, 0);                                                //ֹͣ���������ж���
#endif

    for (;;)
    {
        HAL_RTC_GetTime(&hrtc, &TimeCurrent, RTC_FORMAT_BIN);

        /************************ ADC���� *************************/
        /** Ŀǰ�������ͨ��4-7,ͨ��ָʾ�ƿ��ƶ�Ӧ���ͨ��4-7 *       */
        for (i = 4; i < 8; i++)
        {
            /* ��������ʱ�洢��,���������� */
            for (j = 0, sum = 0; j < ADC_FIFO_SIZE; j++)
            {
                array_temp[j] = AdcValueFifo[i][j];
            }
            bubble_sort_uint16_t(array_temp, ADC_FIFO_SIZE);

            /* ��ȥ�޼�����,����ƽ��ֵ */
            for (j = ADC_FIFO_TRIM, sum = 0; j < (ADC_FIFO_SIZE - ADC_FIFO_TRIM);)
            {
                sum += array_temp[j++];
            }
            sum /= (ADC_FIFO_SIZE - (ADC_FIFO_TRIM << 1));

            AinValue[i] = AdcConvert(AIN_Range[i], sum);

            /* ��ֹ�ɼ���־λ���Ϊ0,��ɼ�����Ϊ0 */
            if ((g_tParam.SystemFlag & (uint32_t)1) == 0)
            {
                 AinValue[i] = 0;
            }

            if (AinValue[i] > g_tParam.threshold)
            {
                ChannelState[i] = 1;

                /* ͨ��1��ΪGPRS����ָʾ�� */
                if (i != 0)
                {
                    SetOutputBit(i, ON);
                }
            }
            else
            {
                ChannelState[i] = 0;

                /* ͨ��1��ΪGPRS����ָʾ�� */
                if (i != 0)
                {
                    SetOutputBit(i, OFF);
                }
            }
        }
#if ADC_DEBUG_EN
        /* ��ӡADCֵ */
        sprintf(str_tmp, "Adc: %d  %d  %d  %d  %d  %d  %d  %d\r\n",
                AdcValue[0], AdcValue[1], AdcValue[2], AdcValue[3], AdcValue[4], AdcValue[5], AdcValue[6], AdcValue[7]);
        comSendString(COM1, str_tmp);

        sprintf(str_tmp, "AIN Value: %lduV  %lduV  %lduV  %lduV  %lduA  %lduA  %lduA  %lduA\r\n",
                AinValue[0], AinValue[1], AinValue[2], AinValue[3], AinValue[4], AinValue[5], AinValue[6], AinValue[7]);
        comSendString(COM1, str_tmp);
#endif

        /**************************** LCD��ʾ���� ********************************/
#if LCD_EN
        UpdateLcdDisplay();
#endif

        /* ��ʱ1�� */
        osDelayUntil(&xLastWakeTime, 1000);
        Timer_OneSecondCallback();                                   /* ���ʱ��λ���� */
        GprsTimer();                                                 /* GPRS�Ķ�ʱ�� */
        HAL_GPIO_TogglePin(LED1_GPIO, LED1_PIN);


        if (IwdgRefreshEn)
        {
            HAL_IWDG_Refresh(&hiwdg);                                    /* ι�������Ź� */
        }
    }
}
#endif

/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
/************** (C) COPYRIGHT ��������Ƽ���չ���޹�˾ ********END OF FILE****/
