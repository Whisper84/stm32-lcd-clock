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
#include "lcd.h"
  
uint8_t LCD_PhaseExecute = 0;
uint8_t LCD_SegmentIndex = 0;

uint16_t  SegmentsValues_Lower_Quarter_Digits[4];

uint16_t  digit[3];
uint16_t  LCD_PortVal[4] = {0};
uint16_t temp = 0;

uint16_t  CommonLine[4] = {
                            1 << (LCD_CommonLinesPinOffset + 8),
                            2 << (LCD_CommonLinesPinOffset + 8),
                            4 << (LCD_CommonLinesPinOffset + 8),
                            8 << (LCD_CommonLinesPinOffset + 8)
                          };


uint32_t  CommonLine_VDD_2[4] = { 
                            0xFFFF000F,
                            0xFFFF00F0,
                            0xFFFF0F00,
                            0xFFFFF000
                          };

/* GPIOs to be configured to Output PP */
uint32_t  CommonLine_OUT_PP[4] = {
                            0x00000003,
                            0x00000030,
                            0x00000300,
                            0x00003000
                           };

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
      //0x47D1,0x0640,0x4C31,0x4C61,0x0D60,0x4961,0x4971,0x4440,0x4D71,0x4D61
      0x6A6, 0x06, 0x2E4, 0x0E6, 0x446, 0x4E2, 0x6E2, 0x086, 0x6E6, 0x4E6
    };

//uint8_t SegmentsValues[4] = {1, 0, 0, 1};
uint8_t Segment_1[4] = {1, 0, 0, 1};
uint8_t Segment_2[4] = {1, 0, 0, 1};

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



/**
  * @brief  
  * @retval
  */
void LCD_Init(void)
{
  LCD_InitGpio();
  LCD_InitNVIC();
  LCD_InitTim();
}

void LCD_InitGpio(void)
{
  GPIO_InitTypeDef          GPIO_InitStructure;
  
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOC, ENABLE);
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOD, ENABLE);
  RCC_APB2PeriphClockCmd( RCC_APB2Periph_GPIOE, ENABLE);  
  
  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_0 | GPIO_Pin_1 | GPIO_Pin_2 |
                                GPIO_Pin_3 | GPIO_Pin_4 | GPIO_Pin_5 |
                                GPIO_Pin_6 | GPIO_Pin_7 | GPIO_Pin_8 |  
                                GPIO_Pin_9 | GPIO_Pin_10 | GPIO_Pin_11;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP;
  GPIO_Init( LCD_SegmentsPort, &GPIO_InitStructure);

  GPIO_InitStructure.GPIO_Pin = GPIO_Pin_12 | GPIO_Pin_13 | GPIO_Pin_14 | GPIO_Pin_15;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init( LCD_CommonLinesPort, &GPIO_InitStructure);
  
  GPIO_InitStructure.GPIO_Pin = LCD_BiasPlusPin;  
  GPIO_InitStructure.GPIO_Speed = GPIO_Speed_50MHz;
  GPIO_InitStructure.GPIO_Mode = GPIO_Mode_Out_PP; 
  GPIO_Init( LCD_BiasPort, &GPIO_InitStructure);
  
  GPIO_SetBits(LCD_BiasPort, LCD_BiasPlusPin);
  
  /* Compute and load the GPIOs masks of the Common lines */
  LCD_ComsMasksInit();
  
}

/**
  * @brief  Configure the nested vectored interrupt controller.
  * @retval
  */
void LCD_InitNVIC(void)
{
  NVIC_InitTypeDef NVIC_InitStructure;
  
  NVIC_PriorityGroupConfig(NVIC_PriorityGroup_2);
  
  NVIC_InitStructure.NVIC_IRQChannel = LCD_TIM_IRQChannel; 
  NVIC_InitStructure.NVIC_IRQChannelPreemptionPriority = LCD_Priority_Value;
  NVIC_InitStructure.NVIC_IRQChannelSubPriority = 1;
  NVIC_InitStructure.NVIC_IRQChannelCmd = ENABLE;
  NVIC_Init(&NVIC_InitStructure);
}


/**
  * @brief  computes the offset to add to the GPIO masks
  * @param Pin_offset: pin offset.
  * @retval : None.
  */
uint32_t LCD_OffsetValue(uint8_t Pin_offset)
{
  uint8_t i;
  uint32_t offset_Value = 1;

  for (i = 0; i < Pin_offset; i++)
    offset_Value =  offset_Value * 16;
    
  return (offset_Value - 1);
}

/**
  * @brief  initilizes the LCD GPIOs masks.
  * @param ne.
  * @retval : None.
  */
void LCD_ComsMasksInit(void)
{
  uint8_t i;
  
  for (i = 0; i < 4; i++)
  {
    CommonLine_VDD_2[i] = (CommonLine_VDD_2[i] << ((LCD_CommonLinesPinOffset) * 4))
                         + LCD_OffsetValue(CommonLines_Pin_Offset);
    CommonLine_OUT_PP[i] = CommonLine_OUT_PP[i] << ((LCD_CommonLinesPinOffset) * 4);
  }
}

/**
  * @brief  Настройка таймера для формирования сигналов на линиях COM
  * @retval 
  */
void LCD_InitTim(void)
{
  TIM_OCInitTypeDef         TIM_OCInitStructure;
  TIM_TimeBaseInitTypeDef   TIM_TimeBaseStructure;  
    
  RCC_APB1PeriphClockCmd( RCC_APB1Periph_TIMx, ENABLE);  // Тестовый ШИМ
  
  TIM_TimeBaseStructure.TIM_Prescaler = 72 - 1;
  TIM_TimeBaseStructure.TIM_Period = 3000;
  TIM_TimeBaseStructure.TIM_ClockDivision = 0;
  TIM_TimeBaseStructure.TIM_CounterMode = TIM_CounterMode_Up;
  
  TIM_TimeBaseInit(LCD_TIMER, &TIM_TimeBaseStructure);

  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_Pulse = 0;   
  TIM_OCInitStructure.TIM_OutputState = ENABLE;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC1Init(LCD_TIMER, &TIM_OCInitStructure);
  
  TIM_OCInitStructure.TIM_OCMode = TIM_OCMode_Toggle;
  TIM_OCInitStructure.TIM_Pulse = LCD_DefaultContrast;
  TIM_OCInitStructure.TIM_OutputState = ENABLE;
  TIM_OCInitStructure.TIM_OCPolarity = TIM_OCPolarity_High;
  TIM_OC2Init(LCD_TIMER, &TIM_OCInitStructure);  
  
  TIM_ITConfig( LCD_TIMER, TIM_IT_CC1 | TIM_IT_CC2 , ENABLE);

  TIM_Cmd( LCD_TIMER, ENABLE);

}

void TIM4_IRQHandler(void)
{
    
  if (TIM_GetITStatus(LCD_TIMER, TIM_IT_CC1) == SET)
  {
    TIM_ClearITPendingBit(LCD_TIMER, TIM_IT_CC1);
    
    // All Segment lines = 0 ( all SegmentsLines_LQD_Port pins = 0 ) 
    LCD_SegmentsPort->ODR = (uint16_t)~ALL_SEGMENT_LINES;
    // All Common lines = 0 
    LCD_CommonLinesPort->ODR &= ~ALL_COMMON_LINES;
    // Configure all Common lines on CommonLines port pins as Out_PP 
    LCD_CommonLinesPort->CRH |= ALL_COMMON_LINES_PP;
    
  }
  
  else if (TIM_GetITStatus(LCD_TIMER, TIM_IT_CC2) == SET)
  {
    TIM_ClearITPendingBit(LCD_TIMER, TIM_IT_CC2);

    if (LCD_PhaseExecute == 0)       
    {
      //temp = Segment_1[LCD_SegmentIndex];
      //temp |= Segment_2[LCD_SegmentIndex] << 1;
      //temp |= Segment_2[LCD_SegmentIndex] << 11;
      
      LCD_SegmentsPort->ODR = LCD_PortVal[LCD_SegmentIndex];
      // Common_line[lcdcr] is set to low 
      LCD_CommonLinesPort->BRR = CommonLine[LCD_SegmentIndex];
      // Other Common lines set to Vdd/2 
      LCD_CommonLinesPort->CRH &= CommonLine_VDD_2[LCD_SegmentIndex];
      // Set Common_line[lcdcr] out push pull 
      LCD_CommonLinesPort->CRH |= CommonLine_OUT_PP[LCD_SegmentIndex];
       
      LCD_PhaseExecute++;
    }
    else 
    {  
     
      //temp = Segment_1[LCD_SegmentIndex];
      //temp |= Segment_2[LCD_SegmentIndex] << 1;
      //temp |= Segment_2[LCD_SegmentIndex] << 11;
      
      LCD_SegmentsPort->ODR = ~LCD_PortVal[LCD_SegmentIndex];
            
      // Common_line[lcdcr] is set to high 
      LCD_CommonLinesPort->BSRR = CommonLine[LCD_SegmentIndex];
     
      // Other Common lines set to Vdd/2 
      LCD_CommonLinesPort->CRH &= CommonLine_VDD_2[LCD_SegmentIndex];
     
      // Other Common lines out push pull 
      LCD_CommonLinesPort->CRH |= CommonLine_OUT_PP[LCD_SegmentIndex];
         
      LCD_SegmentIndex++;
      
      if (LCD_SegmentIndex > 3)
        LCD_SegmentIndex = 0;
     
      LCD_PhaseExecute = 0;
      
    }
    
  }
 
  
}

/******************************* (C) LiteMesh *********************************/