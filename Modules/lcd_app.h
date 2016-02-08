/******************************* (C) LiteMesh **********************************
 * @module  LCD
 * @file    lcd.h
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   lcd
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef LCD_APP_H
#define LCD_APP_H




void LCD_WriteChar(char* car, uint8_t position);

/**
  * @brief  This function writes a string in the LCD
  */
void LCD_WriteString(char* str);




#endif /* #ifndef LCD_APP_H */

/****************************** (C) LiteMesh ***************** end of file ****/