/******************************* (C) LiteMesh **********************************
 * @module  Console
 * @file    portConsole.h
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   Port functions console lib
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

/* Define to prevent recursive  ----------------------------------------------*/
#ifndef PORT_MICRORL_H
#define PORT_MICRORL_H
 
/**
  * @brief  
  */
void MICRORL_Init(void);

/**
  * @brief  
  */
void MICRORL_GetChar(uint8_t ch);

/**
  * @brief  Print callback for microrl library
  */
void print (const char * str);

int execute (int argc, const char * const * argv);

void print_help (void);

#endif /* #ifndef PORTCONSOLE_H */

/****************************** (C) LiteMesh ***************** end of file ****/