/******************************* (C) LiteMesh **********************************
 * @module  menu
 * @file    menu.h
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef MENU_H
#define MENU_H

#include <stdbool.h>

/**
  * @brief   Число секунд, которое звучит сигнал таймера
  */
#define TIMER_TIMEOUT_SECOND   3

/**
  * @brief   Число секунд, которое звучит сигнал будильника
  */
#define ALARM_TIMEOUT_SECOND   3

/**
  * @brief   Menu state
  */
typedef enum 
{  
  MENU_IDLE = 0,
  MENU_SET_TIME,
  MENU_STOPWATCH,
  MENU_TIMER,
  MENU_ALARM_1,
  MENU_ALARM_2,
    
}MENU_STATE_t;

/**
  * @brief   
  */
typedef enum 
{  
  SET_TIME_IDLE = 0,
  SET_HOUR,
  CHANGE_HOUR,
  CHANGE_MIN,
  CHANGE_YEAR,
  CHANGE_MONTH,
  CHANGE_DAY,  
  
} SET_TIME_t;

/**
  * @brief   
  */
typedef enum 
{ 
  STOPWATCH_IDLE,
  STOPWATCH_GO,
  STOPWATCH_STOP,
  
} STOPWATCH_t;

/**
  * @brief   
  */
typedef struct
{
  uint8_t min;
  uint8_t sec;
  
} STOPWATCH_TIME_t;

typedef enum
{
  TIMER_IDLE = 0,
  TIMER_SET_HOUR,
  TIMER_CHANGE_HOUR,
  TIMER_CHANGE_MIN,
  
} TIMER_t;

typedef enum
{
  ALARM_IDLE = 0,
  ALARM_SET,
  ALARM_SET_HOUR,
  ALARM_CHANGE_HOUR,
  ALARM_CHANGE_MIN,
  ALARM_CHANGE_WEEKDAY,
  
} ALARM_t;

/**
* @brief   Назначения (типы) кнопок
  */
typedef enum 
{  
  BUTTON_ENTER = 0,
  BUTTON_UP,
  BUTTON_DOWN,
  BUTTON_RETURN,
  BUTTON_RESERV,
  
} BUTTON_FUNC_t;



void MENU_MainHandler(BUTTON_FUNC_t button);

void MENU_SetTime(BUTTON_FUNC_t button);

void MENU_Stopwatch(BUTTON_FUNC_t button);

void MENU_Timer(BUTTON_FUNC_t button);

void MENU_Alarm_1(BUTTON_FUNC_t button);

void MENU_Alarm_2(BUTTON_FUNC_t button);

/**
  * @brief  Выход из режима установки времени
  */
void MENU_TimeReturn(void);

/**
  * @brief  Выход из режима установки будильника
  */
void MENU_AlarmReturn(void);

/**
  * @brief  Сохраняет текущее время в sCurrentTime
  */
void MENU_SaveCurrentTime(void);

/**
  * @brief  Устанавливаем время срабатывания и активируем таймер
  */
void MENU_TimerSetTime(void);

/**
  * @brief  Корректировка часов
  */
void MENU_ChangeHours(BUTTON_FUNC_t button);

/**
  * @brief  Корректировка минут
  */
void MENU_ChangeMinutes(BUTTON_FUNC_t button);

/**
  * @brief  Корректировка года
  */
void MENU_ChangeYear(BUTTON_FUNC_t button);

/**
  * @brief  Корректировка месяца
  */
void MENU_ChangeMonth(BUTTON_FUNC_t button);

/**
  * @brief  Корректировка дня
  */
void MENU_ChangeDay(BUTTON_FUNC_t button);

/**
  * @brief  Отсчет и вывод времени секундомера
  */
void MENU_WriteStopwatch(void);

/**
  * @brief  Вызывается 2 раза в секунду. Отвечает за мигание различных символов.
  */
void MENU_Blink(void);

/**
  * @brief  Таймеры и будильники
  */
void MENU_Timers(void);

#endif /* #ifndef MENU_H */

/******************************* (C) LiteMesh *********************************/