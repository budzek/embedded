/*
 * timer.h
 *
 *  Created on: 2014-10-16
 *      Author: embedded
 */

#ifndef TIMER_H_
#define TIMER_H_

#include "../pre_emptive_os/api/general.h"
#include <lpc2xxx.h>
#include <config.h>

void timerDelayMs(tU32 time);
void timerDelayUs(tU32 time);

#endif /* TIMER_H_ */
