/******************************* (C) LiteMesh **********************************
 * @module  Console
 * @file    portConsole.c
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   Port functions console lib
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

#include "stm32f10x.h" 
#include "port_microrl.h"
#include "microrl.h"
#include "config.h"
#include "rtc_test.h"
#include "onewire.h"

#include <stdio.h>
#include <stdlib.h>
#include <string.h>


microrl_t rl;
microrl_t *prl = &rl;

/*
AVR platform specific implementation routines (for Atmega8, rewrite for your MC)
*/
#define _AVR_DEMO_VER "1.0"

// definition commands word
#define _CMD_HELP        "help"
#define _CMD_CLEAR       "clear"
#define _CMD_TIME        "time"
#define _CMD_TEMP        "temp"

// arguments for set/clear
#define _SCMD_PB  "port_b"
#define _SCMD_PD  "port_d"

#define _NUM_OF_CMD 
#define _NUM_OF_SETCLEAR_SCMD 2

//available  commands
char * keyworld [] = {
  _CMD_HELP,
  _CMD_CLEAR,
  _CMD_TIME,
  _CMD_TEMP,
};

// 'set/clear' command argements
char * set_clear_key [] = {_SCMD_PB, _SCMD_PD};

// array for comletion
char * compl_world [_NUM_OF_CMD + 1];

/**
  * @brief  
  * @retval 
  */
void MICRORL_Init(void)
{
  microrl_init(prl, print);
  microrl_set_execute_callback (prl, execute);
  
}

/**
  * @brief  Print callback for microrl library
  * @retval 
  */
void print (const char *str)
{
  printf(str);
}

/**
  * @brief  
  * @retval 
  */
void MICRORL_GetChar(uint8_t ch)
{
  microrl_insert_char(prl, ch);
}

//*****************************************************************************
// execute callback for microrl library
// do what you want here, but don't write to argv!!! read only!!
int execute (int argc, const char * const * argv)
{
	int i = 0;
	// just iterate through argv word and compare it with your commands
	while (i < argc) {
		if (strcmp (argv[i], _CMD_HELP) == 0) 
        {
 		  print ("microrl library v");
		  print (MICRORL_LIB_VER);
		  print ("\n\r");
		  print_help ();        // print help
		} 
        else if (strcmp (argv[i], _CMD_CLEAR) == 0) 
        {
		  print ("\033[2J");    // ESC seq for clear entire screen
		  print ("\033[H");     // ESC seq for move cursor at left-top corner
		}  
		/* Вывести даду и время */
        else if (strcmp (argv[i], _CMD_TIME) == 0) 
        {
          rtc_print_time();
		  return 0;
		}
        /* Вывести температуру */
		else if (strcmp (argv[i], _CMD_TEMP) == 0) 
        {
          float temper;
		  if (!DALLAS_GetTemperature(&temper))
			printf ("Temperature: %.2f\r\n", temper);
		  return 0;
		}                		
		
        else 
        {
		  print ("command: '");
		  print ((char*)argv[i]);
		  print ("' Not found.\n\r");
		}
		i++;
	}
	return 0;
}

void print_help (void)
{
	print ("Command:\n\r");
	print ("\tclear               - clear screen\n\r");
    print ("\ttime                - get data and time\n\r");
	print ("\ttemp                - get temperature\n\r");
}

/******************************* (C) LiteMesh *********************************/