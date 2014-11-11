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
	shouldStop = FALSE;

	TIMER1_TCR = 0x02; // reset timer
	TIMER1_MR0 = mr0;
	TIMER1_IR  = 0xff; // reset all flags before enable IRQs
	TIMER1_MCR = 0x05; // on match: generate interrupt and stop

	VICIntSelect &= ~0x20; // assign interrupt request 5 to IRQ category
						   // 1 - FIQ
						   // 0 - IRQ
	VICIntEnable |=  0x20; // enable interrupt request 5
	VICVectAddr5  = (tU32)stopTimerDelay;
	VICVectCntl5  = 0x25; // enable interrupt slot 5 and assign interrupt request 5 to it

	TIMER1_TCR = 0x01; // enable timer

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

void stopTimerDelay(){
	shouldStop = TRUE;

	TIMER1_IR = 0xff;        //reset all IRQ flags
	VICVectAddr = 0x00;        //dummy write to signal end of interrupt
}
