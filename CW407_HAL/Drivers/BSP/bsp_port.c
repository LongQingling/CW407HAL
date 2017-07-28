/****************** Copyright(c) ��������Ƽ���չ���޹�˾ ***********************
* 
*------File Info--------------------------------------------------------------
* File Name: �ļ���
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
#include <stdint.h>                    /* data type definitions and macro*/
#include "bsp.h"
/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/
/* Private macro ------------------------------------------------------------*/
/* Private constants ------------------------------------------------------- */
/* Private variables --------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/


/* Private functions --------------------------------------------------------*/

/* Declaration of extern functions ------------------------------------------*/
/* Global variables ---------------------------------------------------------*/



/** @defgroup bsp_port_functions
  * @{
  */

void LED1_On(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_RESET);
}

void LED1_Off(void)
{
    HAL_GPIO_WritePin(GPIOB, GPIO_PIN_0, GPIO_PIN_SET);
}




/************** (C) COPYRIGHT ��������Ƽ���չ���޹�˾ ********END OF FILE****/
