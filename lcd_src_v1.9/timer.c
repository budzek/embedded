/*
 * timer.c
 *
 *  Created on: 2014-10-16
 *      Author: embedded
 */

#include "timer.h"

static void timerDelayCommon(tU32 mr0)
{
	TIMER1_TCR = 0x02;
	TIMER1_MR0 = mr0;
	TIMER1_IR  = 0xff;
	TIMER1_MCR = 0x04;
	TIMER1_TCR = 0x01;

	while (TIMER1_TCR & 0x01);
}

void timerDelayMs(tU32 time)
{
	timerDelayCommon(time * (FOSC / 100) / 10);
}

void timerDelayUs(tU32 time)
{
	timerDelayCommon(time * (FOSC / 100) / 10000);
}
