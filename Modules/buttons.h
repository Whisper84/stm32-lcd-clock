/******************************* (C) LiteMesh **********************************
 * @module  Buttons
 * @file    buttons.h
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   Кнопки, концевики и т.д.
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

#ifndef BUTTONS_H
#define BUTTONS_H

#define BUTTONS_TIC_DELLAY   0   
   
/**
  * Статус кнопки
  */
typedef enum 
{  
  BUT_OFF = 0,
  BUT_ON,
  BUT_NO_DEFINE,
}BUTTON_STATE;

/**
  * @brief  Структура параметров объекта Button
  */
typedef struct 
{  
  GPIO_TypeDef*     GPIOx;          // Порт
  uint16_t          GPIO_Pin;       // Пин
  uint16_t          timeDelay;      // Время задержки антидребезга (время измеряется
                                    // во временных интервалах с которыми вызывается
                                    // обработчик кнопки
  uint16_t          counterDelay;   // Счетчик антидребезга
  BUTTON_STATE      stateOld;       // Прошлое состояние кнопки
  BUTTON_STATE      stateNew;       // Текущее состояние кнопки
  void              (*pressHandler)();   // Обработчик нажатия кнопки
  void              (*unpressHandler)(); // Обработчик отпускания кнопки
  uint8_t           ID;             // уникальный идентификатор (можно не использовать)
}BUTTON_NO_FIX;   

/**
  * @brief  Функция определяет режим работы тестера, вызывается при нажатии 
  *         какой-либо кнопки
  * @retval 
  */
void BUTTON_State(uint8_t state);

/**
  * @brief  Обработчик кнопки без фиксации. В обработчике реализована защита
  *         от дребезга с помощью задержки. Защита срабатывает при нажатии 
  *         и при отпускании.
  */
void BUTTON_NoFixHandler(BUTTON_NO_FIX *button);

/**
  * @brief  Инициализация структур
  */
void BUTTON_Init(void);


/**
  * @brief   Возвращает сотояние ключа
  */
BUTTON_STATE BUTTON_GetState(BUTTON_NO_FIX *button);

/**
  * @brief  Обработчик отжатия кноки 
  */
void BUTTON_1_UpHandler();

/**
  * @brief  Обработчик нажатия кноки 
  */
void BUTTON_1_DownHandler();

/**
  * @brief  Обработчик нажатия кноки 
  */
void BUTTON_2_DownHandler();

/**
  * @brief  Обработчик нажатия кноки 
  */
void BUTTON_3_DownHandler();

/**
  * @brief  Обработчик нажатия кноки 
  */
void BUTTON_4_DownHandler();

/**
  * @brief  Обработчик нажатия кноки 
  */
void BUTTON_5_DownHandler();

/**
  * @brief  Вызов обработчиков всех кнопок
  * @retval 
  */
void BUTTON_CommonHandler();

/**
  * @brief  Инициализация buzzer
  * @retval 
  */
void BUZZER_Init(void);

/**
  * @brief  
  */
void BUZZER_On(void);

/**
  * @brief  
  */
void BUZZER_Off(void);

/**
  * @brief  
  */
void BuzzerSignal(void);

#endif /* #ifndef BUTTONS_H */

/******************************* (C) LiteMesh *********************************/