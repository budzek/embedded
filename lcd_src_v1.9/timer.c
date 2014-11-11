/*
 * timer.c
 *
 *  Created on: 2014-10-16
 *      Author: embedded
 */

#include "timer.h"
static volatile tU8 shouldStop = FALSE;
static void timerDelayCommon(tU32 mr0)
{
	TIMER1_TCR = 0x02;
	TIMER1_MR0 = mr0;
	TIMER1_IR  = 0xff;
//	TIMER1_MCR = 0x04;
//	0x05 -> 101 interrupt will be thrown
	TIMER1_MCR = 0x05;
	TIMER1_TCR = 0x01;

	//	while (TIMER1_TCR & 0x01);

	shouldStop = FALSE;
	while (shouldStop == FALSE);
}

void timerDelayMs(tU32 time)
{
	timerDelayCommon(time * (FOSC / 100) / 10);
}

void timerDelayUs(tU32 time)
{
	timerDelayCommon(time * (FOSC / 100) / 10000);
}

void initTimer(){
	  //initialize the interrupt vector
	  VICIntSelect &= ~0x00000010;      //Interrupt on MR0: an interrupt is generated when MR0 matches the value in the TC.
	  VICVectCntl7  =  0x00000027; //?
	  VICVectAddr7  =  (tU32)stopTimerDelay;  // address of the ISR
	  VICIntEnable |=  0x00000010;      // TIMER0 interrupt enabled
}

void stopTimerDelay(){
	shouldStop = TRUE;
}
