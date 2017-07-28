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
#include "stdint.h"                    /* data type definitions and macro*/
#include "stm32f4xx_hal.h"
#include "main.h"
#include "string.h"
#include "bsp.h"
#include "cmsis_os.h"
#include "ff.h"
#include "diskio.h"
#include "fatfs.h"
//#include "MyNet.h"
//#include "Task_ADC.h"
//#include "Task_GPRS.h"
//#include "spi_sd_card_driver.h"
//#include <time.h>
#include "ff.h"

#if LCD_EN
#include "lcd.h"
#endif

/* Private typedef ----------------------------------------------------------*/
/* Private define -----------------------------------------------------------*/

/* Private macro ------------------------------------------------------------*/
/* Private constants ------------------------------------------------------- */
/* Private variables --------------------------------------------------------*/
/* Private function prototypes ----------------------------------------------*/
#if MODBUS_EN
static void Timer5Config(void);
#endif
/* Private functions --------------------------------------------------------*/

/* Forward Declaration of local functions -----------------------------------*/
/* Declaration of extern functions ------------------------------------------*/
/* Global variables ---------------------------------------------------------*/
#if SD_CARD_EN
FATFS   SdFatfs;                                                     /* ����SD���ļ�ϵͳ�ṹ����Ϣ */
#endif

//TIM_HandleTypeDef htim7;
volatile uint32_t ulHighFrequencyTimerTicks;

uint8_t DataPackage[DATA_PACKAGE_SIZE];                              /* ���ڴ洢һ��HJT212���ݰ�(Ŀǰ��̫����GPRS����,ͬһ����������ѯ����,��˲����ͻ) */

/* CMSIS OS tick */
static __IO uint32_t _uwTick;
/******** �ź��� *******/
osMutexId osMutexEthernet;                                           /* ��̫�������ź���,��ʱ��֧��һ������ */
osMutexId osMutexGprs;                                               /* GPRS�ӿڻ����ź��� */
osMutexId osMutexUart1,osMutexUart2,osMutexUart3,osMutexUart4,osMutexUart5; /* ���ڻ����ź��� */

RTC_TimeTypeDef TimeCurrent;                                         /* ���浱ǰʱ�� ���������и���ʱ�� */
RTC_DateTypeDef DateCurrent;                                         /* ���浱ǰ���� */

/* ʱ����� */
uint16_t TimeSecondCnt = 0;                                          /* ʱ�����,������� */

/* ���Ź������������ر��� */
uint8_t IwdgRefreshEn = 1;                                           /* ʹ�ܶ������Ź�ˢ�¹��� */

/** @defgroup Application_Functions
  * @{
  */

/**
  * @brief  Timer5Config function 
  *         ��ʹ��CubeMX���ɵĶ�ʱ����ʼ�����������ô˺������г�ʼ��
  * @param  None
  * @retval None
  */
#if MODBUS_EN
static void Timer5Config(void)
{
    htim5.Instance = TIM5;
    htim5.Init.Prescaler = HAL_RCC_GetHCLKFreq() / 1000000 - 1;      /* us*/
    htim5.Init.CounterMode = TIM_COUNTERMODE_UP;
    htim5.Init.Period = 5000 - 1;
    htim5.Init.ClockDivision = TIM_CLOCKDIVISION_DIV1;
    if (HAL_TIM_Base_Init(&htim5) != HAL_OK)
    {
        Error_Handler();
    }

    /* ��Ӳ��� */
    //HAL_TIM_GenerateEvent(&htim5,TIM_EVENTSOURCE_UPDATE);
    __HAL_TIM_CLEAR_IT(&htim5, TIM_IT_UPDATE);                       /* �ؼ����� Ҫ�����ж��йرն�ʱ��,������������ʱ��ǰҪ�������־λ,��ʱ��֪��ԭ�� */
    // HAL_TIM_Base_Start_IT(&htim5);
}
#endif

/**
  * @brief This function is called to increment  a global variable "uwTick"
  *        used as application time base.
  * @note In the default implementation, this variable is incremented each 1ms
  *       in Systick ISR.
  * @note ���ϵͳ�ĺ���,�����Լ�������
  * @retval None
  */
void HAL_IncTick(void)
{
    static uint8_t i = 0;
    _uwTick++;

    /* uip 10ms��ʱ�� */
    if (++i >= 10)
    {
        i = 0;
        //      TimerCallback();
    }
}

/**
  * @brief Provides a tick value in millisecond.
  * @note ���ϵͳ�ĺ���,ʹ�ñ�����Ϊ_uwTick
  * @retval tick value
  */
uint32_t HAL_GetTick(void)
{
    return _uwTick;
}

#if SD_CARD_EN
/**
  * @brief  SdCardInit function
  * @param  None
  * @retval 0:����ͨ��; 
  *         1:SD������ʧ��; 
  */
void SdCardInit(void)
{
    FRESULT result;

    //  result = SD_Init();
    //  DEBUG_PrintfString("SD card init result is:%d\r\n", result);

    /* ����SD�� */
    result = f_mount(&SdFatfs, (TCHAR const*)SD_Path, 1);
    if (result == RES_OK)
    {
        DEBUG_PrintfString("SD fatfs mount sucess.\r\n");
        CheckSDCardSpace();
    }
    else
    {
        DEBUG_PrintfString("SD fatfs mount fail.\r\n");
    }
}

/**
  * @brief  TestSdCard function
  * @param  None
  * @retval 0:����ͨ��; 
  *         1:SD������ʧ��; 
  */
uint8_t TestSdCard(void)
{
    char result;
    FIL file;
    FATFS SdFatfs;
    uint32_t temp;
    char str_temp[200];                                              //ע���ջ���

    /* ����SD�� */
    //  result = f_mount(&SdFatfs, (TCHAR const*)USER_Path, 1);
    //  if (result != RES_OK)
    //  {
    //      DEBUG_PrintfString("SD fatfs mount fail.\r\n");
    //      return 1;
    //  }
    //  DEBUG_PrintfString("SD fatfs mount sucess.\r\n");

    /* ���Դ�temp.txt�ļ�,��д��abc,�����ʧ�����½�һ��temp.txt�ļ� */
    result = f_open(&file, "0:/temp.txt", FA_OPEN_EXISTING | FA_WRITE); //�򿪽������ڵ��ļ�

    if (result == RES_OK)
    {
        DEBUG_PrintfString("Open file\"temp.txt\" sucess.\r\n");

        /* �ƶ����ļ���� */
        temp = f_size(&file);
        f_lseek(&file, temp);

        result = f_write(&file, "abc1234567891551\r\n", 18, (UINT *)&temp);
        if (result == RES_OK)
        {
            sprintf(str_temp, "Write %d bytes data to file suess.\r\n", temp);
            f_sync(&file);
        }
        else
        {
            DEBUG_PrintfString("Write data to file fail.\r\n");
        }
        f_close(&file);
    }
    else
    {
        DEBUG_PrintfString("Open file\"temp.txt\" fail.\r\n");
        result = f_open(&file, "0:/temp.txt", FA_CREATE_ALWAYS | FA_WRITE); //�򿪽������ڵ��ļ�

        if (result == RES_OK)
        {
            DEBUG_PrintfString("Create file\"temp.txt\" sucess.\r\n");
            result = f_write(&file, "abc1234567891551\r\n", 18, (UINT *)&temp);
            if (result == RES_OK)
            {
                sprintf(str_temp, "Write %d bytes data to file suess.\r\n",temp);
                f_sync(&file);
            }
            else
            {
                DEBUG_PrintfString("Write data to file fail.\r\n");
            }
            f_close(&file);
        }
        else
        {
            DEBUG_PrintfString("Create file\"temp.txt\" fail.\r\n");
        }
    }

    /* ���Դ�text.txt�ļ�,�����ļ����ݴ�ӡ�������� */
    result = f_open(&file, "temp.txt", FA_OPEN_EXISTING | FA_READ);  //�򿪽������ڵ��ļ�

    if (result == RES_OK)
    {
        DEBUG_PrintfString("Open file\"temp.txt\" sucess.\r\n");

        while (f_gets(str_temp, 200, &file) != NULL)                 //��ȡһ������
        {
            DEBUG_PrintfString(str_temp);
        }

        f_close(&file);
    }
    else
    {
        DEBUG_PrintfString("File\"temp.txt\" not exist.\r\n");
    }

    return 0;
}
#endif

/**
  * @brief  TestSpiFlash function
  * @param  None
  * @retval 0:����ͨ��; 
  *        1:SPI FLASH����ʧ�� ;
  *        2:��ʽ��ʧ��;
*/
#if SPI_FLASH_EN
uint8_t TestSpiFlash(void)
{
    char result;
    FIL file;
    FATFS SfFatfs;
    uint32_t temp;
    char str_temp[200];

    /* ����SD�� */
    result = f_mount(&SfFatfs, (TCHAR const*)USER_Path, 0);
    if (result == FR_NO_FILESYSTEM)
    {
        DEBUG_PrintfString("SPI-FLASH no file system.\r\nStart format.\r\n");
        result = f_mkfs((TCHAR const*)USER_Path, 0, 1);
        if (result != RES_OK)
        {
            DEBUG_PrintfString("SPI-FLASH disk format fail.\r\nReturn.\r\n");
            return 2;
        }
        DEBUG_PrintfString("SPI-FLASH disk format sucess.\r\n");

        return 1;
    }
    DEBUG_PrintfString("SPI-FLASH fatfs mount sucess.\r\n");

    /* ��ʽ������ */


    /* ���Դ�temp.txt�ļ�,��д��abc,�����ʧ�����½�һ��temp.txt�ļ� */
    result = f_open(&file, "1:/temp.txt", FA_OPEN_EXISTING | FA_WRITE); //�򿪽������ڵ��ļ�

    if (result == RES_OK)
    {
        DEBUG_PrintfString("Open file\"temp.txt\" sucess.\r\n");
        f_close(&file);
    }
    else
    {
        DEBUG_PrintfString("Open file\"temp.txt\" fail.\r\n");
        result = f_open(&file, "1:/temp.txt", FA_CREATE_ALWAYS | FA_WRITE); //�򿪽������ڵ��ļ�

        if (result == RES_OK)
        {
            DEBUG_PrintfString("Create file\"temp.txt\" sucess.\r\n");
            f_write(&file, "abc", 3, (UINT *)&temp);
            f_close(&file);
        }
        else
        {
            DEBUG_PrintfString("Create file\"temp.txt\" fail.\r\n");
        }
    }

    /* ���Դ�text.txt�ļ�,�����ļ����ݴ�ӡ�������� */
    result = f_open(&file, "1:/temp.txt", FA_OPEN_EXISTING | FA_READ); //�򿪽������ڵ��ļ�

    if (result == RES_OK)
    {
        DEBUG_PrintfString("Open file\"temp.txt\" sucess.\r\n");

        while (f_gets(str_temp, 200, &file) != NULL)                 //��ȡһ������
        {
            DEBUG_PrintfString(str_temp);
        }

        f_close(&file);
    }
    else
    {
        DEBUG_PrintfString("File\"temp.txt\" not exist.\r\n");
    }

    return 0;
}
#endif //SPI_FLASH_EN

/**
  * @brief  TestEeprom function 
  *         Ŀǰ�������������Ƿ����� 
  * @param  None
  * @retval 0:����ͨ��; 
*/
#if EEPROM_EN
uint8_t TestEeprom(void)
{
    uint8_t i;
    uint8_t data;
    char str_tmp[5];
    char ucString[30] = { 0, 1, 2, 3, 4, 5, 6, 7, 8, 9, 11, 12, 13, 14, 15, 16, 17, 18, 19, 20, 21, 22, 23, 24, 25, 26, 27, 28, 29, 30 };
    HAL_StatusTypeDef result;

    DEBUG_PrintfString("************ EEPROM test **************\r\n");

    /* ���� */
    for (i = 0, data = 0xff; i < 200; i++)
    {
        EEPROM_WriteData(i, &data, 1);
    }

    /* ��ӡEEPROM��������� */
    DEBUG_PrintfString("EEPROM data:\r\n");
    for (i = 0; i < 200; i++)
    {
        EEPROM_ReadData(i, &data, 1);
        sprintf(str_tmp, "%d ", data);
        DEBUG_PrintfString(str_tmp);
    }
    DEBUG_PrintfString("\r\n");

    /* д����� */
    result = EEPROM_WriteData(0, (uint8_t *)ucString, sizeof(ucString));

    sprintf(str_tmp, "result: %d", result);
    DEBUG_PrintfString(str_tmp);

    /* ��ӡEEPROM��������� */
    DEBUG_PrintfString("EEPROM data:\r\n");
    for (i = 0; i < 200; i++)
    {
        EEPROM_ReadData(i, &data, 1);
        sprintf(str_tmp, "%d ", data);
        DEBUG_PrintfString(str_tmp);
    }
    DEBUG_PrintfString("\r\n");

    //  EEPROM_ReadData(0,ucString,sizeof(ucString));
    //  DEBUG_PrintfString("EEPROM data:\r\n");
    //  for (i=0;i<sizeof(ucString);i++)
    //  {
    //  	sprintf(str_tmp,"%d ",ucString[i]);
    //  	DEBUG_PrintfString(str_tmp);
    //  }
    //  DEBUG_PrintfString("\r\n");

    //  for (i=0;i<200;i++)
    //  {
    //      EEPROM_ReadData(i,&data,1);
    //  	if (data != i)
    //  	{
    //          DEBUG_PrintfString("EEPROM error.\r\n");
    //  	}
    //  }

    DEBUG_PrintfString("*********** EEPROM test finish ***********\r\n");

    return 0;
}
#endif //EEPROM_EN

//SDRAM�ڴ����
#if SDRAM_EN
void TestSdram(void)
{
    uint32_t i = 0;
    uint32_t address = 0;
    uint32_t *p;
    uint8_t result = 0;
    uint16_t *pusP = 0;
    uint8_t *pucP = 0;
    uint8_t str_tmp[50];

    DEBUG_PrintfString("Extern memory Test start.\r\n");

    /* ���ֲ��� */
    DEBUG_PrintfString("32-bit accesse test...\r\n");
    DEBUG_PrintfString("Start write test data to sdram.\r\n");
    for (i = 0, p = (uint32_t *)SDRAM_DEVICE_ADDR; i < SDRAM_DEVICE_SIZE / 4; i++)
    {
        *p++ = i;
    }

    DEBUG_PrintfString("Start read test date to sdram.\r\n");

    for (i = 0, p = (uint32_t *)SDRAM_DEVICE_ADDR; i < SDRAM_DEVICE_SIZE / 4; i++)
    {
        if (*p != i)
        {
            sprintf(str_tmp, "Error:add[%d]=%08X;\r\n", i, *p);
            DEBUG_PrintfString(str_tmp);
            result = 1;
        }
        p++;
    }

    //  DEBUG_PrintfString("SDRAM test finish.\r\n");
    if (result == 0)
    {
        DEBUG_PrintfString("SDRAM 32bit test OK.\r\n");
    }
    else
    {
        DEBUG_PrintfString("SDRAM 32bit test fail.\r\n");
    }

    /* �����ַ��ʲ��� */
    //����
    // for (i=0,p=(uint32_t *)SDRAM_DEVICE_ADDR; i<SDRAM_DEVICE_SIZE/4; i++)
    // {
    //    *p++ = 0xFFFFFFFF;
    // }
    DEBUG_PrintfString("16-bit accesse test...\r\n");
    DEBUG_PrintfString("Start write test data to sdram.\r\n");
    for (pusP = (uint16_t *)SDRAM_DEVICE_ADDR, address = SDRAM_DEVICE_ADDR; address < (SDRAM_DEVICE_ADDR + SDRAM_DEVICE_SIZE / 2); address += 0x10000)
    {
        for (i = 0; i <= 0xffff; i++)
        {
            *pusP++ = i;
        }
    }

    DEBUG_PrintfString("Start read test date to sdram.\r\n");

    for (pusP = (uint16_t *)SDRAM_DEVICE_ADDR, address = SDRAM_DEVICE_ADDR; address < (SDRAM_DEVICE_ADDR + SDRAM_DEVICE_SIZE / 2); address += 0x10000)
    {
        for (i = 0; i <= 0xffff; i++)
        {
            if (*pusP != i)
            {
                sprintf(str_tmp, "Error:add[%d]=%d;\r\n", i, *pusP);
                DEBUG_PrintfString(str_tmp);
                result = 1;
            }
            pusP++;
        }
    }

    //  DEBUG_PrintfString("SDRAM test finish.\r\n");
    if (result == 0)
    {
        DEBUG_PrintfString("SDRAM 16bit test OK.\r\n");
    }
    else
    {
        DEBUG_PrintfString("SDRAM 16bit test fail.\r\n");
    }


    /* ���ֽڷ��ʲ��� */
    DEBUG_PrintfString("8-bit accesse test...\r\n");
    DEBUG_PrintfString("Start write test data to sdram.\r\n");
    for (pucP = (uint8_t *)SDRAM_DEVICE_ADDR, address = SDRAM_DEVICE_ADDR; address < (SDRAM_DEVICE_ADDR + SDRAM_DEVICE_SIZE); address += 0x100)
    {
        for (i = 0; i <= 0xff; i++)
        {
            *pucP++ = i;
        }
    }

    DEBUG_PrintfString("Start read test date to sdram.\r\n");

    //  for (result = 0, pucP = (uint8_t *)SDRAM_DEVICE_ADDR, address = SDRAM_DEVICE_ADDR; address < (SDRAM_DEVICE_ADDR + SDRAM_DEVICE_SIZE); address += 0x100)
    for (result = 0, pucP = (uint8_t *)SDRAM_DEVICE_ADDR, address = 0; address < 100; address++)
    {
        for (i = 0; i <= 0xff; i++)
        {
            if (*pucP != i)
            {
                printf("Error:add[%d]=%d;\r\n", address + i, *pucP);
                result = 1;
            }
            pucP++;
        }
    }

    //  DEBUG_PrintfString("SDRAM test finish.\r\n");
    if (result == 0)
    {
        DEBUG_PrintfString("SDRAM bytes access test OK.\r\n");
    }
    else
    {
        DEBUG_PrintfString("SDRAM bytes access test fail.\r\n");
    }

    /* ��⣬��Ҫ���������Ƿ������Լ��������� */
    // //ÿ��16K�ֽ�,д��һ������,�ܹ�д��2048������,�պ���32M�ֽ�
    // for (i = 0; i < 32 * 1024 * 1024; i += 16 * 1024)
    // {
    //    *(volatile uint32_t *)(SDRAM_DEVICE_ADDR + i) = temp;
    //    temp++;
    // }
    // //���ζ���֮ǰд�������,����У��
    // for (i = 0; i < 32 * 1024 * 1024; i += 16 * 1024)
    // {
    //    temp = *(volatile uint32_t *)(SDRAM_DEVICE_ADDR + i);
    //    if (i == 0) sval = temp;
    //    else if (temp <= sval) break; //�������������һ��Ҫ�ȵ�һ�ζ��������ݴ�.
    //    DEBUG_PrintfString("SDRAM Capacity:%dKB\r\n", (uint16_t)(temp - sval + 1) * 16); //��ӡSDRAM����
    // }
}
#endif


/**
  * @brief  default task function
  * @param  None
  * @retval None
  */
void Task_Default(void const *argument)
{
    char dat;
    uint32_t error;

    /* USER CODE BEGIN 5 */
    DEBUG_PrintfString("System startup.\r\n");
    //  HAL_UART_Transmit(&huart1,"System startup.\r\n",17,0xffff);

#if RELEASE_EN
    FLASH_OBProgramInitTypeDef sOBInit;
    HAL_FLASHEx_OBGetConfig(&sOBInit);
    if (sOBInit.RDPLevel == OB_RDP_LEVEL_0)
    {
        DEBUG_PrintfString("Set read protected.\r\n");
        osDelay(100);
        sOBInit.RDPLevel = OB_RDP_LEVEL_1;
        sOBInit.OptionType = OPTIONBYTE_RDP;
        taskENTER_CRITICAL();
        HAL_FLASH_Unlock();
        HAL_FLASH_OB_Unlock();
        HAL_FLASHEx_OBProgram(&sOBInit);
        HAL_FLASH_OB_Lock();
        HAL_FLASH_Lock();
        taskEXIT_CRITICAL();

        DEBUG_PrintfString("Set read protected finish.\r\n");
        osDelay(100);
        __disable_fault_irq();                                       //��ֹfault�ж�
        NVIC_SystemReset();                                          //����MCU
    }
#endif

    /******************* �ź�����ʼ�� ************************/
    //  osMutexEthernet = osMutexCreate(NULL);
    //  osMutexGprs = osMutexCreate(NULL);
    //  osMutexUart1 = osMutexCreate(NULL);
    //  osMutexUart2 = osMutexCreate(NULL);
    //  osMutexUart3 = osMutexCreate(NULL);
    //  osMutexUart4 = osMutexCreate(NULL);
    //  osMutexUart5 = osMutexCreate(NULL);

    //  LoadParam();                                                     /* ����FLASH�еĲ��� */

    ///   static FLASH_EraseInitTypeDef EraseInitStruct;
    //
    //   __set_PRIMASK(1);       /* ���ж� */
    //
    //   /* Unlock the Flash to enable the flash control register access *************/
    //   HAL_FLASH_Unlock();
    //   /* Fill EraseInit structure*/
    //   EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
    //   EraseInitStruct.PageAddress = bsp_GetSector(PARAM_START_ADDR); //FLASH_USER_START_ADDR;
    //   EraseInitStruct.NbPages     = 1;
    //   HAL_FLASHEx_Erase(&EraseInitStruct, &error);
    //   HAL_FLASH_Lock();
    //
    //   __set_PRIMASK(0);       /* ���ж� */
    //   SetDefaultParam();
    //   SaveParam();

#if SD_CARD_EN
    //dat = SD_Init();
    //DEBUG_PrintfString("SD card init result is:%d\r\n", dat);
    SdCardInit();
    TestSdCard();
#endif

#if SPI_FLASH_EN
    TestSpiFlash();
#endif

#if EEPROM_EN
    TestEeprom();
#endif

#if SDRAM_EN
//  TestSdram();
#endif

#if ETHERNET_EN
    UIP_Config();
    osThreadDef(EthernetTask, Task_Ethernet, TASK_EHTERNET_PRIORITY, 0, TASK_ETHERNET_STACK_SIZE);
    osThreadCreate(osThread(EthernetTask), NULL);
#endif

#if LCD_EN
    /* Һ������ʼ��,��ʵֻ��ʼ��Һ�����Ĵ��ڵĻ������,������ADC����ǰ��ʼ��,��Ϊ��������ʹ�õĵ�Һ���� */
    LCD_Init();
#endif

#if ADC_EN
    /* definition and creation of defaultTask */
    osThreadDef(adcTask, Task_ADC, TASK_ADC_PRIORITY, 0, TASK_ADC_STACK_SIZE);
    osThreadCreate(osThread(adcTask), NULL);
    //  xTaskCreate(Task_ADC,"Task adc",256,0,4,NULL);
#endif

    /* ��ʼ��ModBus���� */
#if MODBUS_EN
    MODBUS_InitVar(115200, WKM_MODBUS_DEVICE, 1);
    Timer5Config();                                                  /* ���ö�ʱ��,����Modbus��ʱ3.5T��ʱʱ�� */
#endif



#if GPRS_EN
    /* ����GPRS���� */
    osThreadDef(GprsTask, Task_GPRS, TASK_GPRS_PRIORITY, 0, TASK_GPRS_STACK_SIZE);
    if (osThreadCreate(osThread(GprsTask), NULL) == NULL)
    {
        DEBUG_PrintfString("Create GPRS task fail.\n");
    }
#endif

    /* ��ȡʱ������ */
    HAL_RTC_GetTime(&hrtc, &TimeCurrent, RTC_FORMAT_BIN);
    HAL_RTC_GetDate(&hrtc, &DateCurrent, RTC_FORMAT_BIN);

    /* Infinite loop */
    for (;;)
    {
        osDelay(100);

#if MODBUS_EN
        MODBUS_Poll();
#endif

#if ETHERNET_EN
        /* uip��ѯ������ */
        uip_polling();
#endif

#if LCD_EN
        /* ����Һ�������� */
        LCD_CmdHandle();
#endif

        HAL_GPIO_TogglePin(GPIOB, GPIO_PIN_0);

        /* �����¼����� */
        if (TimeSecondCnt >= 60)
        {
            TimeSecondCnt = 0;

            static uint8_t cnt1 = 0;
            if (++cnt1 >= 1)
            {
                cnt1 = 0;
                /* ��ӡ������Ϣ,������ջ��� */
                //              PrintTaskList();
            }
        }

    }
}

/**
  * @brief  PrintTaskList
  *         ��ӡ��������
  * @param  None
  * @retval None
  */
//void PrintTaskList(void)
//{
//    static char str_temp[500];
//    vTaskList(str_temp);
//    DEBUG_PrintfString("\r\n������\t\t״̬\t���ȼ�\tʣ��ջ\t�������\r\n");
//    DEBUG_PrintfString(str_temp);
//
//    vTaskGetRunTimeStats(str_temp);
//    DEBUG_PrintfString("\r\n������\t\t���м���\tʹ����\r\n");
//    DEBUG_PrintfString(str_temp);
//}

/**
  * @brief  Timer_OneSecondCallback
  *         1�붨ʱ�����õĻص�����,�˺���һ�����ڸ��¸�����ʱ����,Ŀǰ�������������
  * @param  None
  * @retval None
  */
//void Timer_OneSecondCallback(void)
//{
//    TimeSecondCnt++;                                                 /* ���ʱ��λ��1 */
//}

/******************************************************************************
  Function:     CheckSDCardSpace
  Description:  ���SD���ռ䣬��ʣ��ռ䲻��ʱ��ɾ��������ļ�(���ļ���)
  Input:        none
  Return:       none
  Others:       none
******************************************************************************/
void CheckSDCardSpace(void)
{
    FRESULT res;                                                     // FatFs function common result code--FAT
    FATFS *fs;
    DWORD fre_clust, fre_sect;
    DWORD tot_sect;
#if DEBUG_EN
    char str[200];
#endif

    /* Get volume information and free clusters of drive 1 */
    res = f_getfree("0:/", &fre_clust, &fs);
    if (res == FR_OK)
    {
        /* Get total sectors and free sectors */
        tot_sect = (fs->n_fatent - 2) * fs->csize;
        fre_sect = fre_clust * fs->csize;

#if DEBUG_EN
        /* Print the free space (assuming 512 bytes/sector) */
        sprintf(str, "%luKB total drive space.%luKB available\r\n",
                tot_sect / 2, fre_sect / 2);
        //LCD_Txt(0, 6, str);
        DEBUG_PrintfString(str);
#endif

        if (fre_sect < 20480)                                        //ʣ��ռ䲻��20MB
        {
            //          DeleteFirstFile();
        }
    }
}
/*----------------------------------------------------------------------------
 * end of file
 *---------------------------------------------------------------------------*/
/************** (C) COPYRIGHT ��������Ƽ���չ���޹�˾ ********END OF FILE****/
