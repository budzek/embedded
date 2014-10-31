#include "../pre_emptive_os/api/osapi.h"
#include "../pre_emptive_os/api/general.h"
#include <printf_P.h>
#include <ea_init.h>
#include <stdlib.h>
#include "lcd.h"
#include "key.h"
#include "buzzer.h"

#define MAXROW 27
#define MAXCOL 31

#define SNAKE_START_COL 15
#define SNAKE_START_ROW  7
#define PAUSE_LENGTH     2

static void addSegment();
static void setupLevel();
static void gotoxy(tU8 x, tU8 y, tU8 color);
static void addFood();

static tS32 snakeLength;
static tS32 speed;
static tS8 screenGrid[MAXROW][MAXCOL];
static tS8 direction = KEY_RIGHT;

struct snakeSegment {
	tS32 row;
	tS32 col;
} snake[100];

extern volatile tU32 ms;

void playSnake(void) {
	tU8 keypress;

	while (1) {
		setupLevel();
		speed = 14;

		do {
			tS32 i;
			osSleep(speed * PAUSE_LENGTH);

			keypress = checkKey();
			if (keypress != KEY_NOTHING) {
				if ((keypress == KEY_UP) || (keypress == KEY_RIGHT)
						|| (keypress == KEY_DOWN) || (keypress == KEY_LEFT))
					direction = keypress;
			}

			addSegment();

			//removed last segment of snake
			gotoxy(snake[0].col, snake[0].row, 0);

			//remove last segment from the array
			for (i = 1; i <= snakeLength; i++)
				snake[i - 1] = snake[i];

			//display snake in yellow
			for (i = 0; i <= snakeLength; i++)
				gotoxy(snake[i].col, snake[i].row, 0xfc);

			//collision detection - snake (bad!)
			for (i = 0; i < snakeLength - 1; i++)
				if ((snake[snakeLength - 1].row) == (snake[i].row)
						&& (snake[snakeLength - 1].col) == (snake[i].col)) {
					keypress = KEY_CENTER; //exit loop - game over
					osSleep(500);
					break;
				}

			//collision detection - food (good!)
			if (screenGrid[snake[snakeLength - 1].row][snake[snakeLength - 1].col]
					== '.') {
				screenGrid[snake[snakeLength - 1].row][snake[snakeLength - 1].col] =' ';

				playMelody();
				snakeLength++;
				addSegment();
				addFood();

				//if length of snake reaches certain size, increase speed
				if (snakeLength % 3 == 0 && (speed > 1)) {
					speed--;
				}
			}
		} while (keypress != KEY_CENTER);
	}
}

/*****************************************************************************
 *
 * Description:
 *    Initialize one level of the game. Draw game board.
 *
 ****************************************************************************/
void setupLevel() {
	tS32 row, col, i;
	srand(ms);

	printf("MAXROW = ");
	printf("%d", MAXROW);
	printf(" MAXCOL = ");
	printf("%d", MAXCOL);

	//clear screen
	lcdColor(0, 0xe0);
	lcdClrscr();

	//draw frame
	lcdGotoxy(42, 0);
	lcdPuts("Snake");

	//draw game board rectangle
	lcdRect(0, 14, (4 * MAXCOL) + 4, (4 * MAXROW) + 4, 3);
	lcdRect(2, 16, 4 * MAXCOL, 4 * MAXROW, 1);

	//set up global variables for new level
	snakeLength = 4;
	direction = KEY_RIGHT;

	//fill grid with blanks
	for (row = 0; row < MAXROW; row++)
		for (col = 0; col < MAXCOL; col++)
			screenGrid[row][col] = ' ';

	//fill grid with food
	addFood();

	//create snake array of length snakeLength
	for (i = 0; i < snakeLength; i++) {
		snake[i].row = SNAKE_START_ROW;
		snake[i].col = SNAKE_START_COL + i;
	}

	//draw game board
	for (row = 0; row < MAXROW; row++) {
		for (col = 0; col < MAXCOL; col++) {
			switch (screenGrid[row][col]) {
			case ' ':
				gotoxy(col, row, 0);
				break;
			case '.':
				gotoxy(col, row, 0x1c);
				break;
			default:
				break;
			}
		}
	}

}

void addSegment() {
	switch (direction) {
	case (KEY_RIGHT):
		printf("KEY_RIGHT\n");
		snake[snakeLength].row = snake[snakeLength - 1].row;
		snake[snakeLength].col = snake[snakeLength - 1].col + 1;
		break;
	case (KEY_LEFT):
printf("KEY_LEFT\n");

		snake[snakeLength].row = snake[snakeLength - 1].row;
		snake[snakeLength].col = snake[snakeLength - 1].col - 1;
		break;
	case (KEY_UP):
printf("KEY_UP\n");

		snake[snakeLength].row = snake[snakeLength - 1].row - 1;
		snake[snakeLength].col = snake[snakeLength - 1].col;
		break;
	case (KEY_DOWN):
printf("KEY_DOWN\n");

		snake[snakeLength].row = snake[snakeLength - 1].row + 1;
		snake[snakeLength].col = snake[snakeLength - 1].col;
	}

	//TODO remove equality ??
	if (snake[snakeLength].row >= MAXROW) {
		//  if (snake[snakeLength].row >= MAXROW){
		printf("\n snake[%d].row >= %d %s", snakeLength-1, snake[snakeLength-1].row, " >= MAXROW");
		snake[snakeLength].row = 0;
	}
	if (snake[snakeLength].row < 0) {
		printf("\n snake[%d].row >= %d %s", snakeLength, snake[snakeLength-1].row, " < 0");
		snake[snakeLength].row = MAXROW - 1;
	}
	//  if (snake[snakeLength].col >= MAXCOL){
	if (snake[snakeLength].col >= MAXCOL) {
		printf("\n snake[%d].col >= %d %s", snakeLength, snake[snakeLength-1].col, " >= MAXCOL");
		snake[snakeLength].col = 0;
	}
	if (snake[snakeLength].col < 0) {
		printf("\n snake[%d].col >= %d %s", snakeLength, snake[snakeLength-1].col, " < 0");
		snake[snakeLength].col = MAXCOL - 1;
	}
}

void gotoxy(tU8 x, tU8 y, tU8 color) {
	lcdRect(2 + (x * 4), 16 + (y * 4), 4, 4, color);
}

void addFood() {
	printf("***addFood started\n");

	unsigned char ok =0;
	unsigned char add =1;

	int row, col;
	while (!ok) {
		ok =1;
		printf("***addFood while looped\n");

		row = rand() % MAXROW;
		col = rand() % MAXCOL;
		int i = 0;
		//check if snake is in place of food, if so run again to find empty place for food
		for (i = 0; i < snakeLength; i++) {
			if (snake[i].row == row && snake[i].col == col) {
				ok =0;
			}
		}

	}
//	int row2, col2;
//	for (row2 = 0; row2 < MAXROW; row2++)
//		for (col2 = 0; col2 < MAXCOL; col2++)
//			if(screenGrid[row2][col2] == '.'){
//				printf("Food at position [%d][%d]\n", row2, col2);
//				add = 0;
//			}
//	if(add){
	screenGrid[row][col] = '.'; //= food
	gotoxy(col, row, 0x1c);
//	}
	printf("Food added at [%d][%d]\n", row, col);
	printf("***addFood ended\n");

}

