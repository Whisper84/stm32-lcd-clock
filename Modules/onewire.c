/*
 * onewire.c
 *
 *  Created on: 13.02.2012
 *      Author: di
 */

#include "stm32f10x.h"
#include "onewire.h"
#include "systick.h"

#include <stdio.h>
#include <stdlib.h>
   
#define DALLAS_USART 	   USART2
#define DALLAS_DMA_CH_RX   DMA1_Channel6
#define DALLAS_DMA_CH_TX   DMA1_Channel7
#define DALLAS_DMA_FLAG	   DMA1_FLAG_TC6

#define DALLAS_0	0x00
#define DALLAS_1	0xff
#define DALLAS_R_1	0xff
   
/**
  * @brief  Буфер для приема/передачи по 1-wire
  */
uint8_t ow_buf[8];

/**
  * @brief  Преобразование температуры
  * @retval 
  */
bool DALLAS_GetTemperature(float *temp)
{
  uint8_t  state;
  uint8_t  tempBuf[2] = {0};
  uint16_t tempInt = 0;
  float    temperature;
  
  DALLAS_OutSetAsTX();
        
  /* Запускаем преобразование температуры */
  state = DALLAS_Send(DALLAS_SEND_RESET, "\xcc\x44", 2, NULL, NULL, DALLAS_NO_READ);
  
  if (state == DALLAS_NO_DEVICE)
  {
    return false;
  }
  
  DALLAS_OutSetAsPower();  
  Delay_ms(1000);
  DALLAS_OutSetAsTX();
  
  /* Чтение температуры */
  state = DALLAS_Send(DALLAS_SEND_RESET, "\xcc\xbe\xff\xff", 4, tempBuf, 2, 2);

  /* Ошибка при чтении */
  if (state != DALLAS_OK)  
  {
    //printf("\n\r 1-WIRE sensor read error\n\r");
    return false;
  }
    
  tempInt = tempBuf[0];
  tempInt |= tempBuf[1] << 8;
  
  /* Число в дополнительном коде */
  if ((tempInt & 0x8000))  
  {
    tempInt = ~tempInt + 1;
    temperature = -1*(tempInt/16.0);
  }
  else
    temperature = tempInt/16.0;
  
  if ((temperature == 85.0) || (temperature == -0.0625))
	return false;
  
  *temp = temperature;
	
  /* Выводим результат в консоль */
  //printf("%s %.2f %s", " Temperature:", temperature, "deg.\n\r");

  return true;
}


/**
  * @brief  Функция преобразует один байт в восемь, для передачи через USART
  * @param  ow_byte - байт, который надо преобразовать
  * @param  ow_bits - ссылка на буфер, размером не менее 8 байт
  * @retval 
  */
void DALLAS_ToBits(uint8_t ow_byte, uint8_t *ow_bits) 
{
  uint8_t i;

  for (i = 0; i < 8; i++) 
  {
    if (ow_byte & 0x01) 
    {
      *ow_bits = DALLAS_1;
    } 
    else 
    {
      *ow_bits = DALLAS_0;
    }
    ow_bits++;
    ow_byte = ow_byte >> 1;
  }
}

/**
  * @brief  Обратное преобразование - из того, что получено через USART опять собирается байт
  * @param  ow_bits - ссылка на буфер, размером не менее 8 байт
  * @param  ow_byte - собранный байт данных
  * @retval 
  */
uint8_t DALLAS_ToByte(uint8_t *ow_bits) 
{
  uint8_t ow_byte, i;
  ow_byte = 0;

  for (i = 0; i < 8; i++) 
  {
    ow_byte = ow_byte >> 1;
    if (*ow_bits == DALLAS_R_1) 
    {
      ow_byte |= 0x80;
    }
    ow_bits++;
  }
  return ow_byte;
}

/**
  * @brief  Настроить TX пин как питающий выход
  * @retval 
  */
void DALLAS_OutSetAsPower(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;  

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);
  GPIO_SetBits(GPIOA, GPIO_Pin_2);
}

/**
  * @brief  Настроить TX пин как передатчик
  * @retval 
  */
void DALLAS_OutSetAsTX(void)
{
  GPIO_InitTypeDef GPIO_InitStruct;  

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

}

//-----------------------------------------------------------------------------
// инициализирует USART и DMA
//-----------------------------------------------------------------------------
/**
  * @brief  Настроить USART1 для 
  * @retval 
  */
uint8_t DALLAS_Init() 
{
  GPIO_InitTypeDef  GPIO_InitStruct;
  USART_InitTypeDef USART_InitStructure;
  
  RCC_APB2PeriphClockCmd(RCC_APB2Periph_GPIOA | RCC_APB2Periph_AFIO, ENABLE);

  GPIO_InitStruct.GPIO_Pin = GPIO_Pin_2;
  GPIO_InitStruct.GPIO_Mode = GPIO_Mode_AF_OD;
  GPIO_InitStruct.GPIO_Speed = GPIO_Speed_10MHz;
  GPIO_Init(GPIOA, &GPIO_InitStruct);

  RCC_APB1PeriphClockCmd(RCC_APB1Periph_USART2, ENABLE);
                
  RCC_AHBPeriphClockCmd(RCC_AHBPeriph_DMA1, ENABLE);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;

  USART_Init(DALLAS_USART, &USART_InitStructure);
  USART_Cmd(DALLAS_USART, ENABLE);
        
  // Здесь вставим разрешение работы USART в полудуплексном режиме 
  USART_HalfDuplexCmd(DALLAS_USART, ENABLE);
  
  return DALLAS_OK;
}

//-----------------------------------------------------------------------------
// осуществляет сброс и проверку на наличие устройств на шине
//-----------------------------------------------------------------------------
uint8_t DALLAS_Reset() 
{
  uint8_t ow_presence;
  
  USART_InitTypeDef USART_InitStructure;

  USART_InitStructure.USART_BaudRate = 9600;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(DALLAS_USART, &USART_InitStructure);

  // отправляем 0xf0 на скорости 9600
  USART_ClearFlag(DALLAS_USART, USART_FLAG_TC);
  USART_SendData(DALLAS_USART, 0xf0);

  while (USART_GetFlagStatus(DALLAS_USART, USART_FLAG_TC) == RESET) { }

  ow_presence = USART_ReceiveData(DALLAS_USART);

  USART_InitStructure.USART_BaudRate = 115200;
  USART_InitStructure.USART_WordLength = USART_WordLength_8b;
  USART_InitStructure.USART_StopBits = USART_StopBits_1;
  USART_InitStructure.USART_Parity = USART_Parity_No;
  USART_InitStructure.USART_HardwareFlowControl = USART_HardwareFlowControl_None;
  USART_InitStructure.USART_Mode = USART_Mode_Tx | USART_Mode_Rx;
  USART_Init(DALLAS_USART, &USART_InitStructure);

  if (ow_presence != 0xf0) {
    return DALLAS_OK;
  }

  return DALLAS_NO_DEVICE;
}

/**
  * @brief  Процедура общения с шиной 1-wire
  */
uint8_t DALLAS_Send(uint8_t sendReset, uint8_t *command, uint8_t cLen,
		uint8_t *data, uint8_t dLen, uint8_t readStart) {

  uint32_t timeOutCounter = 0;		  
		  
  /* если требуется сброс - сбрасываем и проверяем на наличие устройств */
  if (sendReset == DALLAS_SEND_RESET) 
  {
    if (DALLAS_Reset() == DALLAS_NO_DEVICE) 
    {
      return DALLAS_NO_DEVICE;
    }
  }
  
  while (cLen > 0) 
  {
    DALLAS_ToBits(*command, ow_buf);
    command++;
    cLen--;

    DMA_InitTypeDef DMA_InitStructure;

    /* DMA на чтение */
    DMA_DeInit(DALLAS_DMA_CH_RX);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(USART2->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) ow_buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralSRC;
    DMA_InitStructure.DMA_BufferSize = 8;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DALLAS_DMA_CH_RX, &DMA_InitStructure);

    /* DMA на запись */
    DMA_DeInit(DALLAS_DMA_CH_TX);
    DMA_InitStructure.DMA_PeripheralBaseAddr = (uint32_t) &(USART2->DR);
    DMA_InitStructure.DMA_MemoryBaseAddr = (uint32_t) ow_buf;
    DMA_InitStructure.DMA_DIR = DMA_DIR_PeripheralDST;
    DMA_InitStructure.DMA_BufferSize = 8;
    DMA_InitStructure.DMA_PeripheralInc = DMA_PeripheralInc_Disable;
    DMA_InitStructure.DMA_MemoryInc = DMA_MemoryInc_Enable;
    DMA_InitStructure.DMA_PeripheralDataSize = DMA_PeripheralDataSize_Byte;
    DMA_InitStructure.DMA_MemoryDataSize = DMA_MemoryDataSize_Byte;
    DMA_InitStructure.DMA_Mode = DMA_Mode_Normal;
    DMA_InitStructure.DMA_Priority = DMA_Priority_Low;
    DMA_InitStructure.DMA_M2M = DMA_M2M_Disable;
    DMA_Init(DALLAS_DMA_CH_TX, &DMA_InitStructure);

    /* Старт цикла отправки */
    USART_ClearFlag(DALLAS_USART, USART_FLAG_RXNE | USART_FLAG_TC /*| USART_FLAG_TXE*/);
    USART_DMACmd(DALLAS_USART, USART_DMAReq_Tx | USART_DMAReq_Rx, ENABLE);
    DMA_Cmd(DALLAS_DMA_CH_RX, ENABLE);
    DMA_Cmd(DALLAS_DMA_CH_TX, ENABLE);
        
    /* Ждем, пока не примем 8 байт */
    while (DMA_GetFlagStatus(DALLAS_DMA_FLAG) == RESET) 
	{
      timeOutCounter++;
	  if (timeOutCounter > 1000)
		return DALLAS_ERROR;
	}
    
    /* Отключаем DMA */
    DMA_Cmd(DALLAS_DMA_CH_TX, DISABLE);
    DMA_Cmd(DALLAS_DMA_CH_RX, DISABLE);
    USART_DMACmd(DALLAS_USART, USART_DMAReq_Tx | USART_DMAReq_Rx, DISABLE);

    /* Если прочитанные данные кому-то нужны - выкинем их в буфер */
    if (readStart == 0 && dLen > 0) 
    {
      *data = DALLAS_ToByte(ow_buf);
      data++;
      dLen--;
    } 
    else
    {
      if (readStart != DALLAS_NO_READ) 
      {
        readStart--;
      }
    }
    
  }
  
  return DALLAS_OK;
}
