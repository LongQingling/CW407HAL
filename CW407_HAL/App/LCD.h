/****************** Copyright(c) ��������Ƽ���չ���޹�˾ ***********************/
/* Define to prevent recursive inclusion ------------------------------------ */
#ifndef __LCD_H__
#define __LCD_H__

/* Includes ----------------------------------------------------------------- */
/* Private macros ----------------------------------------------------------- */
/******** ������Ϣ *******/
#define LCD_COM     COM1      /* ����LCD���� */

/* Exported types ----------------------------------------------------------- */
/* Exported constants ------------------------------------------------------- */
/* Exported macros ---------------------------------------------------------- */
/* Exported variables ------------------------------------------------------- */
extern uint8_t fan_last_state;                      //�����һ�εĿ���״̬
extern uint8_t clean_last_state;                    //��������һ�εĿ���״̬
extern volatile uint8_t LCD_ScreenId;               //������ʾ����ǰ����ID,���ڸ�����ʾ
/* Exported functions ------------------------------------------------------- */
void LCD_Icon(uint8_t screen_id, uint8_t icon_id, uint8_t state);
void LCD_Gif(uint8_t screen_id, uint8_t control_id, uint8_t state);
void LCD_Txt(uint8_t screen_id, uint8_t icon_id, char* str);
void LCD_SetScreen(uint8_t screen_id);
void UpdateLcdDisplay(void);
void UpdateLcdTime(void);
uint8_t LCD_CmdHandle(void);
void DisplayNetState(void);
void LCD_Init(void);

#endif /*__LCD_H__*/

/********* (C) COPYRIGHT 2015-2025 ��������Ƽ���չ���޹�˾ ****END OF FILE****/

