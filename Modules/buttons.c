/******************************* (C) LiteMesh **********************************
 * @module  Buttons
 * @file    buttons.c
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   Кнопки, buzzer, концевики, управляющие пины и т.д.
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

#include "stm32f10x.h"
#include "buttons.h"
#include "systick.h"
#include "buttons.h"
#include "menu.h"
#include "onewire.h"
   
#include <stdio.h>

BUTTON_NO_FIX   BUTTON_1;
BUTTON_NO_FIX   BUTTON_2;
BUTTON_NO_FIX   BUTTON_3;
BUTTON_NO_FIX   BUTTON_4;
BUTTON_NO_FIX   BUTTON_5;

extern bool fShowTemperature;
extern float temperature;

/**
  * @brief  Флаг устанавливается когда нужно податься звуковой сигнал
  */
bool fBuzzerSignal = false;

/**
  * @brief  Инициализация структур
  * @retval 
  */
void BUTTON_Init(void)
{
  GPIO_InitTypeDef          GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOA, ENABLE);
    
  BUTTON_1.GPIOx             = GPIOA;
  BUTTON_1.GPIO_Pin          = GPIO_Pin_8;
  BUTTON_1.timeDelay         = BUTTONS_TIC_DELLAY;
  BUTTON_1.pressHandler      = BUTTON_1_UpHandler;
  BUTTON_1.unpressHandler    = BUTTON_1_DownHandler; 
  
  BUTTON_2.GPIOx             = GPIOA;
  BUTTON_2.GPIO_Pin          = GPIO_Pin_9;
  BUTTON_2.timeDelay         = BUTTONS_TIC_DELLAY;
  BUTTON_2.pressHandler      = BUTTON_1_UpHandler;
  BUTTON_2.unpressHandler    = BUTTON_2_DownHandler;
  
  BUTTON_3.GPIOx             = GPIOA;
  BUTTON_3.GPIO_Pin          = GPIO_Pin_10;
  BUTTON_3.timeDelay         = BUTTONS_TIC_DELLAY;
  BUTTON_3.pressHandler      = BUTTON_1_UpHandler;
  BUTTON_3.unpressHandler    = BUTTON_3_DownHandler;
  
  BUTTON_4.GPIOx             = GPIOA;
  BUTTON_4.GPIO_Pin          = GPIO_Pin_11;
  BUTTON_4.timeDelay         = BUTTONS_TIC_DELLAY;
  BUTTON_4.pressHandler      = BUTTON_1_UpHandler;
  BUTTON_4.unpressHandler    = BUTTON_4_DownHandler;
  
  BUTTON_5.GPIOx             = GPIOA;
  BUTTON_5.GPIO_Pin          = GPIO_Pin_12;
  BUTTON_5.timeDelay         = BUTTONS_TIC_DELLAY;
  BUTTON_5.pressHandler      = BUTTON_1_UpHandler;
  BUTTON_5.unpressHandler    = BUTTON_5_DownHandler;
      
  GPIO_InitStructure.GPIO_Pin   = BUTTON_1.GPIO_Pin;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_IPU;
  GPIO_Init(BUTTON_1.GPIOx, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin   = BUTTON_2.GPIO_Pin;  
  GPIO_Init(BUTTON_2.GPIOx, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin   = BUTTON_3.GPIO_Pin;  
  GPIO_Init(BUTTON_3.GPIOx, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin   = BUTTON_4.GPIO_Pin;  
  GPIO_Init(BUTTON_4.GPIOx, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin   = BUTTON_5.GPIO_Pin;  
  GPIO_Init(BUTTON_5.GPIOx, &GPIO_InitStructure);
  
  BUTTON_1.stateOld     = BUTTON_GetState(&BUTTON_1);
  BUTTON_2.stateOld     = BUTTON_GetState(&BUTTON_2);
  BUTTON_3.stateOld     = BUTTON_GetState(&BUTTON_3);
  BUTTON_4.stateOld     = BUTTON_GetState(&BUTTON_4);
  BUTTON_5.stateOld     = BUTTON_GetState(&BUTTON_5);
}

/**
  * @brief  Обработчик кнопки без фиксации. В обработчике реализована защита
  *         от дребезга с помощью задержки. Защита срабатывает при нажатии 
  *         и при отпускании.
  * @param *button: указатель на структуру типа BUTTON_NO_FIX
  * @retval нет
  */
void BUTTON_NoFixHandler(BUTTON_NO_FIX *button)
{
  uint8_t temp;
  
  if (button->stateNew != BUT_NO_DEFINE)
  {  
    temp = GPIO_ReadInputDataBit(button->GPIOx, button->GPIO_Pin);
    
    if (temp == 0)  // определяем текущее (новое) состояние кнопки        
      button->stateNew = BUT_OFF;
    else
      button->stateNew = BUT_ON;
  
    if (button->stateNew != button->stateOld)  // зафиксирован факт нажатия/отжатия
      button->stateNew = BUT_NO_DEFINE;  // переводим кнопку в неопределенное состояние
  }
    
  else
  {
    button->counterDelay++;             // запускаем счетчик на задержку
    
    if (button->counterDelay > button->timeDelay)  // задержка закончилась
    {
      button->counterDelay = 0;

      temp = GPIO_ReadInputDataBit(button->GPIOx, button->GPIO_Pin);
      
      if (temp == 0)  // определяем текущее (новое) состояние кнопки        
      {  
        button->stateNew = BUT_OFF;
        button->unpressHandler();
      }  
      else
      {  
        button->stateNew = BUT_ON;
        button->pressHandler();
      }  
    }
  }
  
  button->stateOld = button->stateNew;

}

/**
  * @brief   Возвращает сотояние ключа
  * @retval  0 - линия разомкнута
  *          1 - линия замкнута
  */
BUTTON_STATE BUTTON_GetState(BUTTON_NO_FIX *button)
{
  return (BUTTON_STATE)GPIO_ReadInputDataBit(button->GPIOx, button->GPIO_Pin);
}

/**
  * @brief  Обработчик отжатия кноки 
  */
void BUTTON_1_UpHandler()
{
}

/**
  * @brief  Обработчик нажатия кноки 
  */
void BUTTON_1_DownHandler()
{
  MENU_MainHandler(BUTTON_ENTER);
}

/**
  * @brief  Обработчик нажатия кноки 
  */
void BUTTON_2_DownHandler()
{
  MENU_MainHandler(BUTTON_DOWN);
}

/**
  * @brief  Обработчик нажатия кноки 
  */
void BUTTON_3_DownHandler()
{
  MENU_MainHandler(BUTTON_UP);
}

/**
  * @brief  Обработчик нажатия кноки 
  */
void BUTTON_4_DownHandler()
{
  MENU_MainHandler(BUTTON_RETURN);
}

/**
  * @brief  Обработчик нажатия кноки 
  */
void BUTTON_5_DownHandler()
{
  
  if (DALLAS_GetTemperature(&temperature)) {
    //printf(" Temperature: %.2f deg.\r\n", temperature);
	fShowTemperature = true;
  }
  else {
	//printf(" Sensor error!\r\n");
  }
  
}


/**
  * @brief  Вызов обработчиков всех кнопок
  * @retval 
  */
void BUTTON_CommonHandler()
{
  BUTTON_NoFixHandler(&BUTTON_1);
  BUTTON_NoFixHandler(&BUTTON_2);
  BUTTON_NoFixHandler(&BUTTON_3);
  BUTTON_NoFixHandler(&BUTTON_4);
  BUTTON_NoFixHandler(&BUTTON_5);
}

/**
  * @brief  Инициализация buzzer
  * @retval 
  */
void BUZZER_Init(void)
{
  GPIO_InitTypeDef          GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
  
  GPIO_InitStructure.GPIO_Pin   = GPIO_Pin_0;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_2MHz;
  GPIO_InitStructure.GPIO_Mode  = GPIO_Mode_Out_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  GPIO_ResetBits(GPIOC, GPIO_Pin_0);
}

/**
  * @brief  
  */
void BUZZER_On(void)
{
  GPIO_SetBits(GPIOC, GPIO_Pin_0);
}

/**
  * @brief  
  */
void BUZZER_Off(void)
{
  GPIO_ResetBits(GPIOC, GPIO_Pin_0);
}

/**
  * @brief  
  */
void BuzzerSignal(void)
{
  static uint16_t counter = 0;
  
  if (fBuzzerSignal)
  {
    switch (counter)
	{
	  case 1 :
		BUZZER_On();
	  break;
	  
	  case 3 :	
		BUZZER_Off();
	  break;
	  
	  case 5 :
		BUZZER_On();
	  break;
	  
	  case 7 :	
		BUZZER_Off();
	  break;
	  
	  case 11 :
		counter = 0;
	  break;	
		
	  default :
      break;
	}
	counter++;
  }
  else
  {	
	counter = 0;
	BUZZER_Off();
  }	
}

/******************************* (C) LiteMesh *********************************/