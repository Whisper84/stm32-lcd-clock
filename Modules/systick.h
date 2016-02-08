/******************************* (C) LiteMesh **********************************
 * @module  systick
 * @file    systick.h
 * @version 1.0.0
 * @date    29.05.2015
 * $brief   Functions to work with System Timer. 
 *          Module is based on code teplofizik, 2013
 *******************************************************************************
 * @history     Version  Author         Comment
 * 29.05.2015   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

#ifndef SYSTICK_H
#define SYSTICK_H
   
#define TIMER_HANDLERS    20  // Максимально количество обработчкиков

typedef void (*TTimerHandler)();

/**
  * @brief  Запуск таймера с заданной частотой
  */
void timer_Init(uint32_t Frequency);

/**
  * @brief  Добавить функцию в список вызова. 
  *         Handler будет вызываться с заданной частотой
  */
void timer_AddFunction(uint16_t Frequency, TTimerHandler Handler);

/**
  * @brief  Изменить частоту таймера
  */
void timer_ChangeFrequency(TTimerHandler Handler, uint16_t Frequency);

/**
  * @brief  Возобночить работу задачи
  */
void timer_Resume(TTimerHandler Handler);

/**
  * @brief  Перезапуск задачи
  */
void timer_Restart(TTimerHandler Handler);

/**
  * @brief  Перезапуск задачи. Задача будет вызвана сразу.
  */
void timer_RestartAtOnce(TTimerHandler Handler);

/**
  * @brief  Остановить задачу
  */
void timer_Stop(TTimerHandler Handler);

/**
  * @brief  Функция перебора и вызова актуальных задач. 
  *         Должна вызываться в главном цикле
  */
void timer_Main(void);

/**
  * @brief  
  * @retval Частота тактирования ядра
  */
uint32_t clock_GetSYS(void);

/**
  * @brief  Задержка в миллисекундах
  */
void Delay_ms(__IO uint32_t nTime);

/**
  * @brief  Вспомогательная функция для реализации Delay_ms
  */
void TimingDelay_Decrement(void);

#endif // SYSTICK_H
