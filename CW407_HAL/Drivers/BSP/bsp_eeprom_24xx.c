/*
*********************************************************************************************************
*
*	ģ������ : ����EEPROM 24xx����ģ��
*	�ļ����� : bsp_eeprom_24xx.c
*	��    �� : V1.0
*	˵    �� : ʵ��24xxϵ��EEPROM�Ķ�д������д��������ҳдģʽ���д��Ч�ʡ�
*
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

/*
    Ӧ��˵�������ʴ���EEPROMǰ�����ȵ���һ�� bsp_InitI2C()�������ú�I2C��ص�GPIO.
*/

#include "bsp.h"
#include "stm32f4xx_hal.h"

/*
*********************************************************************************************************
*	�� �� ��: EEPROM_WriteData
*	����˵��: ��EERPOMд������
*	��    ��:  ��
*	�� �� ֵ: 0 ��ʾ������ other ��ʾ������
*********************************************************************************************************
*/
HAL_StatusTypeDef EEPROM_WriteData(uint16_t _usMemAddress, uint8_t *_pucData, uint16_t _usSize)
{
   HAL_StatusTypeDef sta;

   while (_usSize--)
   {
      sta = HAL_I2C_Mem_Write(&hi2c1, EE_DEV_ADDR, _usMemAddress++, EE_ADD_SIZE, _pucData++, 1, 0xffff);
      if (sta != HAL_OK)
          return sta;
      osDelay(4);//��ͬ������ʱ���ܲ�һ��
   }
   return sta;
}

/*
*********************************************************************************************************
*	�� �� ��: EEPROM_WriteData
*	����˵��: ��EERPOMд������
*	��    ��:  ��
*	�� �� ֵ: 0 ��ʾ������ other ��ʾ������
*********************************************************************************************************
*/
HAL_StatusTypeDef EEPROM_ReadData(uint16_t _usMemAddress, uint8_t *_pucData, uint16_t _usSize)
{
   HAL_StatusTypeDef sta;
   while (_usSize--)
   {
      sta = HAL_I2C_Mem_Read(&hi2c1, EE_DEV_ADDR, _usMemAddress++, EE_ADD_SIZE, _pucData++, 1, 0xffffffff);
      if (sta != HAL_OK)
          return sta;
      osDelay(4);//��ͬ������ʱ���ܲ�һ��
   }
   return sta;
}



/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
