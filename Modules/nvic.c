/******************************* (C) LiteMesh **********************************
 * @module  NVIC
 * @file    nvic.c
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   NVIC
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

#include "stm32f10x.h"
#include "nvic.h"

/**
  * @brief  
  * @retval 
  */
void API_NVIC_Init(void)
{
  /*
  NVIC_InitTypeDef   NVIC_InitStructure;
  
  NVIC_InitStructure.NVIC_IRQChannel = TIM1_TRG_COM_TIM17_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  //NVIC_SetPriority (TIM1_TRG_COM_TIM17_IRQn, 2);  // Понижаем приоритет
  
  NVIC_InitStructure.NVIC_IRQChannel = USART1_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  //NVIC_SetPriority (USART1_IRQn, 2);  // Понижаем приоритет
  */
}


 /******************************* (C) LiteMesh *********************************/