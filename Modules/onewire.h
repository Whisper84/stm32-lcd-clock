/*
 * onewire.h
 *
 *  Version 1.0.1
 */

#ifndef ONEWIRE_H_
#define ONEWIRE_H_

#include <stdbool.h>

// первый параметр функции OW_Send
#define DALLAS_SEND_RESET	1
#define DALLAS_NO_RESET		2

// статус возврата функций
#define DALLAS_OK		    1
#define DALLAS_ERROR		2
#define DALLAS_NO_DEVICE	3

#define DALLAS_NO_READ		0xff
#define DALLAS_READ_SLOT	0xff

/**
  * @brief  Преобразование температуры
  * @retval false - ошибка преобразования
  *         true - преобразование завершилось успешно
  */
bool DALLAS_GetTemperature(float *temp);

uint8_t DALLAS_Init();

void DALLAS_OutSetAsPower(void);

void DALLAS_OutSetAsTX(void);

/**
  * @brief  Процедура общения с шиной 1-wire
  * @param sendReset: посылать RESET в начале общения (OW_SEND_RESET или OW_NO_RESET)
  * @param command: массив байт, отсылаемых в шину. Если нужно чтение - отправляем OW_READ_SLOTH
  * @param cLen: длина буфера команд, столько байт отошлется в шину
  * @param data: если требуется чтение, то ссылка на буфер для чтения
  * @param dLen: длина буфера для чтения. Прочитается не более этой длины
  * @param readStart: с какого символа передачи начинать чтение (нумеруются с 0)
  *                   можно указать OW_NO_READ, тогда можно не задавать data и dLen  
  * @retval : None.
  */
uint8_t DALLAS_Send(uint8_t sendReset, uint8_t *command, uint8_t cLen, 
                uint8_t *data, uint8_t dLen, uint8_t readStart);

#endif /* ONEWIRE_H_ */
