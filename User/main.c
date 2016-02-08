#include "stm32f10x.h"

#include "main.h"
#include "rtc.h"
#include "systick.h"
#include "usart.h"
#include "lcd.h"
#include "menu.h"
#include "onewire.h"
#include "buttons.h"
#include "port_microrl.h"
#include "rtc_test.h"

#include <stdio.h>

int main()
{
  timer_Init(1000);
  USART_UserInit();  
  MICRORL_Init();
  LCD_Init();
  BUTTON_Init();
  BUZZER_Init();
  
  rtc_init();
  rtc_set_test_time();
  
  DALLAS_Init();  // Датчик температуры
	
  timer_AddFunction(30, &BUTTON_CommonHandler);
  timer_AddFunction(250, &MENU_Blink);
  timer_AddFunction(1000, &MENU_Timers);
  timer_AddFunction(100, &BuzzerSignal);  

  RTC_ITConfig(RTC_IT_SEC, ENABLE);

  while (1)
  {
	timer_Main();
  }
}

#ifdef  USE_FULL_ASSERT

/**
  * @brief  Reports the name of the source file and the source line number
  *         where the assert_param error has occurred.
  * @param  file: pointer to the source file name
  * @param  line: assert_param error line source number
  * @retval None
  */
void assert_failed(uint8_t* file, uint32_t line)
{ 
  /* User can add his own implementation to report the file name and line number,
     ex: printf("Wrong parameters value: file %s on line %d\r\n", file, line) */

  /* Infinite loop */
  while (1)
  {
  }
}
#endif

