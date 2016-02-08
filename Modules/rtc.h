/******************************* (C) LiteMesh **********************************
 * @module  rtc
 * @file    rtc.h
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   Real Time Clock
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef RTC_H
#define RTC_H
  
/**
  * @brief  Расчет времени
  */
void RTC_TimeCalculate(void);

/**
  * @brief
  */
uint8_t RTC_GetHour(void);

/**
  * @brief
  */
uint8_t RTC_GetMinute(void);

/**
  * @brief
  */
uint8_t RTC_GetSecond(void);

/**
  * @brief
  */
void RTC_SetSecondFlag(void);

/**
  * @brief
  */
void RTC_ResetSecondFlag(void);

/**
  * @brief
  */
uint8_t RTC_GetSecondFlag(void);

/**
  * @brief  Настройка RTC модуля и прерываний
  */
void RTC_Init(void);

/**
  * @brief  Настройка прерывания от RTC модуля
  */
void RTC_NVIC_Init(void);

#endif /* #ifndef RTC_H */

/****************************** (C) LiteMesh ***************** end of file ****/