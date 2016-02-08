/******************************* (C) LiteMesh **********************************
 * @module  menu
 * @file    menu.c
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

#include "stm32f10x.h"
#include "menu.h"
#include "buttons.h"
#include "rtc_test.h"
#include "lcd.h"

/**
  * @brief  Флаг устанавливается когда нужно подать звуковой сигнал
  */
extern bool fBuzzerSignal;

MENU_STATE_t   MENU_STATE = MENU_IDLE;

SET_TIME_t     SET_TIME = SET_TIME_IDLE;

STOPWATCH_t    STOPWATCH = STOPWATCH_IDLE;

STOPWATCH_TIME_t sStopwatchTime = {0, 0};

TIMER_t        TIMER = TIMER_IDLE;

ALARM_t        ALARM_1 = ALARM_IDLE;

ALARM_t        ALARM_2 = ALARM_IDLE;

/**
  * @brief  Содержит дни когда должен сработать будильник
  *         0 - Вс, 1 - Пн...
  */
uint8_t Alarm1WeekDay[7] = {0, 0, 0, 0, 0, 0, 0};  // Содержит дни когда должен сработать будильник
uint8_t Alarm2WeekDay[7] = {0, 0, 0, 0, 0, 0, 0};  
uint8_t dayIndex = 0;      // Номер дня недели которым нужно мигать при установке будильника
float temperature;  // Показания датчика температуры

bool fTimerIsSet = false;

bool fStopWatch = false;

bool fAlarm1IsSet = false;
bool fAlarm2IsSet = false;
uint16_t counterAlarm1 = 0;
uint16_t counterAlarm2 = 0;
bool fAlarm1Load = false; // Устанавливается когда сработал таймер
bool fAlarm2Load = false; // Устанавливается когда сработал таймер

bool fTimeBlink = false;
bool fHourBlink = false;
bool fMinuteBlink = false;
bool fYearBlink = false;
bool fMonthBlink = false;
bool fDayBlink = false;
bool fHandBlink = false;
bool f1Blink = false;
bool f2Blink = false;
bool fWeekdayBlink = false;
bool fShowTemperature = false;

/**
  * @brief  Структура для хранения времени при настройке
  */
RTC_t sCurrentTime;

/**
  * @brief  Структура для настройки времени таймера
  */
RTC_t sTimerTime;

/**
  * @brief  Структура для хранения времени для будильника 1
  */
RTC_t sAlarm1;

/**
  * @brief  Структура для хранения времени для будильника 2
  */
RTC_t sAlarm2;

/**
  * @brief  Отрабатывает нажатия на кнопки
  * @retval 
  */
void MENU_MainHandler(BUTTON_FUNC_t button)
{
  switch(MENU_STATE)
  {
	case MENU_IDLE:
	  
	  if (button == BUTTON_ENTER)
	  {	
		MENU_STATE = MENU_SET_TIME;
	    MENU_SetTime(button);
	  }	
	break;
	
	case MENU_SET_TIME:
	  
	  if (button == BUTTON_RETURN)
		MENU_TimeReturn();
	  else 
	    MENU_SetTime(button);

	break;
	
	case MENU_STOPWATCH:
	  
	  if (button == BUTTON_RETURN)
	  {	
		MENU_STATE = MENU_IDLE;
		
		STOPWATCH = STOPWATCH_IDLE;
		LCD_TimeSimbol(false);  // Выключаем символ часов
		fStopWatch = false;     // Останавливаем секундомер
		sStopwatchTime.min = 0;
		sStopwatchTime.sec = 0;
	  }
	  else	  
	    MENU_Stopwatch(button);

	break;

    case MENU_TIMER :
	  
	  if (button == BUTTON_RETURN)
	  {
		MENU_STATE = MENU_IDLE;
		TIMER = TIMER_IDLE;
		
		fHourBlink = false;  // Отключаем мигание часов
		fMinuteBlink = false;
		fHandBlink = false;
		LCD_HandSimbol(false);
	  }
	  else
		MENU_Timer(button);
	  
	break;
	
    case MENU_ALARM_1 :
      MENU_Alarm_1(button);
	break;
	
	case MENU_ALARM_2 :
      MENU_Alarm_2(button);
	break;
	 
    default :
	break;
  }
  
  /* Если звучит сигнал будильника - отключаем при нажатии любой кнопки */
  if (fAlarm1Load)
  {
	fAlarm1Load = false;
	fBuzzerSignal = false;
	counterAlarm1 = 0;
  }
  if (fAlarm2Load)
  {
	fAlarm2Load = false;
	fBuzzerSignal = false;
	counterAlarm2 = 0;
  }
}

/**
  * @brief  Установка времени и даты
  */
void MENU_SetTime(BUTTON_FUNC_t button)
{
  switch (SET_TIME)
  {
    case SET_TIME_IDLE :
	  
	  //if (button == BUTTON_ENTER)
	  //{	
		MENU_SaveCurrentTime(); // Сохраняем текущее время
		fTimeBlink = true;      // Мигаем временем
		LCD_Colon(true);        // Выводим символ двоеточия
		SET_TIME = SET_HOUR;    
	  //}	
	break;
	
	case SET_HOUR :
	  
	  if (button == BUTTON_ENTER)
	  {
		fTimeBlink = false; // Отключаем мигание времени
		fHourBlink = true;  // Включаем мигание часов
		LCD_WriteRightPart(sCurrentTime.min);  // Отображаем минуты
		SET_TIME = CHANGE_HOUR;
	  }
	  /* Переход в стейт меню - установка секундомера */
	  else if (button == BUTTON_UP)
	  {
	    fTimeBlink = false;
		SET_TIME = SET_TIME_IDLE;		
	    MENU_STATE = MENU_STOPWATCH;
		MENU_Stopwatch(button);
	  }
	  /* Переход в стейт меню - установка Будильника 2 */
	  else if (button == BUTTON_DOWN)
	  {
	    fTimeBlink = false;
		SET_TIME = SET_TIME_IDLE;		
	    MENU_STATE = MENU_ALARM_2;
		MENU_Alarm_2(button);
	  }
	break;
	
	case CHANGE_HOUR :
	  
	  if (button == BUTTON_ENTER)
	  {
		fHourBlink = false;  // Отключаем мигание часов
		LCD_WriteLeftPart(sCurrentTime.hour); // Отображаем часы
		fMinuteBlink = true; // Включаем мигание минут
		SET_TIME = CHANGE_MIN;
	  }
	  else if (button == BUTTON_UP)
	    MENU_ChangeHours(button);
	  else if (button == BUTTON_DOWN)
	    MENU_ChangeHours(button);
	  
	break;
	
	case CHANGE_MIN :
	
	  if (button == BUTTON_ENTER)
	  {
		fMinuteBlink = false;  // Отключаем мигание минут
		LCD_Colon(false);      // Убираем символ двоеточия
		LCD_WriteYear(sCurrentTime.year);  // Отображаем год
		fYearBlink = true;     // Включаем мигание года
		SET_TIME = CHANGE_YEAR;
	  }
	  else if (button == BUTTON_UP)
	    MENU_ChangeMinutes(button);
	  else if (button == BUTTON_DOWN)
	    MENU_ChangeMinutes(button);
	break;
	
    case CHANGE_YEAR :
	  
	  if (button == BUTTON_ENTER)
	  {
		fYearBlink = false;  // Отключаем мигание года
		fMonthBlink = true;  // Включаем мигание месяца
		LCD_ClearDigits();   // Очищаем знакоместа 
		LCD_WriteRightPart(sCurrentTime.month);  // Отображаем месяц
		SET_TIME = CHANGE_MONTH;
	  }
	  else if (button == BUTTON_UP)
	    MENU_ChangeYear(button);
	  else if (button == BUTTON_DOWN)
	    MENU_ChangeYear(button);
	break;
	
	case CHANGE_MONTH :
	  
	  if (button == BUTTON_ENTER)
	  {
		fMonthBlink = false;  // Отключаем мигание месяца
		fDayBlink = true;     // Включаем мигание дня
		LCD_WriteRightPart(sCurrentTime.mday);  // Отображаем день месяца
		SET_TIME = CHANGE_DAY;
	  }
	  else if (button == BUTTON_UP)
	    MENU_ChangeMonth(button);
	  else if (button == BUTTON_DOWN)
	    MENU_ChangeMonth(button);
	break;
	
	case CHANGE_DAY :
	  
	  if (button == BUTTON_ENTER)
	  {
		fDayBlink = false;  // Отключаем мигание дня
		rtc_settime(&sCurrentTime); // Устанавливаем новое время
		
		rtc_gettime(&sCurrentTime);
	    LCD_WriteLeftPart(sCurrentTime.hour);
        LCD_WriteRightPart(sCurrentTime.min);
        LCD_WriteWeekDay(sCurrentTime.wday);
		
		MENU_STATE = MENU_IDLE;
		SET_TIME = SET_TIME_IDLE;
	  }
	  else if (button == BUTTON_UP)
	    MENU_ChangeDay(button);
	  else if (button == BUTTON_DOWN)
	    MENU_ChangeDay(button);
	break;
	  
    default :
	break;
  }
}

/**
  * @brief  Секундомер
  */
void MENU_Stopwatch(BUTTON_FUNC_t button)
{
  switch (STOPWATCH)
  {
    case STOPWATCH_IDLE :
	  
	    LCD_WriteLeftPart(0);   // Выводим 00:00
        LCD_WriteRightPart(0);
	    LCD_Colon(true);        // Выводим символ двоеточия
		LCD_TimeSimbol(true);   // Выводим символ часов
		STOPWATCH = STOPWATCH_GO;
	break;
	
    case STOPWATCH_GO :
	  
	  if (button == BUTTON_ENTER)
	  {	 
	    if (fStopWatch == false)
		  fStopWatch = true;  // Запускаем секундомер
	    else
		{  
		  fStopWatch = false; // Останавливаем секундомер
		  STOPWATCH = STOPWATCH_STOP;
		}  
	  }
	  /* Переход в стейт меню - таймер */
	  else if (button == BUTTON_UP)
	  {
		if (fStopWatch == false)
		{  
	      STOPWATCH = STOPWATCH_IDLE;
		  LCD_TimeSimbol(false);  // Выключаем символ часов
		  fStopWatch = false;     // Останавливаем секундомер
		  sStopwatchTime.min = 0;
		  sStopwatchTime.sec = 0;
		  
		  MENU_STATE = MENU_TIMER;
		  TIMER = TIMER_IDLE;
		  MENU_Timer(button);
		}  
	  }
	  /* Переход в стейт меню - установка времени */
	  else if (button == BUTTON_DOWN)
	  {
		if (fStopWatch == false)
		{  
	      STOPWATCH = STOPWATCH_IDLE;
		  LCD_TimeSimbol(false);  // Выключаем символ часов
		  fStopWatch = false;     // Останавливаем секундомер
		  sStopwatchTime.min = 0;
		  sStopwatchTime.sec = 0;
		  
		  MENU_STATE = MENU_SET_TIME;
		  SET_TIME = SET_TIME_IDLE;
		  MENU_SetTime(button);
		}  
	  }
	  
	break;  
	
	case STOPWATCH_STOP :
	
	  if (button == BUTTON_ENTER)
	  {
	    LCD_WriteLeftPart(0);    // Выводим 00:00
        LCD_WriteRightPart(0);
		sStopwatchTime.min = 0;  // Обнуляем секундомер
 		sStopwatchTime.sec = 0;
		STOPWATCH = STOPWATCH_GO;
	  }
	break;
  }
}

/**
  * @brief  Установка таймера
  */
void MENU_Timer(BUTTON_FUNC_t button)
{
  switch (TIMER)
  {
    case TIMER_IDLE :
	  
	  fHandBlink = true;      // Мигаем символом "Рука"
	  MENU_SaveCurrentTime(); // Сохраняем текущее время для редактирования
	  LCD_WriteLeftPart(sCurrentTime.hour); // Выводим время для последующего редактирования
      LCD_WriteRightPart(sCurrentTime.min);
	  LCD_Colon(true);        // Выводим символ двоеточия
      TIMER = TIMER_SET_HOUR;	  
	  
    break;
	
    case TIMER_SET_HOUR :
	  
	  if (button == BUTTON_ENTER)
	  {
		/* Если таймер уже установлен нужно его отключить */
		if (fTimerIsSet)
		{
		  fTimerIsSet = false;
		  fBuzzerSignal = false;
		  fHandBlink = false;
		  LCD_HandSimbol(false);
		  TIMER = TIMER_IDLE;
		  MENU_STATE = MENU_IDLE;
		}
		else
		{  
		  fHourBlink = true;  
		  LCD_WriteRightPart(sCurrentTime.min);  // Отображаем минуты
		  TIMER = TIMER_CHANGE_HOUR;
		}  
	  }
	  /* Переход в стейт меню - установка будильника 1 */
	  else if (button == BUTTON_UP)
	  {
	    fHandBlink = false;
		LCD_HandSimbol(false);
		TIMER = TIMER_IDLE;		
		
	    MENU_STATE = MENU_ALARM_1;
		ALARM_1 = ALARM_IDLE;
		MENU_Alarm_1(button);
	  }
	  /* Переход в стейт меню - Секундомер */
	  else if (button == BUTTON_DOWN)
	  {
	    fHandBlink = false;
		LCD_HandSimbol(false);
		TIMER = TIMER_IDLE;		
		
	    MENU_STATE = MENU_STOPWATCH;
		STOPWATCH = STOPWATCH_IDLE;
		MENU_Stopwatch(button);
	  }
	  
	break;
	
	case TIMER_CHANGE_HOUR :
	
	  if (button == BUTTON_ENTER)
	  {
		fHourBlink = false;  // Отключаем мигание часов
		LCD_WriteLeftPart(sCurrentTime.hour); // Отображаем часы
		fMinuteBlink = true; // Включаем мигание минут
		TIMER = TIMER_CHANGE_MIN;
	  }
	  else if (button == BUTTON_UP)
	  {
	    MENU_ChangeHours(button);
	  }
	  else if (button == BUTTON_DOWN)
	  {
	    MENU_ChangeHours(button);
	  }
	  
	break;
	
	case TIMER_CHANGE_MIN :
	  
	  if (button == BUTTON_ENTER)
	  {
		fMinuteBlink = false;  // Отключаем мигание минут
		MENU_TimerSetTime();   // Устанавилваем время срабатывания и активируем таймер
		TIMER = TIMER_IDLE;
		MENU_STATE = MENU_IDLE;
	  }
	  else if (button == BUTTON_UP)
	  {
	    MENU_ChangeMinutes(button);
	  }
	  else if (button == BUTTON_DOWN)
	  {
	    MENU_ChangeMinutes(button);
	  }
	  
	break;
  }
}

void MENU_Alarm_1(BUTTON_FUNC_t button)
{
  switch (ALARM_1)
  {
    case ALARM_IDLE :
	  
	  f1Blink = true;         // Мигаем символом "1"
	  MENU_SaveCurrentTime(); // Сохраняем текущее время для редактирования
	  LCD_WriteLeftPart(sCurrentTime.hour); // Выводим время для последующего редактирования
      LCD_WriteRightPart(sCurrentTime.min);
	  LCD_Colon(true);        // Выводим символ двоеточия
      ALARM_1 = ALARM_SET_HOUR;	  
	  
    break;
	
    case ALARM_SET_HOUR :
	  
	  if (button == BUTTON_ENTER)
	  {
		/* Если будильник уже установлен нужно его отключить */
		if (fAlarm1IsSet)
		{
		  fAlarm1IsSet = false;
		  fBuzzerSignal = false;
		  f1Blink = false;
		  LCD_1_Simbol(false);
		  ALARM_1 = ALARM_IDLE;
		  MENU_STATE = MENU_IDLE;
		}
		else
		{  
		  LCD_WriteWeekDays(Alarm1WeekDay);  // Выводим те дни недели на который установлен будильник 
		  fAlarm1IsSet = true;
		  fHourBlink = true;  // Включаем мигание часов
		  LCD_WriteRightPart(sCurrentTime.min);  // Отображаем минуты
		  /* Переходим в стейт корректировки часов */
		  ALARM_1 = ALARM_CHANGE_HOUR;
		}  
	  }
	  /* Переход в стейт меню - установка будильника 2 */
	  else if (button == BUTTON_UP)
	  {
	    f1Blink = false;
		LCD_1_Simbol(false);
		ALARM_1 = ALARM_IDLE;		
		
	    MENU_STATE = MENU_ALARM_2;
		ALARM_2 = ALARM_IDLE;
		MENU_Alarm_2(button);
	  }
	  /* Переход в стейт меню - Таймер */
	  else if (button == BUTTON_DOWN)
	  {
	    f1Blink = false;
		LCD_1_Simbol(false);
		ALARM_1 = ALARM_IDLE;	
		
		MENU_STATE = MENU_TIMER;
		TIMER = TIMER_IDLE;
		MENU_Timer(button);
	  }
	  else if (button == BUTTON_RETURN)
		MENU_AlarmReturn();
		  
	break;
	
	case ALARM_CHANGE_HOUR :
	
	  if (button == BUTTON_ENTER)
	  {
		fHourBlink = false;  // Отключаем мигание часов
		LCD_WriteLeftPart(sCurrentTime.hour); // Отображаем часы
		fMinuteBlink = true; // Включаем мигание минут
		ALARM_1 = ALARM_CHANGE_MIN;
	  }
	  else if (button == BUTTON_UP)
	    MENU_ChangeHours(button);
	  else if (button == BUTTON_DOWN)
	    MENU_ChangeHours(button);
	  else if (button == BUTTON_RETURN)
		MENU_AlarmReturn();
	  
	break;
	
	case ALARM_CHANGE_MIN :
	  
	  if (button == BUTTON_ENTER)
	  {
		fMinuteBlink = false;  // Отключаем мигание минут
		LCD_WriteRightPart(sCurrentTime.min);  // Отображаем минуты
		ALARM_1 = ALARM_CHANGE_WEEKDAY;
		fWeekdayBlink = true;
		LCD_WriteWeekDays(Alarm1WeekDay);  // Нужно вывести те дни недели когда установлен будильник
		dayIndex = 1;  // Начать мигать символом "Пн"
	  }
	  else if (button == BUTTON_UP)
	    MENU_ChangeMinutes(button);
	  else if (button == BUTTON_DOWN)
	    MENU_ChangeMinutes(button);
	  else if (button == BUTTON_RETURN)
		MENU_AlarmReturn();
	  
	break;
	
    case ALARM_CHANGE_WEEKDAY :
	
	  if (button == BUTTON_ENTER)
	  {
		if (Alarm1WeekDay[dayIndex])
		{  
		  Alarm1WeekDay[dayIndex] = 0;
		  LCD_WeekDay(dayIndex, false);
		}  
		else
		{  
		  Alarm1WeekDay[dayIndex] = 1;
		  LCD_WeekDay(dayIndex, true); 
		}
		
		if (dayIndex == 0)
		  dayIndex++;
		else if (dayIndex++ == 6)
		  dayIndex = 0;
	  }
	  else if (button == BUTTON_UP)
	  {
		/* Нужно погасить текущей день если он не установлен */
		if (!Alarm1WeekDay[dayIndex])
		  LCD_WeekDay(dayIndex, false);
		else
		  LCD_WeekDay(dayIndex, true);
		  
		if (dayIndex++ == 6)
		  dayIndex = 0;
	  }
	  else if (button == BUTTON_DOWN)
	  {
		/* Нужно погасить текущей день если он не установлен */
		if (!Alarm1WeekDay[dayIndex])
		  LCD_WeekDay(dayIndex, false);
        else
		  LCD_WeekDay(dayIndex, true);
		
		if (dayIndex-- == 0)
		  dayIndex = 6;
	  }
	  else if (button == BUTTON_RETURN)
	  {
        /* Сохраняем установленное время и дни недели */
		sAlarm1.hour = sCurrentTime.hour;
		sAlarm1.min = sCurrentTime.min;
		fAlarm1IsSet = true;
		MENU_AlarmReturn();
	  }		
	  
	break;
  }
}

void MENU_Alarm_2(BUTTON_FUNC_t button)
{
  switch (ALARM_2)
  {
    case ALARM_IDLE :
	  
	  f2Blink = true;         // Мигаем символом "2"
	  MENU_SaveCurrentTime(); // Сохраняем текущее время для редактирования
	  LCD_WriteLeftPart(sCurrentTime.hour); // Выводим время для последующего редактирования
      LCD_WriteRightPart(sCurrentTime.min);
	  LCD_Colon(true);        // Выводим символ двоеточия
      ALARM_2 = ALARM_SET_HOUR;	  
	  
    break;
	
    case ALARM_SET_HOUR :
	  
	  if (button == BUTTON_ENTER)
	  {
		/* Если таймер уже установлен нужно его отключить */
		if (fAlarm2IsSet)
		{
		  fAlarm2IsSet = false;
		  fBuzzerSignal = false;
		  f2Blink = false;
		  LCD_2_Simbol(false);
		  ALARM_2 = ALARM_IDLE;
		  MENU_STATE = MENU_IDLE;
		}
		else
		{  
		  LCD_WriteWeekDays(Alarm2WeekDay);  // Выводим те дни недели на который установлен будильник 
		  fAlarm2IsSet = true;
		  fHourBlink = true;  // Включаем мигание часов
		  LCD_WriteRightPart(sCurrentTime.min);  // Отображаем минуты
		  /* Переходим в стейт корректировки часов */
		  ALARM_2 = ALARM_CHANGE_HOUR;
		}  
	  }
	  /* Переход в стейт меню - установка будильника 2 */
	  else if (button == BUTTON_UP)
	  {
	    f2Blink = false;
		LCD_2_Simbol(false);
		ALARM_2 = ALARM_IDLE;		
		
	    MENU_STATE = MENU_SET_TIME;
		SET_TIME = SET_TIME_IDLE;
		MENU_SetTime(button);
	  }
	  /* Переход в стейт меню - установка будильника 1 */
	  else if (button == BUTTON_DOWN)
	  {
	    f2Blink = false;
		LCD_2_Simbol(false);
		ALARM_2 = ALARM_IDLE;	
		
		MENU_STATE = MENU_ALARM_1;
		ALARM_1 = ALARM_IDLE;
		MENU_Alarm_1(button);
	  }
	  else if (button == BUTTON_RETURN)
		MENU_AlarmReturn();
		  
	break;
	
	case ALARM_CHANGE_HOUR :
	
	  if (button == BUTTON_ENTER)
	  {
		fHourBlink = false;  // Отключаем мигание часов
		LCD_WriteLeftPart(sCurrentTime.hour); // Отображаем часы
		fMinuteBlink = true; // Включаем мигание минут
		ALARM_2 = ALARM_CHANGE_MIN;
	  }
	  else if (button == BUTTON_UP)
	    MENU_ChangeHours(button);
	  else if (button == BUTTON_DOWN)
	    MENU_ChangeHours(button);
	  else if (button == BUTTON_RETURN)
		MENU_AlarmReturn();
	  
	break;
	
	case ALARM_CHANGE_MIN :
	  
	  if (button == BUTTON_ENTER)
	  {
		fMinuteBlink = false;  // Отключаем мигание минут
		LCD_WriteRightPart(sCurrentTime.min);  // Отображаем минуты
		ALARM_2 = ALARM_CHANGE_WEEKDAY;
		fWeekdayBlink = true;
		LCD_WriteWeekDays(Alarm2WeekDay);  // Нужно вывести те дни недели когда установлен будильник
		dayIndex = 1;  // Начать мигать символом "Пн"
	  }
	  else if (button == BUTTON_UP)
	    MENU_ChangeMinutes(button);
	  else if (button == BUTTON_DOWN)
	    MENU_ChangeMinutes(button);
	  else if (button == BUTTON_RETURN)
		MENU_AlarmReturn();
	  
	break;
	
    case ALARM_CHANGE_WEEKDAY :
	
	  if (button == BUTTON_ENTER)
	  {
		if (Alarm2WeekDay[dayIndex])
		{  
		  Alarm2WeekDay[dayIndex] = 0;
		  LCD_WeekDay(dayIndex, false);
		}  
		else
		{  
		  Alarm2WeekDay[dayIndex] = 1;
		  LCD_WeekDay(dayIndex, true); 
		}
		
		if (dayIndex == 0)
		  dayIndex++;
		else if (dayIndex++ == 6)
		  dayIndex = 0;
	  }
	  else if (button == BUTTON_UP)
	  {
		/* Нужно погасить текущей день если он не установлен */
		if (!Alarm2WeekDay[dayIndex])
		  LCD_WeekDay(dayIndex, false);
		else
		  LCD_WeekDay(dayIndex, true);
		  
		if (dayIndex++ == 6)
		  dayIndex = 0;
	  }
	  else if (button == BUTTON_DOWN)
	  {
		/* Нужно погасить текущей день если он не установлен */
		if (!Alarm2WeekDay[dayIndex])
		  LCD_WeekDay(dayIndex, false);
        else
		  LCD_WeekDay(dayIndex, true);
		
		if (dayIndex-- == 0)
		  dayIndex = 6;
	  }
	  else if (button == BUTTON_RETURN)
	  {
        /* Сохраняем установленное время и дни недели */
		sAlarm2.hour = sCurrentTime.hour;
		sAlarm2.min = sCurrentTime.min;
		fAlarm2IsSet = true;
		MENU_AlarmReturn();
	  }		
	  
	break;
  }
}

/**
  * @brief  Выход из режима установки времени
  */
void MENU_TimeReturn(void)
{
  fTimeBlink = false;
  fHourBlink = false;
  fMinuteBlink = false;
  fYearBlink = false;
  fMonthBlink = false;
  fDayBlink = false;
	  
  MENU_STATE = MENU_IDLE;
  SET_TIME = SET_TIME_IDLE;
}

/**
  * @brief  Выход из режима установки будильника
  */
// TODO Проверить. Если не устновлено ни одного дня недели отключать Alarm
void MENU_AlarmReturn(void)
{
  bool alarm1 = false;
  bool alarm2 = false;
    
  /* Если не установлено ни одного дня недели отключать Alarm */
  for (uint8_t i = 0; i < 7; i++)
  {
    if (Alarm1WeekDay[i])
	  alarm1 = true;
	if (Alarm2WeekDay[i])
	  alarm2 = true;
  }
  
  if (!alarm1) {
	LCD_1_Simbol(false);
	fAlarm1IsSet = false;
  }	
  if (!alarm2) {
	LCD_2_Simbol(false);
	fAlarm2IsSet = false;
  }
	
  fHourBlink = false;
  fMinuteBlink = false;
  fWeekdayBlink = false;
  f1Blink = false;
  f2Blink = false;
  
  MENU_STATE = MENU_IDLE;
  ALARM_1 = ALARM_IDLE;
  ALARM_2 = ALARM_IDLE;	
}

/**
  * @brief  Сохраняет текущее время в sCurrentTime
  */
void MENU_SaveCurrentTime(void)
{
  rtc_gettime(&sCurrentTime);
}

/**
  * @brief  Устанавливаем время срабатывания и активируем таймер
  */
void MENU_TimerSetTime(void)
{
  /* Проверяем изминили ли время */
  //if ((sCurrentTime.hour != sTimerTime.hour) && (sCurrentTime.min != sTimerTime.min))
  
  /* Устанавливаем время срабатывания таймера */
  sTimerTime.hour = sCurrentTime.hour;
  sTimerTime.min = sCurrentTime.min;
	
  /* Отключаем мигание символа "Рука" */
  fHandBlink = false;
  LCD_HandSimbol(true);
	
  /* Устанавливаем флаг - "Таймер взведен" */
  fTimerIsSet = true;
}

/**
  * @brief  Корректировка часов
  */
void MENU_ChangeHours(BUTTON_FUNC_t button)
{
  if (button == BUTTON_UP)
  {
    if (sCurrentTime.hour == 23)
	  sCurrentTime.hour = 0;
	else 
	  sCurrentTime.hour++;
  }
  else if (button == BUTTON_DOWN)
  {
    if (sCurrentTime.hour == 0)
	  sCurrentTime.hour = 23;
	else 
	  sCurrentTime.hour--;
  }
}

/**
  * @brief  Корректировка минут
  */
void MENU_ChangeMinutes(BUTTON_FUNC_t button)
{
  if (button == BUTTON_UP)
  {
    if (sCurrentTime.min == 59)
	  sCurrentTime.min = 0;
	else 
	  sCurrentTime.min++;
  }
  else if (button == BUTTON_DOWN)
  {
    if (sCurrentTime.min == 0)
	  sCurrentTime.min = 59;
	else 
	  sCurrentTime.min--;
  }
}

/**
  * @brief  Корректировка года
  */
void MENU_ChangeYear(BUTTON_FUNC_t button)
{
  if (button == BUTTON_UP)
  {
    if (sCurrentTime.year == 4095)
	  sCurrentTime.year = 1;
	else 
	  sCurrentTime.year++;
  }
  else if (button == BUTTON_DOWN)
  {
    if (sCurrentTime.year == 1)
	  sCurrentTime.year = 4095;
	else 
	  sCurrentTime.year--;
  }
}

/**
  * @brief  Корректировка месяца
  */
void MENU_ChangeMonth(BUTTON_FUNC_t button)
{
  if (button == BUTTON_UP)
  {
    if (sCurrentTime.month == 12)
	  sCurrentTime.month = 1;
	else 
	  sCurrentTime.month++;
  }
  else if (button == BUTTON_DOWN)
  {
    if (sCurrentTime.month == 1)
	  sCurrentTime.month = 12;
	else 
	  sCurrentTime.month--;
  }
}

/**
  * @brief  Корректировка дня
  */
void MENU_ChangeDay(BUTTON_FUNC_t button)
{
  if (button == BUTTON_UP)
  {
    if (sCurrentTime.mday == 31)
	  sCurrentTime.mday = 1;
	else 
	  sCurrentTime.mday++;
  }
  else if (button == BUTTON_DOWN)
  {
    if (sCurrentTime.mday == 1)
	  sCurrentTime.mday = 31;
	else 
	  sCurrentTime.mday--;
  }
}

/**
  * @brief  Отсчет и вывод времени секундомера
  */
void MENU_WriteStopwatch(void)
{
  if (sStopwatchTime.sec == 59)
  {
	sStopwatchTime.sec = 0;
	sStopwatchTime.min++;
  }
  
  if (sStopwatchTime.min == 99)
    sStopwatchTime.min = 0;
  
  sStopwatchTime.sec++;
  
  LCD_WriteLeftPart(sStopwatchTime.min);
  LCD_WriteRightPart(sStopwatchTime.sec);
}

/**
  * @brief  Вызывается 2 раза в секунду. Отвечает за мигание различных символов.
  */
void MENU_Blink(void)
{
  if (MENU_STATE != MENU_IDLE)
  {
    if (fTimeBlink)
	  LCD_TimeBlink(sCurrentTime.hour, sCurrentTime.min);
	if (fHourBlink)
	  LCD_HourBlink(sCurrentTime.hour);
	if (fMinuteBlink)
	  LCD_MinuteBlink(sCurrentTime.min);
	if (fYearBlink)
	  LCD_YearBlink(sCurrentTime.year);
	if (fMonthBlink)
	  LCD_MinuteBlink(sCurrentTime.month);
	if (fDayBlink)
	  LCD_MinuteBlink(sCurrentTime.mday);
	if (fHandBlink)
	  LCD_HandBlink();
	if (f1Blink)
	  LCD_1_Blink();
	if (f2Blink)
	  LCD_2_Blink();
	if (fWeekdayBlink)
	  LCD_WeekdayBlink(dayIndex);
  }
}

/**
  * @brief  Таймеры и будильники
  */
void MENU_Timers(void)
{
  RTC_t time;
  
  static bool fTimerLoad = false;  // Устанавливается когда сработал таймер
  static uint16_t counterTimeout = 0;
  
  static bool fAlarm1Signal = false;
  static bool fAlarm2Signal = false;
  
  
  rtc_gettime(&time);
  
  /* Установлен таймер */
  if (fTimerIsSet)
  {
    if ((sTimerTime.hour == time.hour) && (sTimerTime.min == time.min))
	  fTimerLoad = true;
	
	if (fTimerLoad)
	{
	  counterTimeout++;
	  fBuzzerSignal = true;
	  	  
	  /* Сработал таймаут звучания сигнала, нужно отключить таймер */
	  if (counterTimeout > TIMER_TIMEOUT_SECOND)
	  {
	    fBuzzerSignal = false;
		counterTimeout = 0;
		fTimerLoad = false;
		fTimerIsSet = 0;
		LCD_HandSimbol(false);
	  }
	}
  }
  
  /* Установлен первый будильник */
  if (fAlarm1IsSet) 
  {
    if ((sAlarm1.hour == time.hour) && (sAlarm1.min == time.min) && (Alarm1WeekDay[time.wday]))
	{  
	  if (fAlarm1Signal == false)
	  {	
		fAlarm1Signal = true;
	    fAlarm1Load = true;
	  }
	}
	else
	  fAlarm1Signal = false;
	  
	if (fAlarm1Load) 
	{
	  counterAlarm1++;
	  fBuzzerSignal = true;
	  	  
	  /* Сработал таймаут звучания сигнала, нужно отключить таймер */
	  if (counterAlarm1 > ALARM_TIMEOUT_SECOND)
	  {
	    fBuzzerSignal = false;
		counterAlarm1 = 0;
		fAlarm1Load = false;
		
	  }
	}
  }
  
  /* Установлен второй будильник */
  if (fAlarm2IsSet) 
  {
    if ((sAlarm2.hour == time.hour) && (sAlarm2.min == time.min) && (Alarm2WeekDay[time.wday]))
	{  
	  if (fAlarm2Signal == false)
	  {	
		fAlarm2Signal = true;
	    fAlarm2Load = true;
	  }
	}
	else
	  fAlarm2Signal = false;
	  
	if (fAlarm2Load) 
	{
	  counterAlarm2++;
	  fBuzzerSignal = true;
	  	  
	  /* Сработал таймаут звучания сигнала, нужно отключить таймер */
	  if (counterAlarm2 > ALARM_TIMEOUT_SECOND)
	  {
	    fBuzzerSignal = false;
		counterAlarm2 = 0;
		fAlarm2Load = false;
		
	  }
	}
  }
  
}

/**
  * @brief  This function handles RTC global interrupt request.
  * @retval None
  */
void RTC_IRQHandler(void)
{
  RTC_t time;
  static uint8_t temperatureCounter = 0;
    
  if (RTC_GetITStatus(RTC_IT_SEC) != RESET)
  {
    /* Clear the RTC Second interrupt */
    RTC_ClearITPendingBit(RTC_IT_SEC);
	
	if (fShowTemperature)
	{  
	  LCD_Temperature(temperature);
	  LCD_Colon(false);
	  if (temperatureCounter++ > 2) {
	    fShowTemperature = false;
		temperatureCounter = 0;
		LCD_ThermometerSimbol(false);
		LCD_DotSimbol(false);
		
		rtc_gettime(&time);
	    LCD_WriteLeftPart(time.hour);
        LCD_WriteRightPart(time.min);
        LCD_WriteWeekDay(time.wday);
		LCD_Colon(true);
	  }	
	}	
	else  
	{  
	  if (MENU_STATE == MENU_IDLE)
	  {
	    LCD_ColonToggle();
	  
	    rtc_gettime(&time);
	    LCD_WriteLeftPart(time.hour);
        LCD_WriteRightPart(time.min);
        LCD_WriteWeekDay(time.wday);
	  
	    if (fAlarm1IsSet)
	      LCD_1_Simbol(true);
	  
	    if (fAlarm2IsSet)
	      LCD_2_Simbol(true);
	  }  
	  else if (fStopWatch)
	    MENU_WriteStopwatch();
	}
	
	
  }
}  

/******************************* (C) LiteMesh *********************************/