/*
*********************************************************************************************************
*
*	ģ������ : cpu�ڲ�falsh����ģ��(for F4)
*	�ļ����� : bsp_cpu_flash.c
*	��    �� : V1.0
*	˵    �� : �ṩ��дCPU�ڲ�Flash�ĺ���
*	�޸ļ�¼ :
*		�汾��  ����        ����     ˵��
*		V1.0    2013-02-01 armfly  ��ʽ����
*
*	Copyright (C), 2013-2014, ���������� www.armfly.com
*
*********************************************************************************************************
*/

#include "bsp.h"

//#define SECTOR_MASK			0xFFFFF800  /* ������оƬ2kB/ҳ������ */
#define SECTOR_MASK			0xFFFFFC00  /* ��С����оƬ1kB/ҳ������ */

/*Variable used for Erase procedure*/
static FLASH_EraseInitTypeDef EraseInitStruct;
/*
*********************************************************************************************************
*	�� �� ��: bsp_GetSector
*	����˵��: ���ݵ�ַ���������׵�ַ
*	��    �Σ���
*	�� �� ֵ: �����׵�ַ
*********************************************************************************************************
*/
uint32_t bsp_GetSector(uint32_t _ulWrAddr)
{
   uint32_t sector = 0;

   sector = _ulWrAddr & SECTOR_MASK;

   return sector;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_ReadCpuFlash
*	����˵��: ��ȡCPU Flash������
*	��    �Σ�_ucpDst : Ŀ�껺����
*			 _ulFlashAddr : ��ʼ��ַ
*			 _ulSize : ���ݴ�С����λ���ֽڣ�
*	�� �� ֵ: 0=�ɹ���1=ʧ��
*********************************************************************************************************
*/
uint8_t bsp_ReadCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpDst, uint32_t _ulSize)
{
   uint32_t i;

   /* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
   if (_ulFlashAddr + _ulSize > FLASH_BASE_ADDR + FLASH_SIZE)
   {
      return 1;
   }

   /* ����Ϊ0ʱ����������,������ʼ��ַΪ���ַ����� */
   if (_ulSize == 0)
   {
      return 1;
   }

   for (i = 0; i < _ulSize; i++)
   {
      *_ucpDst++ = *(uint8_t *)_ulFlashAddr++;
   }

   return 0;
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_CmpCpuFlash
*	����˵��: �Ƚ�Flashָ����ַ������.
*	��    ��: _ulFlashAddr : Flash��ַ
*			 _ucpBuf : ���ݻ�����
*			 _ulSize : ���ݴ�С����λ���ֽڣ�
*	�� �� ֵ:
*			FLASH_IS_EQU		0   Flash���ݺʹ�д���������ȣ�����Ҫ������д����
*			FLASH_REQ_WRITE		1	Flash����Ҫ������ֱ��д
*			FLASH_REQ_ERASE		2	Flash��Ҫ�Ȳ���,��д
*			FLASH_PARAM_ERR		3	������������
*********************************************************************************************************
*/
uint8_t bsp_CmpCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpBuf, uint32_t _ulSize)
{
   uint32_t i;
   uint8_t ucIsEqu;    /* ��ȱ�־ */
   uint8_t ucByte;

   /* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
   if (_ulFlashAddr + _ulSize > FLASH_BASE_ADDR + FLASH_SIZE)
   {
      return FLASH_PARAM_ERR;     /*��������������*/
   }

   /* ����Ϊ0ʱ������ȷ */
   if (_ulSize == 0)
   {
      return FLASH_IS_EQU;        /* Flash���ݺʹ�д���������� */
   }

   ucIsEqu = 1;            /* �ȼ��������ֽںʹ�д���������ȣ���������κ�һ������ȣ�������Ϊ 0 */
   for (i = 0; i < _ulSize; i++)
   {
      ucByte = *(uint8_t *)_ulFlashAddr;

      if (ucByte != *_ucpBuf)
      {
         if (ucByte != 0xFF)
         {
            return FLASH_REQ_ERASE;     /* ��Ҫ��������д */
         }
         else
         {
            ucIsEqu = 0;    /* ����ȣ���Ҫд */
         }
      }

      _ulFlashAddr++;
      _ucpBuf++;
   }

   if (ucIsEqu == 1)
   {
      return FLASH_IS_EQU;    /* Flash���ݺʹ�д���������ȣ�����Ҫ������д���� */
   }
   else
   {
      return FLASH_REQ_WRITE; /* Flash����Ҫ������ֱ��д */
   }
}

/*
*********************************************************************************************************
*	�� �� ��: bsp_WriteCpuFlash
*	����˵��: д�������ݵ�CPU �ڲ�Flash,�Զ��Ѳ�����ǰ�����ı�־д��FLASH��
*	��    ��: _ulFlashAddr : Flash��ַ,����ʹ���������׵�ַ
*			 _ucpSrc : ���ݻ�����
*			 _ulSize : ���ݴ�С����λ���ֽڣ�
*	�� �� ֵ: 0-�ɹ���1-���ݳ��Ȼ��ַ�����2-дFlash����(����Flash������,��������д�����)
*********************************************************************************************************
*/
uint8_t bsp_WriteCpuFlash(uint32_t _ulFlashAddr, uint8_t *_ucpSrc, uint32_t _ulSize)
{
   uint32_t i;
   uint8_t ucRet;
   uint16_t usTemp;
   uint32_t ulFlashAddrBackup = _ulFlashAddr;
   uint32_t PAGEError = 0;

   /* ���ƫ�Ƶ�ַ����оƬ�������򲻸�д��������� */
   if (_ulFlashAddr + _ulSize > FLASH_BASE_ADDR + FLASH_SIZE)
   {
      return 1;
   }

   /* ����Ϊ0 ʱ����������  */
   if (_ulSize == 0)
   {
      return 0;
   }

   /* ����Ϊ����ʱ����������,��֤���ֶ���,��Ϊ�ǰ��հ��ַ�ʽд�� */
   if ((_ulSize % 2) != 0)
   {
      return 1;
   }

   ucRet = bsp_CmpCpuFlash(_ulFlashAddr + 4, _ucpSrc, _ulSize);

   if (ucRet == FLASH_IS_EQU)
   {
      return 0;
   }

   __set_PRIMASK(1);       /* ���ж� */

   /* Unlock the Flash to enable the flash control register access *************/
   HAL_FLASH_Unlock();

   /* ��Ҫ���� */
// if (ucRet == FLASH_REQ_ERASE)
   {
      /* Fill EraseInit structure*/
      EraseInitStruct.TypeErase   = FLASH_TYPEERASE_PAGES;
      EraseInitStruct.PageAddress = bsp_GetSector(_ulFlashAddr); //FLASH_USER_START_ADDR;
      i= _ulFlashAddr - EraseInitStruct.PageAddress + _ulSize;
      EraseInitStruct.NbPages = i / FLASH_PAGE_SIZE;
      if (i%FLASH_PAGE_SIZE)
      {
          EraseInitStruct.NbPages++;
      }

      if (HAL_FLASHEx_Erase(&EraseInitStruct, &PAGEError) != HAL_OK)
      {
         goto PROGRAM_ERROR;
      }
   }

   /* д����ʼ��־ */
   if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, _ulFlashAddr, FLASH_DATA_BEGIN_FLAG) != HAL_OK)
   {
      goto PROGRAM_ERROR;
   }
   _ulFlashAddr += 4;

   /* ������ģʽ��̣�Ϊ���Ч�ʣ����԰��ֱ�̣�һ��д��4�ֽڣ� */
   for (i = 0; i < _ulSize / 2; i++)
   {
      usTemp = _ucpSrc[2 * i];
      usTemp |= (_ucpSrc[2 * i + 1] << 8);
      if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_HALFWORD, _ulFlashAddr, usTemp) != HAL_OK)
      {
         goto PROGRAM_ERROR;
      }

      _ulFlashAddr += 2;
   }

   /* У�� */
   if (*(uint32_t *)ulFlashAddrBackup != FLASH_DATA_BEGIN_FLAG)
   {
      goto PROGRAM_ERROR;
   }

   ucRet = bsp_CmpCpuFlash(ulFlashAddrBackup + 4, _ucpSrc, _ulSize);

   if (ucRet != FLASH_IS_EQU)
   {
      goto PROGRAM_ERROR;
   }

   /* д�������־ */
   if (HAL_FLASH_Program(FLASH_TYPEPROGRAM_WORD, _ulFlashAddr, FLASH_DATA_END_FLAG) != HAL_OK)
   {
      goto PROGRAM_ERROR;
   }

   /* Flash ��������ֹдFlash���ƼĴ��� */
   /* Lock the Flash to disable the flash control register access (recommended
    to protect the FLASH memory against possible unwanted operation) *********/
   HAL_FLASH_Lock();

   __set_PRIMASK(0);       /* ���ж� */


   return 0;
PROGRAM_ERROR:
   /* Flash ��������ֹдFlash���ƼĴ��� */
   /* Lock the Flash to disable the flash control register access (recommended
      to protect the FLASH memory against possible unwanted operation) *********/
   HAL_FLASH_Lock();
   __set_PRIMASK(0);       /* ���ж� */
   return 2;
}

/***************************** ���������� www.armfly.com (END OF FILE) *********************************/
