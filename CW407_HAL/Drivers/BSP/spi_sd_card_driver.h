#ifndef _SPI_SD_CARD_DRIVER_H_
#define _SPI_SD_CARD_DRIVER_H_

#include "stm32f4xx_hal.h"
/* Private define ------------------------------------------------------------*/
#define SD_DEBUG_EN 0   //Enable debug function

/* SD�����Ͷ��� */
//#define SD_TYPE_MMC     0
//#define SD_TYPE_V1      1
//#define SD_TYPE_V2      2
//#define SD_TYPE_V2HC    4
typedef enum
{
    SD_TYPE_NO_CARD = 0, 
    SD_TYPE_MMC     = 1, 
    SD_TYPE_V1      = 2, 
    SD_TYPE_V2      = 3, 
    SD_TYPE_V2HC    = 4 
}SD_TypeDef;

/* SPI�����ٶ�����*/
#define SPI_SPEED_LOW   0
#define SPI_SPEED_HIGH  1

/* SD�������ݽ������Ƿ��ͷ����ߺ궨�� */
#define NO_RELEASE      0
#define RELEASE         1

/* SD��ָ��� */
#define CMD0    0       //����λ
#define CMD8    8       //����8
#define CMD9    9       //����9 ����CSD����
#define CMD10   10      //����10����CID����
#define CMD12   12      //����12��ֹͣ���ݴ���
#define CMD16   16      //����16������SectorSize Ӧ����0x00
#define CMD17   17      //����17����sector
#define CMD18   18      //����18����Multi sector
#define ACMD23  23      //����23�����ö�sectorд��ǰԤ�Ȳ���N��block
#define CMD24   24      //����24��дsector
#define CMD25   25      //����25��дMulti sector
#define ACMD41  41      //����41��Ӧ����0x00
#define CMD55   55      //����55��Ӧ����0x01
#define CMD58   58      //����58����OCR��Ϣ
#define CMD59   59      //����59��ʹ��/��ֹCRC��Ӧ����0x00

/* Private macro -------------------------------------------------------------*/
/* SD card parameter */
#define SD_CARD_SECTOR_ZISE  512     //����SD��������С


/* ************************** ��Ҫ��ֲ�Ľӿں��� ****************************** */
//SD��CSƬѡʹ�ܶ˲����� PE5
#define SD_CS_ENABLE()      HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_RESET)    //ѡ��SD��
#define SD_CS_DISABLE()     HAL_GPIO_WritePin(GPIOE,GPIO_PIN_5,GPIO_PIN_SET)      //��ѡ��SD��
#define SD_DET()           1 									                  //û��Ӳ�����  //1-�� 0-��  PA1

/* SD����Դ���� PE7 */
#define SD_PWR_ON()         HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_RESET)    //SD����Դʹ��
#define SD_PWR_OFF()        HAL_GPIO_WritePin(GPIOE,GPIO_PIN_7,GPIO_PIN_SET)      //SD����Դ����
/* ************************* ��Ҫ��ֲ�Ľӿں��� end***************************** */

/* Private function prototypes -----------------------------------------------*/

/* Exported variables ------------------------------------------------------- */
extern SD_TypeDef SD_Type;  //���濨������
extern const unsigned char *SdTypeTab[];

/* Exported functions ------------------------------------------------------- */
void SD_SPI_Configuration(void);
void SPI_SetSpeed(unsigned char SpeedSet);
unsigned char SPI_ReadWriteByte(unsigned char TxData);
unsigned char SD_WaitReady(void);
unsigned char SD_SendCommand(unsigned char cmd, unsigned long arg, unsigned char crc);
unsigned char SD_SendCommand_NoDeassert(unsigned char cmd, unsigned long arg, unsigned char crc);
unsigned char SD_Init(void);
unsigned char SD_ReadSingleBlock(unsigned long sector, unsigned char *buffer);
unsigned char SD_ReadMultiBlocks( unsigned long sector, unsigned char *buffer, unsigned char count);
unsigned char SD_WriteSingleBlock(unsigned long sector, const unsigned char *data);
unsigned char SD_WriteMultiBlocks(unsigned long sector, const unsigned char *data, unsigned char count);
unsigned long SD_GetCapacity(void);

#endif //_SPI_SD_CARD_DRIVER_H_

