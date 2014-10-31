/*
 * buzzer.c
 *
 *  Created on: 2014-10-16
 *      Author: embedded
 */

#include "buzzer.h"
#include "hw.h"
#include "timer.h"
#include "../pre_emptive_os/api/osapi.h"
#include "../pre_emptive_os/api/general.h"

static volatile tU8 shouldPlay = FALSE;
#define SOUNDPROC_STACK_SIZE 300

static tU8 soundProcStack[SOUNDPROC_STACK_SIZE];
static tU8 soundProcPid;

static tU32 notes[] = {
        2272, // A - 440 Hz
        2024, // B - 494 Hz
        3816, // C - 262 Hz
        3401, // D - 294 Hz
        3030, // E - 330 Hz
        2865, // F - 349 Hz
        2551, // G - 392 Hz
        1136, // a - 880 Hz
        1012, // b - 988 Hz
        1912, // c - 523 Hz
        1703, // d - 587 Hz
        1517, // e - 659 Hz
        1432, // f - 698 Hz
        1275, // g - 784 Hz
};

static const char *song = "A1,B2,c2,A3,A1,A2,B2,A2,F2,G4,G2,A2,G2,";

static void playNote(tU32 note, tU32 durationMs) {
    tU32 t = 0;
    if (note > 0) {
        while (t < (durationMs*1000)) {
        	setBuzzer(TRUE);
            timerDelayUs(note / 2);
            setBuzzer(FALSE);
            timerDelayUs(note / 2);
            t += note;
        }
    }
    else {
        timerDelayMs(durationMs);
    }
}

static tU32 getNote(tU8 ch)
{
    if (ch >= 'A' && ch <= 'G')
        return notes[ch - 'A'];

    if (ch >= 'a' && ch <= 'g')
        return notes[ch - 'a' + 7];

    return 0;
}

static tU32 getDuration(tU8 ch)
{
    if (ch < '0' || ch > '9')
        return 400;

    /* number of ms */
    return (ch - '0') * 200;
}

static tU32 getPause(tU8 ch)
{
    switch (ch) {
    case '+':
        return 0;
    case ',':
        return 5;
    case '.':
        return 20;
    case '_':
        return 30;
    default:
        return 5;
    }
}

static void playSong(tU8 *song) {
    tU32 note = 0;
    tU32 dur  = 0;
    tU32 pause = 0;

    /*
     * A song is a collection of tones where each tone is
     * a note, duration and pause, e.g.
     *
     * "E2,F4,"
     */

    while(*song != '\0') {
        note = getNote(*song++);
        if (*song == '\0')
            break;

        dur  = getDuration(*song++);

        if (*song == '\0')
            break;

        pause = getPause(*song++);

        playNote(note, dur);
        timerDelayMs(pause);
    }
}

void playMelody(void)
{
	shouldPlay = TRUE;
}

static void
procSound(void* arg)
{
  while(1)
  {
	if (shouldPlay == TRUE) {
		shouldPlay = FALSE;
		playSong(song);
	}
    osSleep(20);
  }
}

void
initSoundProc(void)
{
  tU8 error;

  osCreateProcess(procSound, soundProcStack, SOUNDPROC_STACK_SIZE, &soundProcPid, 3, NULL, &error);
  osStartProcess(soundProcPid, &error);
}


