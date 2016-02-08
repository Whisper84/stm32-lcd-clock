/******************************* (C) LiteMesh **********************************
 * @module  usart
 * @file    usart.c
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   Template
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

#include "stm32f10x.h"   
#include "usart.h"
#include "port_microrl.h"

#include <stdio.h>
   
#ifdef __GNUC__    
  #define PUTCHAR_PROTOTYPE int __io_putchar(int ch)    
#else    
  #define PUTCHAR_PROTOTYPE int fputc(int ch, FILE *f)    
#endif  

PUTCHAR_PROTOTYPE   
{   
  USART_SendData(CONSOLE_USART, (u8) ch);   
  while(USART_GetFlagStatus(CONSOLE_USART, USART_FLAG_TXE) == RESET) {}   
  return ch;   
}   

void USART_UserInit(void) 
{
  GPIO_InitTypeDef  GPIO_InitStructure;
  USART_InitTypeDef USART_InitStructure;
  NVIC_InitTypeDef  NVIC_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_AFIO, ENABLE);
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART3, ENABLE); 
    
  GPIO_PinRemapConfig(GPIO_PartialRemap_USART3, ENABLE);  
  
  /* Отключение JTAG */
  GPIO_PinRemapConfig(GPIO_Remap_SWJ_JTAGDisable, ENABLE); 
  
  /* USART3_Rx */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_11;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_IN_FLOATING;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
  
  /* USART3_Tx */
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_10;
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_AF_PP;
  GPIO_Init(GPIOC, &GPIO_InitStructure);
     
  NVIC_InitStructure.NVIC_IRQChannel = USART3_IRQn;
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = 0x00;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 0;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
  
  USART_DeInit(CONSOLE_USART);
  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Rx | USART_Mode_Tx;
  USART_Init(CONSOLE_USART, &USART_InitStructure);
  
  USART_Cmd(CONSOLE_USART, ENABLE);  
    
  USART_ITConfig(CONSOLE_USART, USART_IT_RXNE, ENABLE);
  USART_ITConfig(CONSOLE_USART, USART_IT_TXE, DISABLE);

}

void USART3_IRQHandler(void)
{
  if(USART_GetITStatus(CONSOLE_USART, USART_IT_RXNE) != RESET)
  {
    USART_ClearITPendingBit(CONSOLE_USART, USART_IT_RXNE);
	
    MICRORL_GetChar(USART_ReceiveData(CONSOLE_USART));
  }
  
  if(USART_GetITStatus(CONSOLE_USART, USART_IT_TXE) != RESET)
  {   
    USART_ClearITPendingBit(CONSOLE_USART, USART_IT_TC);

  }
 
}

/******************************* (C) LiteMesh *********************************/