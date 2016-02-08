/******************************* (C) LiteMesh **********************************
 * @module  LCD
 * @file    lcd.h
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   lcd
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef LCD_H
#define LCD_H

#include <stdbool.h>

/* Define of used TIM, GPIOs ports and the TIM interrupt channel -------------*/

#define LCD_TIMER                 TIM4 /* Timer used to drive tle LCD glass */

#define LCD_TIM_IRQChannel        TIM4_IRQn /* The used interrupt channel */

#define RCC_APB1Periph_TIMx       RCC_APB1Periph_TIM4 /* Activate the used TIM clock */

/* Enable the used GPIOs clocks */

#define RCC_APB2Periph_Used_GPIO  RCC_APB2Periph_GPIOC | RCC_APB2Periph_GPIOD | RCC_APB2Periph_GPIOE 

/* Configuration of the two GPIOs which drive the LCD (power on/off) -------- */

#define LCD_BiasPort     GPIOA       /* Port which the LCD bias pin is connected */

#define LCD_BiasPlusPin  GPIO_Pin_0  /* The number of the first GPIO (LCD bias plus)   
                                         which will power-off the resistor bridge */

/* Port where the segment lines are connected */
#define LCD_SegmentsPort  GPIOB 

/* Common lines configuration ----------------------------------------------- */
#define LCD_CommonLinesPort  GPIOB /* Port where the 4 common lines are connected */

#define CommonLines_EightHighPortPins  
                                /*  The previous line is to define where Common
                                    lines are located:
                                    - Uncomment this line: the 4 common lines  
                                      are located between PX.8 and PX.15
                                    - Comment this line: the 4 common lines  
                                      are located between PX.0 and PX.7
                                */

#define LCD_CommonLinesPinOffset   4   /* Pin offset (do not exceed 4: [0..4]) 
                                       (offset of eight low port pins or
                                       eight high port pins according to
                                       CommonLines_EightHighPortPins commented
                                       or not */

/* LCD contarst value */
#define LCD_DefaultContrast      0x450 

/* LCD interrupt priority configuration ------------------------------------- */
#define LCD_Priority_Value       3

#define CommonLines_Pin_Offset   4

#define ALL_COMMON_LINES  (uint16_t)(0x000F << (LCD_CommonLinesPinOffset + 8))

#define ALL_COMMON_LINES_PP   (0x3333 << (LCD_CommonLinesPinOffset * 4))

#define ALL_SEGMENT_LINES        0xFFFF

typedef struct
{
  uint16_t com1;
  uint16_t com2;
  uint16_t com3;
  
}DIGIT_t;
										
void LCD_Test(void);
										
/**
  * @brief  
  */
void LCD_Init(void);
                                  
void LCD_InitNVIC(void);

void LCD_InitGpio(void);

/**
  * @brief  Настройка таймера: TIM4 (канал 1 GPIOA pin6)
  *         Формирование ШИМ сигнала 4-ех каналов таймера
  */
void LCD_InitTim(void);

/**
  * @brief  Настройка таймеров: TIM3 (канал 1, канал 2)
  *                             TIM1 (канал 3) 
  */
void TIM_InitHallSensor(void);

/**
  * @brief  Настройка TIM4
  */
void TIM_InitIgnitionTim(void);

void TIM_OnePulseTest(void);

/**
  * @brief  computes the offset to add to the GPIO masks
  */
uint32_t LCD_OffsetValue(uint8_t Pin_offset);

/**
  * @brief  initilizes the LCD GPIOs masks.
  */
void LCD_ComsMasksInit(void);

/**
  * @brief  converts an ascii char to the a LCD digit (previous coding)
  * @param c: a char to display.
  * @retval : None.
  */
void LCD_Convert(char* c);

void LCD_WriteChar(char* car, uint8_t position);

/**
  * @brief  This function writes a string in the LCD
  */
void LCD_WriteString(char* str);

/**
  * @brief  Заполнение первого и второго знакоместа.
  *         Вывод беззнакового целого от 0 до 99.
  */
void LCD_WriteLeftPart(uint8_t value);

/**
  * @brief  Заполнение третьего и четвертого знакоместа.
  *         Вывод беззнакового целого от 0 до 99.
  */
void LCD_WriteRightPart(uint8_t value);

/**
  * @brief  Заполнение третьего знакоместа.
  */
void LCD_WriteThirdDigit(uint8_t value);

/**
  * @brief  Вывод дня недели. Пн 1, Вт 2, Ср 3, Чт 4, Пт 5, Сб 6, Вс 0.  
  * @param  value
  * @retval None.
  */
void LCD_WriteWeekDay(uint8_t value);

/**
  * @brief  Вывод дней недели
  */
void LCD_WriteWeekDays(uint8_t *buf);

/**
  * @brief  Вывести/убрать день недели. Пн 1, Вт 2, Ср 3, Чт 4, Пт 5, Сб 6, Вс 0.  
  */
void LCD_WeekDay(uint8_t day, bool state);

/**
  * @brief  Мигает символом дня недели
  */
void LCD_WeekdayBlink(uint8_t day);

/**
  * @brief  Вывод года 
  */
void LCD_WriteYear(uint16_t year);

/**
  * @brief  Мигает символом двоеточия
  */
void LCD_ColonToggle(void);

/**
  * @brief  Вкл/выкл символ двоеточия
  */
void LCD_Colon(bool state);

/**
  * @brief  Вкл/выкл символ часов
  */
void LCD_TimeSimbol(bool state);

/**
  * @brief  Вкл/выкл символ "Рука"
  */
void LCD_HandSimbol(bool state);

/**
  * @brief  Мигает символом "Рука"
  */
void LCD_HandBlink(void);

/**
  * @brief  Вкл/выкл символ "1"
  */
void LCD_1_Simbol(bool state);

/**
  * @brief  Мигает символом "1"
  */
void LCD_1_Blink(void);

/**
  * @brief  Вкл/выкл символ "2"
  */
void LCD_2_Simbol(bool state);

/**
  * @brief  Мигает символом "2"
  */
void LCD_2_Blink(void);

/**
  * @brief  Выводит символ термометра
  */
void LCD_ThermometerSimbol(bool state);

/**
  * @brief  Показания датчика температуры
  */
void LCD_Temperature(float temp);

/**
  * @brief  Вкл/выкл символ "точка"
  */
void LCD_DotSimbol(bool state);

/**
  * @brief  Осуществляет мигание четырех знакомест
  */
void LCD_TimeBlink(uint8_t hours, uint8_t minutes);

/**
  * @brief  Осуществляет мигание первого и второго знакоместа
  */
void LCD_HourBlink(uint8_t hours);

/**
  * @brief  Осуществляет мигание третьего и четвертого знакоместа
  */
void LCD_MinuteBlink(uint8_t minutes);

/**
  * @brief  Осуществляет мигание всех четырех знакоместа
  */
void LCD_YearBlink(uint16_t year);

/**
  * @brief  Очищает 4 знакоместа
  */
void LCD_ClearDigits(void);

/**
  * @brief  Вывод беззнакового целого от 0 до 9999
  */
void LCD_WriteInt(uint16_t value);

/**
  * @brief  
  * @retval 
  */


#endif /* #ifndef LCD_H */

/****************************** (C) LiteMesh ***************** end of file ****/