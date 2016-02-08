/******************************* (C) LiteMesh **********************************
 * @module  LCD
 * @file    lcd.c
 * @version 1.0.0
 * @date    XX.XX.XXXX
 * $brief   lcd
 *******************************************************************************
 * @history     Version  Author         Comment
 * XX.XX.XXXX   1.0.0    Telenkov D.A.  First release.
 *******************************************************************************
 */

#include "stm32f10x.h"
#include "lcd_app.h"
  
uint8_t LCD_PhaseExecute = 0;
uint8_t LCD_SegmentIndex = 0;

uint16_t  SegmentsValues_Lower_Quarter_Digits[4];

uint16_t  digit[3];
uint16_t  LCD_PortVal[4] = {0};
uint16_t temp = 0;


const uint16_t LetterMap[26]=
   { 
     /* A      B      C      D      E      F      G      H      I  */
     0x4D70,0x6469,0x4111,0x6449,0x4911,0x4910,0x4171,0x0D70,0x6009,
     /* J      K      L      M      N      O      P      Q      R  */
     0x0451,0x0B12,0x0111,0x8750,0x8552,0x4551,0x4D30,0x4553,0x4D32,
     /* S      T      U      V      W      X      Y      Z  */     
     0x4961,0x6008,0x0551,0x0390,0x05D2,0x8282,0x8208,0x4281 
   };

const uint16_t NumberMap[10]=
    { 
      /* 0      1      2      3      4      5      6      7      8      9  */
      0x6A6, 0x06, 0x2E4, 0x0E6, 0x446, 0x4E2, 0x6E2, 0x086, 0x6E6, 0x4E6
    };


/**
  * @brief  converts an ascii char to the a LCD digit (previous coding)
  * @param c: a char to display.
  * @retval : None.
  */
void LCD_Convert(char* c)
{ 
  uint16_t car=0,tmp;
  uint8_t i;
  const uint16_t mask[3]={0x0F00,0x00F0,0x000F};
  const uint8_t shift[2]={8,4};

  if ((*c<0x5B)&(*c>0x40))
  {
    car = LetterMap[*c-0x41];
  }
  if ((*c<0x3A)&(*c>0x2F)) 
  {
    car = NumberMap[*c-0x30];
  }
  if (*c==0x20)
  {
    car =0x00;
  }
  
  for (i=0;i<2;i++)
  {
    tmp = car&mask[i];
    digit[i] = tmp>>shift[i];
  }
  
  digit[2] = (car&mask[2]); 
}

/**
  * @brief  This function writes a char in the LCD frame buffer
  * @param car: the caracter to dispaly.  
  * @param position: position in the LCD of the caracter to write. 
  *   it can be: [0:3] (in case of CT4-098 LCD glass)
  * @retval : None.
  */
void LCD_WriteChar(char* car, uint8_t position)
  {
   uint8_t i ,j;
   uint8_t temp = 0;
   //const uint16_t mask[4] = {0x000F, 0x00F0, 0x0F00, 0xF000};
   const uint8_t MyMask[4] = {0x08, 0x04, 0x02, 0x01};
   const uint8_t shift[4] = {3, 2, 1, 0};
      
   LCD_Convert(car);
   
   switch(position)
   {
     case 0: 
       
       for (j = 0; j < 3; j++)
         for (i = 0; i < 3; i++)
           LCD_PortVal[i] |= ((digit[j] & MyMask[i]) >> shift[i])   << j;

     break;
     
     case 1: 
    
       for (j = 0; j < 3; j++)
       {
         if (j == 2)
           temp = 9;
         else
           temp = 3;
           
         for (i = 0; i < 3; i++)
           LCD_PortVal[i] |= ((digit[j] & MyMask[i]) >> shift[i])   << j + temp;
       }
     break;
            
     case 2: 
       
       for (j = 0; j < 3; j++)
       {
         if (j == 2)
           temp = 8;
         else
           temp = 5;
           
         for (i = 0; i < 3; i++)
           LCD_PortVal[i] |= ((digit[j] & MyMask[i]) >> shift[i])   << j + temp;
       }

     break;
 
     case 3: 
       
     for (j = 0; j < 3; j++)
     {  
       if (j == 0)
         temp = 9;
       if (j == 1)
         temp = 6;
       if (j == 2)
         temp = 6;  
         
       for (i = 0; i < 3; i++)
         LCD_PortVal[i] |= ((digit[j] & MyMask[i]) >> shift[i])   << j + temp;
     }
       
     default:
     break;
    }
  
  }

/**
  * @brief  This function writes a string in the LCD
  * @param str: string to dispaly in the LCD.
  * @retval : None.
  */
void LCD_WriteString(char* str)
{
  uint8_t i;
  const uint16_t SegDigitMask[3] = {0xFF2D, 0xF000, 0xF000};
  
  LCD_PortVal[0] &= SegDigitMask[0];
  LCD_PortVal[1] &= SegDigitMask[1];
  LCD_PortVal[2] &= SegDigitMask[2];
   
  for (i = 0; i < 4; i++)
  {
    LCD_WriteChar(str + i, i);
  }
} 



/******************************* (C) LiteMesh *********************************/