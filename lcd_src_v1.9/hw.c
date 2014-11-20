/******************************************************************************
 *
 * Copyright:
 *    (C) 2006 Embedded Artists AB
 *
 * File:
 *    hw.c
 *
 * Description:
 *    Implements hardware specific routines
 *
 *****************************************************************************/

/******************************************************************************
 * Includes
 *****************************************************************************/
#include "../pre_emptive_os/api/osapi.h"
#include "../pre_emptive_os/api/general.h"
#include <lpc2xxx.h>

#include "hw.h"
#include "key.h"
#include "pins.h"

/******************************************************************************
 * Typedefs and defines
 *****************************************************************************/

/*****************************************************************************
 * Local variables
 ****************************************************************************/
static tU8 greenLedShadow;
static tU8 btResetShadow;

/*****************************************************************************
 * Local prototypes
 ****************************************************************************/

void
immediateIoInit(void)
{
  //make all key signals as inputs
  IODIR &= ~(KEYPIN_CENTER | KEYPIN_UP | KEYPIN_DOWN | KEYPIN_LEFT | KEYPIN_RIGHT);

  IODIR |= BUZZER_PIN;
  IOSET  = BUZZER_PIN;

  IODIR |= BACKLIGHT_PIN;
  IOSET  = BACKLIGHT_PIN;

  IODIR |= LCD_RST;
  IOCLR  = LCD_RST;

  IODIR |= BT_RST;
  IOCLR  = BT_RST;

  IODIR |= (LED_GREEN_PIN | LED_RED_PIN);
  IOSET  = (LED_GREEN_PIN | LED_RED_PIN);
}


/*****************************************************************************
 *
 * Description:
 *    Reset the LCD (by strobing the LCD reset pin)
 *
 ****************************************************************************/
void
resetLCD(void)
{

  IOCLR  = LCD_RST;
  osSleep(2);
  IOSET  = LCD_RST;
  osSleep(5);
}

/*****************************************************************************
 *
 * Description:
 *    Controls the buzzer (on or off)
 *
 ****************************************************************************/
void
setBuzzer(tBool on)
{
  if (TRUE == on)
    IOCLR = BUZZER_PIN;
  else
    IOSET = BUZZER_PIN;
}

/*****************************************************************************
 *
 * Description:
 *    Get current state of joystick switch
 *
 ****************************************************************************/
tU8
getKeys(void)
{
  tU8 readKeys = KEY_NOTHING;


  if ((IOPIN & KEYPIN_CENTER) == 0) readKeys |= KEY_CENTER;
  if ((IOPIN & KEYPIN_UP) == 0)     readKeys |= KEY_UP;
  if ((IOPIN & KEYPIN_DOWN) == 0)   readKeys |= KEY_DOWN;
  if ((IOPIN & KEYPIN_LEFT) == 0)   readKeys |= KEY_LEFT;
  if ((IOPIN & KEYPIN_RIGHT) == 0)  readKeys |= KEY_RIGHT;

  return readKeys;
}


/*****************************************************************************
 *
 * Description:
 *    Select/deselect LCD controller (by controlling chip select signal)
 *
 ****************************************************************************/
void
selectLCD(tBool select)
{
  if (TRUE == select)
    IOCLR = LCD_CS_V1_0;
  else
    IOSET = LCD_CS_V1_0;
}


/*****************************************************************************
 *
 * Description:
 *    Send 9-bit data to LCD controller
 *
 ****************************************************************************/
void
sendToLCD(tU8 firstBit, tU8 data)
{
  //disable SPI
  IOCLR = LCD_CLK;
  PINSEL0 &= 0xffff00ff; //Enable MOSI,MISO and clock and TxD1
  
  if (1 == firstBit)
    IOSET = LCD_MOSI;   //set MOSI
  else
    IOCLR = LCD_MOSI;   //reset MOSI
  
  //Set clock high
  IOSET = LCD_CLK;
  
  //Set clock low
  IOCLR = LCD_CLK;
  
  /*
   * Enable SPI again
   */
  //initialize SPI interface
  SPI_SPCCR = 0x08;    
  SPI_SPCR  = 0x20;

  //connect SPI bus to IO-pins
  PINSEL0 |= 0x00005500;
  
  //send byte
  SPI_SPDR = data;
  while((SPI_SPSR & 0x80) == 0) // wait for datatransfer to be completed
    ;
}


/*****************************************************************************
 *
 * Description:
 *    Initialize the SPI interface for the LCD controller
 *
 ****************************************************************************/
void
initSpiForLcd(void)
{
  //make SPI slave chip select an output and set signal high

  IODIR |= (LCD_CS_V1_0 | LCD_CLK | LCD_MOSI);

  //deselect controller
  selectLCD(FALSE);

  //connect SPI bus to IO-pins
  PINSEL0 |= 0x00005500;
  
  //initialize SPI interface
  SPI_SPCCR = 0x08;    //bus into master state
  SPI_SPCR  = 0x20;    //size of sending data unit
}

