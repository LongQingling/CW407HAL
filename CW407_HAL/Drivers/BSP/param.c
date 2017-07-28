/*
*********************************************************************************************************
*
*	ģ������ : Ӧ�ó������ģ��
*	�ļ����� : param.c
*	��    �� : V1.0
*	˵    �� : ��ȡ�ͱ���Ӧ�ó���Ĳ���
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2016-10-01 ljw  ��ʽ����
*
*	Copyright (C), 2016-2026, ��������Ƽ���չ���޹�˾
*
*********************************************************************************************************
*/

#include "bsp.h"
#include "param.h"
#include "bsp_cpu_flash.h"


/* Global variables ---------------------------------------------------------*/
__align(8) PARAM_T g_tParam;


/* ��16KB һ�������Ŀռ�Ԥ��������Ϊ������ For MDK */
//const uint8_t para_flash_area[16*1024] __attribute__((at(ADDR_FLASH_SECTOR_3)));

/* Private function prototypes ----------------------------------------------*/
static uint8_t FlashCopy(uint32_t dest_addr, uint32_t src_addr, uint32_t _ulSize);

/*---------------------------------------------------------------------------*/
/*                          Private functions                                */
/*---------------------------------------------------------------------------*/

/******************************************************************************
  Function:     FlashCopy
  Description:  ��FLASH��,��ָ�����򿽱���ָ������.��С����Ϊ��λ
  Input:        uint32_t dest_addr  FLASHĿ���ַ(4�ֽڶ���)
                uint32_t src_addr   FLAHSԴ��ַ(4�ֽڶ���)
                uint16_t count      ��������/�ֽ�(������2�ı���)
  Return:       0:�����ɹ�
                1:����ʧ��
                2:������С����
  Others:       ����һ�����⿽������,������У����ȷ��,��д��ɹ���־0xaaaaaaaa,���Կ������򲻰����ñ�־
                �������ʹ�ñȽ�����.Ŀǰû�ӿ��ַ�����,������Ϊͨ��FLASH����.�����ڿ���ϵͳ����
******************************************************************************/
static uint8_t FlashCopy(uint32_t dest_addr, uint32_t src_addr, uint32_t _ulSize)
{
    uint32_t i;
    uint16_t * src,*dest;
    uint32_t addr = dest_addr;
    /*Variable used for Erase procedure*/
    static FLASH_EraseInitTypeDef EraseInitStruct;
    uint32_t PAGEError = 0;

    /* ����Ϊ0 ʱ����������  */
    if (_ulSize == 0)
    {
        return 0;
    }

    /* ����Ϊ����ʱ����������,��֤���ֶ���,��Ϊ�ǰ��հ��ַ�ʽд�� */
    if ((_ulSize % 2) != 0)
    {
        return 2;
    }

    __set_PRIMASK(1);                                                /* ���ж� */
    /* Unlock the Flash to enable the flash control register access *************/
    HAL_FLASH_Unlock();

    //��������Ϣд��Param backup����
    /* Fill EraseInit structure*/
    EraseInitStruct.TypeErase = FLASH_TYPEERASE_PAGES;
    EraseInitStruct.PageAddress = bsp_GetSector(addr);           //FLASH_USER_START_ADDR;
    i = dest_addr - EraseInitStruct.PageAddress + _ulSize + 4;/* ע�����Ҫд�������־λ4�ֽ� */
    EraseInitStruct.NbPages = i / FLASH_PAGE_SIZE;
    if (i % FLASH_PAGE_SIZE)
    {
        EraseInitStruct.NbPages++;
    }

    if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
    {
        goto PROGRAM_ERROR;
    }

    /* ���򿽱�����������������־ */
    addr = dest_addr;
    src = (uint16_t *)src_addr;

    for (i = 0; i < _ulSize / 2; i++)
    {
        if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, addr, *src++) != HAL_OK)
        {
            goto PROGRAM_ERROR;
        }

        addr += 2;
    }

    /* ����У�� */
    dest = (uint16_t *)dest_addr;
    src = (uint16_t *)src_addr;
    i = _ulSize >> 1;
    while (i--)
    {
        if (*src++ != *dest++)
        {
            goto PROGRAM_ERROR;
        }
    }

    /* ����У����ȷ��д�������־ */
    if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, (uint32_t)dest, FLASH_DATA_END_FLAG) != HAL_OK) //д�������־
    {
        goto PROGRAM_ERROR;
    }

    if (*(uint32_t *)(dest_addr + _ulSize) != FLASH_DATA_END_FLAG)
    {
#if SAVE_PARAMETER_DEBUG_EN
        DEBUG_PrintfString("Flash data copy fail.\n");
#endif
        goto PROGRAM_ERROR;
    }

#if SAVE_PARAMETER_DEBUG_EN
    DEBUG_PrintfString("Flash data copy sucess.\n");
#endif
    HAL_FLASH_Lock();
    __set_PRIMASK(0);                                                // ���ж�
    return 0;
    
PROGRAM_ERROR:
    /* Flash ��������ֹдFlash���ƼĴ��� */
    /* Lock the Flash to disable the flash control register access (recommended
     to protect the FLASH memory against possible unwanted operation) *********/
    HAL_FLASH_Lock();
    __set_PRIMASK(0);                                                // ���ж�
    return 1;
}


/*
*********************************************************************************************************
*	�� �� ��: LoadParam
*	����˵��: ��Flash��������g_tParam
*	��    �Σ���
*	�� �� ֵ: 0:��ȡ���ݳɹ�,������Parameter��Parameter backup�������ݶ�����
            1:���ݴӲ�����Parameter��ȡ�ɹ�����������Parameter����������������Parameter backupд��ʧ��
            2:���ݴӲ���������Parameter backup��ȡ�ɹ���������������Parameter backup������������Parameterд��ʧ��
            3:�ָ�Ĭ�ϲ����ɹ�
            4:�ָ�Ĭ�ϲ���,�����浽FLASHʱ�д���
            5:��������
*********************************************************************************************************
*/
uint8_t LoadParam(void)
{
    uint8_t res;

    if (*(uint32_t *)(PARAM_START_ADDR + sizeof(PARAM_T) + 4) == FLASH_DATA_END_FLAG) //������������������
    {
#ifdef PARAM_SAVE_TO_FLASH
        /* �Ӳ�����Param��ȡCPU Flash�еĲ��� */
        bsp_ReadCpuFlash(PARAM_START_ADDR + 4, (uint8_t *)&g_tParam, sizeof(PARAM_T)); //ע��ǰ4���ֽ�����ʼ��־
#endif

#if SAVE_PARAMETER_DEBUG_EN
        DEBUG_PrintfString("Read flash data ok.\n");
#endif

        /* ���������������� */
        if (*(uint32_t *)(PARAM_BACKUP_START_ADDR + sizeof(PARAM_T) + 4) == FLASH_DATA_END_FLAG)
        {
#if SAVE_PARAMETER_DEBUG_EN
            DEBUG_PrintfString("Flash backup data ok.\n");
#endif
            return 0;
        } else
        {
            /* �Ѳ�����Param�����ݿ���������Param backup */
            res = FlashCopy(PARAM_BACKUP_START_ADDR, PARAM_START_ADDR, sizeof(PARAM_T) / 4 + 1); //������СΪ������������+FLASH ��������ʼ��־(4Byte),������־�Զ�д��
#if SAVE_PARAMETER_DEBUG_EN
            if (res == 0)
            {
                DEBUG_PrintfString("copy parameter to flash backup data ok.\n");
            } else
            {
                DEBUG_PrintfString("copy parameter to flash backup data fail.\n");
            }
#endif
            return res;
        }
    } else                                                             /* ���������������쳣 */
    {
#if SAVE_PARAMETER_DEBUG_EN
        DEBUG_PrintfString("Flash data region have no parameter.\n");
#endif

        /* ���������������� */
        if (*(uint32_t *)(PARAM_BACKUP_START_ADDR + sizeof(PARAM_T) + 4) == FLASH_DATA_END_FLAG)
        {

#if SAVE_PARAMETER_DEBUG_EN
            DEBUG_PrintfString("Flash backup data ok.\n");
#endif
            /* �Ӳ���������Param backup��ȡCPU Flash�еĲ��� */
            bsp_ReadCpuFlash(PARAM_BACKUP_START_ADDR + 4, (uint8_t *)&g_tParam, sizeof(PARAM_T)); //ע��ǰ4���ֽ�����ʼ��־

            /* �Ѳ�����Param�����ݿ���������Param backup */
            res = FlashCopy(PARAM_START_ADDR, PARAM_BACKUP_START_ADDR, sizeof(PARAM_T) / 4 + 1); //������СΪ������������+FLASH ��������ʼ��־(4Byte),������־�Զ�д��
            if (res == 0)
            {
#if SAVE_PARAMETER_DEBUG_EN
                DEBUG_PrintfString("copy parameter to flash backup data ok.\n");
#endif
                return res;
            } else
            {
#if SAVE_PARAMETER_DEBUG_EN
                DEBUG_PrintfString("copy parameter to flash backup data fail.\n");
#endif
                return 2;
            }
        } else                                                         /* ����������Parameter backup�����쳣 */
        {
#if SAVE_PARAMETER_DEBUG_EN
            DEBUG_PrintfString("Flash have no parameter.\nSet default parameter.\n");
#endif

            if (SetDefaultParam() == 0)
            {

#if SAVE_PARAMETER_DEBUG_EN
                DEBUG_PrintfString("Default parameter save sucessful.\n");
#endif
                return 3;                                            //3:�ָ�Ĭ�ϲ����ɹ�
            } else
            {
#if SAVE_PARAMETER_DEBUG_EN
                DEBUG_PrintfString("Default parameter save fail.\n");
#endif
                return 4;                                            //4:�ָ�Ĭ�ϲ���,�����浽FLASHʱ�д���
            }
        }
    }
}

/*
*********************************************************************************************************
*	�� �� ��: SaveParam
*	����˵��: ��ȫ�ֱ���g_tParam д�뵽CPU�ڲ�Flash
*	��    ��: ��
*	�� �� ֵ: 0:����ɹ�
             1:����������Paramд��ʧ��
             2:����������Param backupд��ʧ��
*********************************************************************************************************
*/
uint8_t SaveParam(void)
{
    uint8_t res;
#ifdef PARAM_SAVE_TO_FLASH
    /* ��ȫ�ֵĲ����������浽 CPU Flash */
    res = bsp_WriteCpuFlash(PARAM_START_ADDR, (unsigned char *)&g_tParam, sizeof(PARAM_T));
#endif

#if SAVE_PARAMETER_DEBUG_EN
    if (res == 0)
    {
        DEBUG_PrintfString("Flash data region write successfully.\n");
    } else
    {
        DEBUG_PrintfString("Flash data region write fail.\n");
    }
#endif

    /* �Ѳ�����Param�����ݿ���������Param backup */
    /* ������СΪ������������+FLASH ��������ʼ��־(4Byte),������־�Զ�д�� */
    res = FlashCopy(PARAM_BACKUP_START_ADDR, PARAM_START_ADDR, sizeof(PARAM_T) + 4); //�����ߴ�Ҫ������ʵ��־4���ֽ�

#if SAVE_PARAMETER_DEBUG_EN
    if (res == 0)
    {
        DEBUG_PrintfString("copy data to backup region successfully.\n");
    } else
    {
        DEBUG_PrintfString("copy data to backup region fail.\n");
    }
#endif
    return res;
}

/* ******************************************************************************************************
*	�� �� ��: SetDefaultParam
*	����˵��: ���ò���ΪĬ��ֵ,�����浽FLASH��
*   ��    ��: ��
*	�� �� ֵ: 0:����ɹ�
             1:����������Paramд��ʧ��
             2:����������Param backupд��ʧ��
****************************************************************************************************** */
uint8_t SetDefaultParam(void)
{
    uint8_t i;
    static const char mn[] = "88888820150001";                       //Ĭ��MN��,ע�ⲻ�ܳ���14���ַ�
    static const char default_apn[] = "cmnet";                       //Ĭ��apn��

    g_tParam.Version = guiVersion;

    for (i = 0; i < 16; i++)
    {
        g_tParam.DevNO[i] = '\0';
    }
    strncpy(g_tParam.DevNO, mn, sizeof(mn));

    g_tParam.Pwd = 123456;
    g_tParam.u16_SaveRtdInterval = 1;
    g_tParam.u8_ST = 39;

    g_tParam.Server[0].ip[0] = 120;
    g_tParam.Server[0].ip[1] = 197;
    g_tParam.Server[0].ip[2] = 59;
    g_tParam.Server[0].ip[3] = 4;
    g_tParam.Server[0].port = 9895;
    g_tParam.Server[0].rtd_interval = 1;
    g_tParam.Server[0].uip_conn = NULL;
    g_tParam.Server[0].heartbeart_period = 60;

    /* ��˾���ƺ�̨,��Ҫ�޸ģ� */
    g_tParam.Server[1].ip[0] = 120;
    g_tParam.Server[1].ip[1] = 197;
    g_tParam.Server[1].ip[2] = 59;
    g_tParam.Server[1].ip[3] = 4;
    g_tParam.Server[1].port = 0;
    g_tParam.Server[1].rtd_interval = 1;
    g_tParam.Server[1].uip_conn = NULL;
    g_tParam.Server[1].heartbeart_period = 60;

    g_tParam.Server[2].ip[0] = 192;
    g_tParam.Server[2].ip[1] = 168;
    g_tParam.Server[2].ip[2] = 1;
    g_tParam.Server[2].ip[3] = 100;
    g_tParam.Server[2].port = 0;
    g_tParam.Server[2].rtd_interval = 1;
    g_tParam.Server[2].uip_conn = NULL;
    g_tParam.Server[2].heartbeart_period = 60;

    g_tParam.Server[3].ip[0] = 192;
    g_tParam.Server[3].ip[1] = 168;
    g_tParam.Server[3].ip[2] = 1;
    g_tParam.Server[3].ip[3] = 100;
    g_tParam.Server[3].port = 0;
    g_tParam.Server[3].rtd_interval = 1;
    g_tParam.Server[3].uip_conn = NULL;
    g_tParam.Server[3].heartbeart_period = 60;

    g_tParam.local_ip[0] = IP_ADDR0;
    g_tParam.local_ip[1] = IP_ADDR1;
    g_tParam.local_ip[2] = IP_ADDR2;
    g_tParam.local_ip[3] = IP_ADDR3;

    g_tParam.FanChannel = 5;
    g_tParam.CleanChannel = 4;
    g_tParam.threshold = 100;                                        //��������ֵ100uA,ʵ��ֵ100mA

    g_tParam.NTP_En = 1;                                             //Ĭ��ʹ��NTP��ʱ����
    g_tParam.SystemFlag = 0x01;                                      //g_tParam.SystemFlag

    g_tParam.apn[0] = '3';
    g_tParam.apn[1] = 'g';
    g_tParam.apn[2] = 'n';
    g_tParam.apn[3] = 'e';
    g_tParam.apn[4] = 't';
    g_tParam.apn[5] = '\0';

    return SaveParam();                                              /* ���²���д��Flash */
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
