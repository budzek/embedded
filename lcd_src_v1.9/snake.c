#include "../pre_emptive_os/api/osapi.h"
#include "../pre_emptive_os/api/general.h"
#include <printf_P.h>
#include <ea_init.h>
#include <stdlib.h>
#include "lcd.h"
#include "key.h"
#include "buzzer.h"

#define ROWS 27
#define COLUMNS 31

static tS8 board[ROWS][COLUMNS];
static tS8 direction;
static tS32 lengthOfSnake;
static tS32 speed;

struct snakePart {
	tS32 row;
	tS32 column;
} snake[100];

extern volatile tU32 ms;

static void addNewHead();
static void initializeGame();
static void drawBlock(tU8 x, tU8 y, tU8 color);
static void addFood();
static void removeTail();
static void drawBoard();
static tS8 collisionWithSnake();
static tS8 collisionWithFood();

#define START_COLUMN 15
#define START_ROW  7
#define PAUSE_LENGTH 2

void playSnake(void) {
	tU8 joystick;

	while (1) {
		initializeGame();
		speed = 0;

		while (1) {
			tS32 i;
			osSleep((10 - speed) * PAUSE_LENGTH);

			joystick = checkKey();
			if ((joystick == KEY_UP) || (joystick == KEY_RIGHT) || (joystick == KEY_DOWN) || (joystick == KEY_LEFT)) {
				direction = joystick;
			}

			addNewHead();
			removeTail();

			for (i = 0; i <= lengthOfSnake; i++) {
				drawBlock(snake[i].column, snake[i].row, 0xfc);
			}

			if (collisionWithSnake()) {
				osSleep(500);
				break;
			}
			if (collisionWithFood()) {
				// remove food
				board[snake[lengthOfSnake - 1].row][snake[lengthOfSnake - 1].column] = ' ';
				playMelody();
				lengthOfSnake++;
				addNewHead();
				addFood();

				if (lengthOfSnake % 3 == 0 && (speed < 9)) {
					speed++;
				}
			}
		}
	}
}

void initializeGame() {
	tS32 row, column, i;
	srand(ms);

	lengthOfSnake = 4;
	direction = KEY_RIGHT;

	printf("ROWS = ");
	printf("%d", ROWS);
	printf(" COLUMNS = ");
	printf("%d", COLUMNS);

	drawBoard();

	for (row = 0; row < ROWS; row++)
		for (column = 0; column < COLUMNS; column++)
			board[row][column] = ' ';

	addFood();

	for (i = 0; i < lengthOfSnake; i++) {
		snake[i].row = START_ROW;
		snake[i].column = START_COLUMN + i;
	}

	for (row = 0; row < ROWS; row++) {
		for (column = 0; column < COLUMNS; column++) {
			if (board[row][column] == ' ') {
				drawBlock(column, row, 0);
			}
			else {
				drawBlock(column, row, 0x1c);
			}
		}
	}
}

void addNewHead() {
	if (direction == KEY_RIGHT) {
		snake[lengthOfSnake].row = snake[lengthOfSnake - 1].row;
		snake[lengthOfSnake].column = snake[lengthOfSnake - 1].column + 1;
	}
	else if (direction == KEY_LEFT) {
		snake[lengthOfSnake].row = snake[lengthOfSnake - 1].row;
		snake[lengthOfSnake].column = snake[lengthOfSnake - 1].column - 1;
	}
	else if (direction == KEY_DOWN) {
		snake[lengthOfSnake].row = snake[lengthOfSnake - 1].row + 1;
		snake[lengthOfSnake].column = snake[lengthOfSnake - 1].column;
	}
	else if (direction == KEY_UP) {
		snake[lengthOfSnake].row = snake[lengthOfSnake - 1].row - 1;
		snake[lengthOfSnake].column = snake[lengthOfSnake - 1].column;
	}

	if (snake[lengthOfSnake].row >= ROWS) {
		printf("\n snake[%d].row >= %d %s", lengthOfSnake-1, snake[lengthOfSnake-1].row, " >= ROWS");
		snake[lengthOfSnake].row = 0;
	}
	if (snake[lengthOfSnake].row < 0) {
		printf("\n snake[%d].row >= %d %s", lengthOfSnake, snake[lengthOfSnake-1].row, " < 0");
		snake[lengthOfSnake].row = ROWS - 1;
	}

	if (snake[lengthOfSnake].column >= COLUMNS) {
		printf("\n snake[%d].column >= %d %s", lengthOfSnake, snake[lengthOfSnake-1].column, " >= COLUMNS");
		snake[lengthOfSnake].column = 0;
	}
	if (snake[lengthOfSnake].column < 0) {
		printf("\n snake[%d].column >= %d %s", lengthOfSnake, snake[lengthOfSnake-1].column, " < 0");
		snake[lengthOfSnake].column = COLUMNS - 1;
	}
}

void removeTail() {
	tS32 i;
	drawBlock(snake[0].column, snake[0].row, 0);
	for (i = 1; i <= lengthOfSnake; i++) {
		snake[i - 1] = snake[i];
	}
}

void drawBoard() {
	lcdColor(0, 0xe0);
	lcdClrscr();

	lcdGotoxy(42, 0);
	lcdPuts("Snake");

	lcdRect(0, 14, (4 * COLUMNS) + 4, (4 * ROWS) + 4, 3);
	lcdRect(2, 16, 4 * COLUMNS, 4 * ROWS, 1);
}


void drawBlock(tU8 xCoordinate, tU8 yCoordinate, tU8 color) {
	lcdRect(2 + (xCoordinate * 4), 16 + (yCoordinate * 4), 4, 4, color);
}

void addFood() {
	printf("***addFood started\n");

	unsigned char ok = 0;

	int row, column;
	while (!ok) {
		ok = 1;

		row = rand() % ROWS;
		column = rand() % COLUMNS;
		int i = 0;

		//check if snake is in place of food, if so run again to find empty place for food
		for (i = 0; i < lengthOfSnake; i++) {
			if (snake[i].row == row && snake[i].column == column) {
				ok = 0;
			}
		}
	}

	board[row][column] = '.';
	drawBlock(column, row, 0x1c);

	printf("Food added at [%d][%d]\n", row, column);
	printf("***addFood ended\n");

}

tS8 collisionWithSnake() {
	tS32 i;
	for (i = 0; i < lengthOfSnake - 1; i++) {
		if ((snake[lengthOfSnake - 1].row) == (snake[i].row) && (snake[lengthOfSnake - 1].column) == (snake[i].column)) {
			return 1;
		}
	}
	return 0;
}

tS8 collisionWithFood() {
	if (board[snake[lengthOfSnake - 1].row][snake[lengthOfSnake - 1].column] == '.') {
		return 1;
	}
	else {
		return 0;
	}
}

