/******************************* (C) LiteMesh **********************************
 * @module  rtc
 * @file    rtc.c
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   Real Time Clock
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

   
#include "stm32f10x.h"
#include "rtc.h"

static uint16_t RTC_Year = 0;
static uint8_t RTC_Month = 0;
static uint8_t RTC_Day = 0;
static uint8_t RTC_Hour = 0;
static uint8_t RTC_Minute = 0;
static uint8_t RTC_Second = 0;

/**
  * @brief  Флаг устанавливается в прерывании с интервалом в 1 секунду
  */
static uint8_t RTC_SecondFlag = 0;



/**
  * @brief  Расчет времени
  * @retval None
  */
void RTC_TimeCalculate(void)
{
  uint32_t value;
  uint32_t THH = 0, TMM = 0, TSS = 0;
  
  value = RTC_GetCounter();
  
  /* Reset RTC Counter when Time is 23:59:59 */
  if (value == 0x0001517F)
  {
     RTC_SetCounter(0x0);
     RTC_WaitForLastTask();
  }
  
  RTC_Hour = value / 3600;
  RTC_Minute = (value % 3600) / 60;
  RTC_Second = (value % 3600) % 60;

}

/**
  * @brief
  * @retval
  */
uint8_t RTC_GetHour(void)
{
  return RTC_Hour;
}

/**
  * @brief
  * @retval
  */
uint8_t RTC_GetMinute(void)
{
  return RTC_Minute;
}

/**
  * @brief
  * @retval
  */
uint8_t RTC_GetSecond(void)
{
  return RTC_Second;
}

/**
  * @brief
  * @retval
  */
void RTC_SetSecondFlag(void)
{
  RTC_SecondFlag = 1;
}

/**
  * @brief
  * @retval
  */
void RTC_ResetSecondFlag(void)
{
  RTC_SecondFlag = 0;
}

/**
  * @brief
  * @retval
  */
uint8_t RTC_GetSecondFlag(void)
{
  return RTC_SecondFlag;
}


/**
  * @brief  Настройка RTC модуля и прерываний
  * @retval 
  */
void RTC_Init(void)
{
  RTC_NVIC_Init();
    
  /* Enable PWR and BKP clocks */
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_PWR | RCC_APB1Periph_BKP, ENABLE);

  /* Allow access to BKP Domain */
  PWR_BackupAccessCmd(ENABLE);

  /* Reset Backup Domain */
  BKP_DeInit(); 
  
  RCC_LSEConfig(RCC_LSE_ON);
  
  while (RCC_GetFlagStatus(RCC_FLAG_LSERDY) == RESET) {}

  /* Select LSE as RTC Clock Source */
  RCC_RTCCLKConfig(RCC_RTCCLKSource_LSE);

  /* Enable RTC Clock */
  RCC_RTCCLKCmd(ENABLE);

  /* Wait for RTC registers synchronization */
  RTC_WaitForSynchro();

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Enable the RTC Second */
  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();

  /* Set RTC prescaler: set RTC period to 1sec */
  RTC_SetPrescaler(32767); /* RTC period = RTCCLK/RTC_PR = (32.768 KHz)/(32767+1) */

  /* Wait until last write operation on RTC registers has finished */
  RTC_WaitForLastTask();
}

/**
  * @brief  Настройка прерывания от RTC модуля
  * @param  None
  * @retval None
  */
void RTC_NVIC_Init(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;

  /* Configure one bit for preemption priority */
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_1);

  /* Enable the RTC Interrupt */
  NVIC_InitStructure.NVIC_IRQChannel = RTC_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}



/******************************* (C) LiteMesh *********************************/