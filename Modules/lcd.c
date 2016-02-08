/******************************* (C) LiteMesh **********************************
 * @module  LCD
 * @file    lcd.c
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   lcd
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

#include "stm32f10x.h"
#include "lcd.h"
  
uint8_t LCD_PhaseExecute = 0;
uint8_t LCD_SegmentIndex = 0;

uint16_t  SegmentsValues_Lower_Quarter_Digits[4];

uint16_t  digit[3];
uint16_t  LCD_PortVal[4] = {0};
uint16_t  temp = 0;

uint16_t  CommonLine[4] = {
                            1 << (LCD_CommonLinesPinOffset + 8),
                            2 << (LCD_CommonLinesPinOffset + 8),
                            4 << (LCD_CommonLinesPinOffset + 8),
                            8 << (LCD_CommonLinesPinOffset + 8)
                          };


uint32_t  CommonLine_VDD_2[4] = { 
                            0xFFFF000F,
                            0xFFFF00F0,
                            0xFFFF0F00,
                            0xFFFFF000
                          };

/* GPIOs to be configured to Output PP */
uint32_t  CommonLine_OUT_PP[4] = {
                            0x00000003,
                            0x00000030,
                            0x00000300,
                            0x00003000
                           };


DIGIT_t sDigit_1[10];
DIGIT_t sDigit_2[10];
DIGIT_t sDigit_3[10];
DIGIT_t sDigit_4[10];

/**
  * @brief  Маски для вывода/погашения дней недели
  */
uint16_t WeekDayMask[7] = {0x20, 0x800, 0x4, 0x2, 0x8, 0x80, 0x10};
uint16_t WeekDayClearMask[7] = {0xFFDF, 0xF7FF, 0xFFFB, 0xFFFD, 0xFFF7, 0xFF7F, 0xFFEF};
	
void LCD_Test(void)
{
  
}

/**
  * @brief  Заполнение первого и второго знакоместа (левая часть экрана).
  *         Вывод беззнакового целого от 0 до 99.
  */
void LCD_WriteLeftPart(uint8_t value)
{
  uint8_t mostByte = 0;
  uint8_t lowByte = 0;
  
  /* Маска для очистки первого и второго знакоместа */ 
  const uint16_t mask[3] = {0xFBFE, 0xF3F0, 0xF3F0};
 
  /* Очищаем знакоместа от старых символов */ 
  LCD_PortVal[0] &= mask[0];
  LCD_PortVal[1] &= mask[1];
  LCD_PortVal[2] &= mask[2];
  
  /* Вычисляем старший и младший разряд числа */
  mostByte = value / 10;
  lowByte = value - mostByte*10;
  
  /* Пишем символы чисел в буфер экрана */
  LCD_PortVal[0] |= sDigit_1[mostByte].com1;
  LCD_PortVal[1] |= sDigit_1[mostByte].com2;
  LCD_PortVal[2] |= sDigit_1[mostByte].com3;
  
  LCD_PortVal[0] |= sDigit_2[lowByte].com1;
  LCD_PortVal[1] |= sDigit_2[lowByte].com2;
  LCD_PortVal[2] |= sDigit_2[lowByte].com3;
}

/**
  * @brief  Заполнение третьего и четвертого знакоместа.
  *         Вывод беззнакового целого от 0 до 99.
  */
void LCD_WriteRightPart(uint8_t value)
{
  uint8_t mostByte = 0;
  uint8_t lowByte = 0;
  
  /* Маска для очистки третьего и четвертого знакоместа */ 
  const uint16_t mask[3] = {0xFCFF, 0xFC0F, 0xFC0F};
 
  /* Очищаем знакоместа от старых символов */ 
  LCD_PortVal[0] &= mask[0];
  LCD_PortVal[1] &= mask[1];
  LCD_PortVal[2] &= mask[2];
  
  /* Вычисляем старший и младший разряд числа */
  mostByte = value / 10;
  lowByte = value - mostByte*10;
  
  /* Пишем символы чисел в буфер экрана */
  LCD_PortVal[0] |= sDigit_3[mostByte].com1;
  LCD_PortVal[1] |= sDigit_3[mostByte].com2;
  LCD_PortVal[2] |= sDigit_3[mostByte].com3;
  
  LCD_PortVal[0] |= sDigit_4[lowByte].com1;
  LCD_PortVal[1] |= sDigit_4[lowByte].com2;
  LCD_PortVal[2] |= sDigit_4[lowByte].com3;
}

/**
  * @brief  Заполнение третьего знакоместа.
  */
void LCD_WriteThirdDigit(uint8_t value)
{
  /* Маска для очистки третьего и четвертого знакоместа */ 
  const uint16_t mask[3] = {0xFCFF, 0xFC0F, 0xFC0F};
 
  /* Очищаем знакоместа от старых символов */ 
  LCD_PortVal[0] &= mask[0];
  LCD_PortVal[1] &= mask[1];
  LCD_PortVal[2] &= mask[2];
  
  /* Пишем символы чисел в буфер экрана */
  LCD_PortVal[0] |= sDigit_3[value].com1;
  LCD_PortVal[1] |= sDigit_3[value].com2;
  LCD_PortVal[2] |= sDigit_3[value].com3;
}

/**
  * @brief  Вывод дня недели. Пн 1, Вт 2, Ср 3, Чт 4, Пт 5, Сб 6, Вс 0.  
  */
void LCD_WriteWeekDay(uint8_t value)
{
  /* Маска для очистки всех символов дней недели */ 
  const uint16_t mask = 0xF741;
  
  /* Очистка старых символов */
  LCD_PortVal[0] &= mask;
  LCD_PortVal[0] |= WeekDayMask[value];
}

/**
  * @brief  Вывод дней недели
  */
void LCD_WriteWeekDays(uint8_t *buf)
{
  /* Маска для очистки всех символов дней недели */ 
  const uint16_t mask = 0xF741;
  
  /* Очистка старых символов */
  LCD_PortVal[0] &= mask;
  
  for (uint8_t i = 0; i < 7; i++)
  {
    if (*buf == 1)
	  LCD_PortVal[0] |= WeekDayMask[i];
	
	buf++;
  }	
}

/**
  * @brief  Вывести/убрать день недели. Пн 1, Вт 2, Ср 3, Чт 4, Пт 5, Сб 6, Вс 0.  
  */
void LCD_WeekDay(uint8_t day, bool state)
{
  if (state)
  {	
	LCD_PortVal[0] |= WeekDayMask[day];
	state = false;
  }	
  else
  {	
	LCD_PortVal[0] &= WeekDayClearMask[day];
	state = true;
  }
}

/**
  * @brief  Мигает символом дня недели
  */
void LCD_WeekdayBlink(uint8_t day)
{
  static bool state = true;
  
  if (state)
  {	
	LCD_WeekDay(day, state);
	state = false;
  }	
  else
  {	
	LCD_WeekDay(day, state);
	state = true;
  }
}

/**
  * @brief  Вывод года 
  */
void LCD_WriteYear(uint16_t year)
{
  /* Маска для очистки всех четырех знакоместа */   
  const uint16_t mask[3] = {0xF8FE, 0xF000, 0xF000};
  uint8_t thousend, hundres, dozen, unit;  
  
  /* Очищаем знакоместа от старых символов */ 
  LCD_PortVal[0] &= mask[0];
  LCD_PortVal[1] &= mask[1];
  LCD_PortVal[2] &= mask[2];
  
  /* Вычисляем разряды числа */
  thousend = year/1000;
  hundres = (year - thousend*1000)/100;
  dozen = (year - thousend*1000 - hundres*100)/10;
  unit = year - thousend*1000 - hundres*100 - dozen*10;
  
  /* Пишем символы чисел в буфер экрана */
  LCD_PortVal[0] |= sDigit_1[thousend].com1;
  LCD_PortVal[1] |= sDigit_1[thousend].com2;
  LCD_PortVal[2] |= sDigit_1[thousend].com3;
  
  LCD_PortVal[0] |= sDigit_2[hundres].com1;
  LCD_PortVal[1] |= sDigit_2[hundres].com2;
  LCD_PortVal[2] |= sDigit_2[hundres].com3;
  
  LCD_PortVal[0] |= sDigit_3[dozen].com1;
  LCD_PortVal[1] |= sDigit_3[dozen].com2;
  LCD_PortVal[2] |= sDigit_3[dozen].com3;
  
  LCD_PortVal[0] |= sDigit_4[unit].com1;
  LCD_PortVal[1] |= sDigit_4[unit].com2;
  LCD_PortVal[2] |= sDigit_4[unit].com3;
}

/**
  * @brief  Мигает символом двоеточия
  */
void LCD_ColonToggle(void)
{
  static bool state = true;
  
  if (state)
  {	
	LCD_PortVal[3] |= 0x8;
	state = false;
  }	
  else
  {	
	LCD_PortVal[3] &= 0xFFF7;
	state = true;
  }
}

/**
  * @brief  Вкл/выкл символ двоеточия
  */
void LCD_Colon(bool state)
{
  if (state)
	LCD_PortVal[3] |= 0x8;
  else
	LCD_PortVal[3] &= 0xFFF7;
}

/**
  * @brief  Вкл/выкл символ часов
  */
void LCD_TimeSimbol(bool state)
{
  if (state)
	LCD_PortVal[0] |= 0x40;
  else
	LCD_PortVal[0] &= 0xFFBF;
}

/**
  * @brief  Вкл/выкл символ "Рука"
  */
void LCD_HandSimbol(bool state)
{
  if (state)
	LCD_PortVal[3] |= 0x01;
  else
	LCD_PortVal[3] &= 0xFFFE;
}

/**
  * @brief  Мигает символом "Рука"
  */
void LCD_HandBlink(void)
{
  static bool state = true;
  
  if (state)
  {	
	LCD_HandSimbol(state);
	state = false;
  }	
  else
  {	
	LCD_HandSimbol(state);
	state = true;
  }
}

/**
  * @brief  Вкл/выкл символ "1"
  */
void LCD_1_Simbol(bool state)
{
    if (state)
	LCD_PortVal[3] |= 0x100;
  else
	LCD_PortVal[3] &= 0xFEFF;
}

/**
  * @brief  Мигает символом "1"
  */
void LCD_1_Blink(void)
{
  static bool state = true;
  
  if (state)
  {	
	LCD_1_Simbol(state);
	state = false;
  }	
  else
  {	
	LCD_1_Simbol(state);
	state = true;
  }
}

/**
  * @brief  Вкл/выкл символ "2"
  */
void LCD_2_Simbol(bool state)
{
    if (state)
	LCD_PortVal[3] |= 0x20;
  else
	LCD_PortVal[3] &= 0xFFDF;
}

/**
  * @brief  Мигает символом "2"
  */
void LCD_2_Blink(void)
{
  static bool state = true;
  
  if (state)
  {	
	LCD_2_Simbol(state);
	state = false;
  }	
  else
  {	
	LCD_2_Simbol(state);
	state = true;
  }
}

/**
  * @brief  Выводит символ термометра
  */
void LCD_ThermometerSimbol(bool state)
{
  if (state)
	LCD_PortVal[3] |= 0x40;
  else
	LCD_PortVal[3] &= 0xFFBF;
}

/**
  * @brief  Показания датчика температуры
  */
void LCD_Temperature(float temp)
{
  uint8_t intPart;
  uint8_t floatPart;
  
  if (temp < 100.0)
  {	
    intPart = (uint32_t)temp;
    floatPart = ((temp - intPart)*100)/10;
	
	LCD_WriteLeftPart(intPart);
	LCD_WriteThirdDigit(floatPart);
    LCD_ThermometerSimbol(true);
	LCD_DotSimbol(true);
  }
}

/**
  * @brief  Вкл/выкл символ "точка"
  */
void LCD_DotSimbol(bool state)
{
  if (state)
	LCD_PortVal[3] |= 0x200;
  else
	LCD_PortVal[3] &= 0xFDFF;
}

/**
  * @brief  Осуществляет мигание четырех знакомест
  */
void LCD_TimeBlink(uint8_t hours, uint8_t minutes)
{
  /* Маска для очистки всех четырех знакоместа */   
  const uint16_t mask[3] = {0xF8FE, 0xF000, 0xF000};
  static bool state = true;
  
  if (state)
  {	
	LCD_WriteRightPart(minutes);
	LCD_WriteLeftPart(hours);
	state = false;
  }	
  else
  {	
	LCD_PortVal[0] &= mask[0];
    LCD_PortVal[1] &= mask[1];
    LCD_PortVal[2] &= mask[2];
	state = true;
  }
}

/**
  * @brief  Осуществляет мигание первого и второго знакоместа
  */
void LCD_HourBlink(uint8_t hours)
{
  /* Маска для очистки первого и второго знакоместа */ 
  const uint16_t mask[3] = {0xFBFE, 0xF3F0, 0xF3F0};
  static bool state = true;
  
  if (state)
  {	
	LCD_WriteLeftPart(hours);
	state = false;
  }	
  else
  {	
	LCD_PortVal[0] &= mask[0];
    LCD_PortVal[1] &= mask[1];
    LCD_PortVal[2] &= mask[2];
	state = true;
  }
}

/**
  * @brief  Осуществляет мигание третьего и четвертого знакоместа
  */
void LCD_MinuteBlink(uint8_t minutes)
{
  /* Маска для очистки третьего и четвертого знакоместа */ 
  const uint16_t mask[3] = {0xFCFF, 0xFC0F, 0xFC0F};
  static bool state = true;
  
  if (state)
  {	
	LCD_WriteRightPart(minutes);
	state = false;
  }	
  else
  {	
	LCD_PortVal[0] &= mask[0];
    LCD_PortVal[1] &= mask[1];
    LCD_PortVal[2] &= mask[2];
	state = true;
  }
}

/**
  * @brief  Осуществляет мигание всех четырех знакоместа
  */
void LCD_YearBlink(uint16_t year)
{
  /* Маска для очистки всех четырех знакоместа */   
  const uint16_t mask[3] = {0xF8FE, 0xF000, 0xF000};
  uint8_t thousend, hundres, dozen, unit;  
  static bool state = true;
  
  if (state)
  {	
    /* Вычисляем разряды числа */
    thousend = year/1000;
    hundres = (year - thousend*1000)/100;
    dozen = (year - thousend*1000 - hundres*100)/10;
    unit = year - thousend*1000 - hundres*100 - dozen*10;
  
    /* Пишем символы чисел в буфер экрана */
    LCD_PortVal[0] |= sDigit_1[thousend].com1;
    LCD_PortVal[1] |= sDigit_1[thousend].com2;
    LCD_PortVal[2] |= sDigit_1[thousend].com3;
  
    LCD_PortVal[0] |= sDigit_2[hundres].com1;
    LCD_PortVal[1] |= sDigit_2[hundres].com2;
    LCD_PortVal[2] |= sDigit_2[hundres].com3;
  
    LCD_PortVal[0] |= sDigit_3[dozen].com1;
    LCD_PortVal[1] |= sDigit_3[dozen].com2;
    LCD_PortVal[2] |= sDigit_3[dozen].com3;
  
    LCD_PortVal[0] |= sDigit_4[unit].com1;
    LCD_PortVal[1] |= sDigit_4[unit].com2;
    LCD_PortVal[2] |= sDigit_4[unit].com3;
  
	state = false;
  }	
  else
  {	
	LCD_PortVal[0] &= mask[0];
    LCD_PortVal[1] &= mask[1];
    LCD_PortVal[2] &= mask[2];
	state = true;
  }
}

/**
  * @brief  Очищает 4 знакоместа
  */
void LCD_ClearDigits(void)
{
  /* Маска для очистки всех четырех знакоместа */   
  const uint16_t mask[3] = {0xF8FE, 0xF000, 0xF000};
  
  LCD_PortVal[0] &= mask[0];
  LCD_PortVal[1] &= mask[1];
  LCD_PortVal[2] &= mask[2];
}

/**
  * @brief  Вывод беззнакового целого от 0 до 9999
  */
void LCD_WriteInt(uint16_t value)
{
  uint8_t i, tmp;
  
  const uint16_t SegDigitMask[3] = {0xFF2D, 0xF000, 0xF000};
  
  LCD_PortVal[0] &= SegDigitMask[0];
  LCD_PortVal[1] &= SegDigitMask[1];
  LCD_PortVal[2] &= SegDigitMask[2];
 
  for(i = 0; i < 4; i++)
  {
    tmp = value % 10 + 0x30;

    if ((value > 0) || (i == 0))
      LCD_WriteChar(&tmp, 3 - i);

    value = value/10;
  } 

}

/**
  * @brief  This function writes a string in the LCD
  * @param str: string to dispaly in the LCD.
  * @retval : None.
  */
void LCD_WriteString(char* str)
{
  uint8_t i;
  const uint16_t SegDigitMask[3] = {0xFF2D, 0xF000, 0xF000};
  
  LCD_PortVal[0] &= SegDigitMask[0];
  LCD_PortVal[1] &= SegDigitMask[1];
  LCD_PortVal[2] &= SegDigitMask[2];
   
  for (i = 0; i < 4; i++)
  {
    LCD_WriteChar(str + i, i);
  }
} 


/**
  * @brief  
  * @retval
  */
void LCD_Init(void)
{
  /* Заполнение массива масок для отображения цифр на всех 4-ех знакоместах */
  /* 0 */
  sDigit_1[0].com1 = 0x400; sDigit_2[0].com1 = 0x1; sDigit_3[0].com1 = 0x200; sDigit_4[0].com1 = 0x100; 
  sDigit_1[0].com2 = 0x804; sDigit_2[0].com2 = 0xA; sDigit_3[0].com2 = 0x90;  sDigit_4[0].com2 = 0x60;
  sDigit_1[0].com3 = 0xC04; sDigit_2[0].com3 = 0xB; sDigit_3[0].com3 = 0x290; sDigit_4[0].com3 = 0x160;
  
  /* 1 */
  sDigit_1[1].com1 = 0x0; sDigit_2[1].com1 = 0x0; sDigit_3[1].com1 = 0x0;  sDigit_4[1].com1 = 0x0; 
  sDigit_1[1].com2 = 0x4; sDigit_2[1].com2 = 0x8; sDigit_3[1].com2 = 0x10; sDigit_4[1].com2 = 0x40;
  sDigit_1[1].com3 = 0x4; sDigit_2[1].com3 = 0x8; sDigit_3[1].com3 = 0x10; sDigit_4[1].com3 = 0x40;
  
  /* 2 */
  sDigit_1[2].com1 = 0x400; sDigit_2[2].com1 = 0x1; sDigit_3[2].com1 = 0x200; sDigit_4[2].com1 = 0x100; 
  sDigit_1[2].com2 = 0x404; sDigit_2[2].com2 = 0x9; sDigit_3[2].com2 = 0x210; sDigit_4[2].com2 = 0x140;
  sDigit_1[2].com3 = 0xC00; sDigit_2[2].com3 = 0x3; sDigit_3[2].com3 = 0x280; sDigit_4[2].com3 = 0x120;
  
  /* 3 */
  sDigit_1[3].com1 = 0x400; sDigit_2[3].com1 = 0x1; sDigit_3[3].com1 = 0x200; sDigit_4[3].com1 = 0x100; 
  sDigit_1[3].com2 = 0x404; sDigit_2[3].com2 = 0x9; sDigit_3[3].com2 = 0x210; sDigit_4[3].com2 = 0x140;
  sDigit_1[3].com3 = 0x404; sDigit_2[3].com3 = 0x9; sDigit_3[3].com3 = 0x210; sDigit_4[3].com3 = 0x140;
  
  /* 4 */
  sDigit_1[4].com1 = 0x0;   sDigit_2[4].com1 = 0x0; sDigit_3[4].com1 = 0x0;   sDigit_4[4].com1 = 0x0; 
  sDigit_1[4].com2 = 0xC04; sDigit_2[4].com2 = 0xB; sDigit_3[4].com2 = 0x290; sDigit_4[4].com2 = 0x160;
  sDigit_1[4].com3 = 0x4;   sDigit_2[4].com3 = 0x8; sDigit_3[4].com3 = 0x10;  sDigit_4[4].com3 = 0x40;
  
  /* 5 */
  sDigit_1[5].com1 = 0x400; sDigit_2[5].com1 = 0x1; sDigit_3[5].com1 = 0x200; sDigit_4[5].com1 = 0x100; 
  sDigit_1[5].com2 = 0xC00; sDigit_2[5].com2 = 0x3; sDigit_3[5].com2 = 0x280; sDigit_4[5].com2 = 0x120;
  sDigit_1[5].com3 = 0x404; sDigit_2[5].com3 = 0x9; sDigit_3[5].com3 = 0x210; sDigit_4[5].com3 = 0x140;
  
  /* 6 */
  sDigit_1[6].com1 = 0x400; sDigit_2[6].com1 = 0x1; sDigit_3[6].com1 = 0x200; sDigit_4[6].com1 = 0x100; 
  sDigit_1[6].com2 = 0xC00; sDigit_2[6].com2 = 0x3; sDigit_3[6].com2 = 0x280; sDigit_4[6].com2 = 0x120;
  sDigit_1[6].com3 = 0xC04; sDigit_2[6].com3 = 0xB; sDigit_3[6].com3 = 0x290; sDigit_4[6].com3 = 0x160;
  
  /* 7 */
  sDigit_1[7].com1 = 0x400; sDigit_2[7].com1 = 0x1; sDigit_3[7].com1 = 0x200; sDigit_4[7].com1 = 0x100; 
  sDigit_1[7].com2 = 0x4;   sDigit_2[7].com2 = 0x8; sDigit_3[7].com2 = 0x10;  sDigit_4[7].com2 = 0x40;
  sDigit_1[7].com3 = 0x4;   sDigit_2[7].com3 = 0x8; sDigit_3[7].com3 = 0x10;  sDigit_4[7].com3 = 0x40;
  
  /* 8 */
  sDigit_1[8].com1 = 0x400; sDigit_2[8].com1 = 0x1; sDigit_3[8].com1 = 0x200; sDigit_4[8].com1 = 0x100; 
  sDigit_1[8].com2 = 0xC04; sDigit_2[8].com2 = 0xB; sDigit_3[8].com2 = 0x290; sDigit_4[8].com2 = 0x160;
  sDigit_1[8].com3 = 0xC04; sDigit_2[8].com3 = 0xB; sDigit_3[8].com3 = 0x290; sDigit_4[8].com3 = 0x160;
  
  /* 9 */
  sDigit_1[9].com1 = 0x400; sDigit_2[9].com1 = 0x1; sDigit_3[9].com1 = 0x200; sDigit_4[9].com1 = 0x100; 
  sDigit_1[9].com2 = 0xC04; sDigit_2[9].com2 = 0xB; sDigit_3[9].com2 = 0x290; sDigit_4[9].com2 = 0x160;
  sDigit_1[9].com3 = 0x404; sDigit_2[9].com3 = 0x9; sDigit_3[9].com3 = 0x210; sDigit_4[9].com3 = 0x140;
  
  LCD_InitGpio();
  LCD_InitNVIC();
  LCD_InitTim();
}

void LCD_InitGpio(void)
{
  GPIO_InitTypeDef          GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOB, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
                                GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 |  
                                GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init( GPIOB, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init( GPIOB, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_15;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init( GPIOA, &GPIO_InitStructure);
  
  GPIO_SetBits(GPIOA, GPIO_Pin_15);
  
  /* Compute and load the GPIOs masks of the Common lines */
  LCD_ComsMasksInit();
  
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @retval
  */
void LCD_InitNVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  NVIC_InitStructure.NVIC_IRQChannel = LCD_TIM_IRQChannel; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LCD_Priority_Value;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


/**
  * @brief  computes the offset to add to the GPIO masks
  * @param Pin_offset: pin offset.
  * @retval : None.
  */
uint32_t LCD_OffsetValue(uint8_t Pin_offset)
{
  uint8_t i;
  uint32_t offset_Value = 1;

  for (i = 0; i < Pin_offset; i++)
    offset_Value =  offset_Value * 16;
    
  return (offset_Value - 1);
}

/**
  * @brief  initilizes the LCD GPIOs masks.
  * @param ne.
  * @retval : None.
  */
void LCD_ComsMasksInit(void)
{
  uint8_t i;
  
  for (i = 0; i < 4; i++)
  {
    CommonLine_VDD_2[i] = (CommonLine_VDD_2[i] << ((LCD_CommonLinesPinOffset) * 4))
                         + LCD_OffsetValue(CommonLines_Pin_Offset);
    CommonLine_OUT_PP[i] = CommonLine_OUT_PP[i] << ((LCD_CommonLinesPinOffset) * 4);
  }
}

/**
  * @brief  Настройка таймера для формирования сигналов на линиях COM
  * @retval 
  */
void LCD_InitTim(void)
{
  TIM_OCInitTypeDef         TIM_OCInitStructure;
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;  
    
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIMx, ENABLE);  // Тестовый ШИМ
  
  TIM_TimeBaseStructure.TIM_Prescaler = 24;
  TIM_TimeBaseStructure.TIM_Period = 3000;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(LCD_TIMER, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_Pulse = 0;   
  TIM_OCInitStructure.TIM_OutputState = ENABLE;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(LCD_TIMER, &TIM_OCInitStructure);
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_Pulse = LCD_DefaultContrast;
  TIM_OCInitStructure.TIM_OutputState = ENABLE;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC2Init(LCD_TIMER, &TIM_OCInitStructure);  
  
  TIM_ITConfig( LCD_TIMER, TIM_IT_CC1 | TIM_IT_CC2 , ENABLE);

  TIM_Cmd( LCD_TIMER, ENABLE);

}

void TIM4_IRQHandler(void)
{
    
  if (TIM_GetITStatus(LCD_TIMER, TIM_IT_CC1) == SET)
  {
    TIM_ClearITPendingBit(LCD_TIMER, TIM_IT_CC1);
    
    // All Segment lines = 0 ( all SegmentsLines_LQD_Port pins = 0 ) 
    LCD_SegmentsPort->ODR = (uint16_t)~ALL_SEGMENT_LINES;
    // All Common lines = 0 
    LCD_CommonLinesPort->ODR &= ~ALL_COMMON_LINES;
    // Configure all Common lines on CommonLines port pins as Out_PP 
    LCD_CommonLinesPort->CRH |= ALL_COMMON_LINES_PP;
    
  }
  
  else if (TIM_GetITStatus(LCD_TIMER, TIM_IT_CC2) == SET)
  {
    TIM_ClearITPendingBit(LCD_TIMER, TIM_IT_CC2);

    if (LCD_PhaseExecute == 0)       
    {
      LCD_SegmentsPort->ODR = LCD_PortVal[LCD_SegmentIndex];
      // Common_line[lcdcr] is set to low 
      LCD_CommonLinesPort->BRR = CommonLine[LCD_SegmentIndex];
      // Other Common lines set to Vdd/2 
      LCD_CommonLinesPort->CRH &= CommonLine_VDD_2[LCD_SegmentIndex];
      // Set Common_line[lcdcr] out push pull 
      LCD_CommonLinesPort->CRH |= CommonLine_OUT_PP[LCD_SegmentIndex];
       
      LCD_PhaseExecute++;
    }
    else 
    {  
      LCD_SegmentsPort->ODR = ~LCD_PortVal[LCD_SegmentIndex];
            
      // Common_line[lcdcr] is set to high 
      LCD_CommonLinesPort->BSRR = CommonLine[LCD_SegmentIndex];
     
      // Other Common lines set to Vdd/2 
      LCD_CommonLinesPort->CRH &= CommonLine_VDD_2[LCD_SegmentIndex];
     
      // Other Common lines out push pull 
      LCD_CommonLinesPort->CRH |= CommonLine_OUT_PP[LCD_SegmentIndex];
         
      LCD_SegmentIndex++;
      
      if (LCD_SegmentIndex > 3)
        LCD_SegmentIndex = 0;
     
      LCD_PhaseExecute = 0;
      
    }
    
  }
 
  
}

/******************************* (C) LiteMesh *********************************/