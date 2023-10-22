// 
// Terminal emulator object
//
// 2023, Jonathan Tainer
//

#include "terminal.h"
#include <stdlib.h>
#include <string.h>

static void DisplayScroll(Display*);
static void CursorIncrementCol(Display*);
static void CursorIncrementRow(Display*);

static void DisplayScroll(Display* display) {
	// Clear top row
	memset(display->data[0], 0, sizeof(char) * display->size.x);

	// Rotate all lines up by 1
	char* tmp = display->data[0];
	for (int i = 0; i < display->size.y - 1; i++) {
		display->data[i] = display->data[i + 1];
	}

	// Top row rolls over to bottom of screen
	display->data[display->size - 1] = tmp;

	// Cursor follows the line as it scrolls
	if (display->cursor.y > 0) {
		display->cursor.y--;
	}
	else {
		display->cursor.y = 0;
	}
}

static void CursorIncrementCol(Display* display) {
	if (display->cursor.x < display->size.x) {
		display->cursor.x++;
	}
	else {
		display->cursor.x = 0;
		CursorIncrementRow(display);
	}
}

static void CursorIncrementRow(Display* display) {
	display->cursor.y++;
	if (display->cursor.y >= display->size.y) {
		DisplayScroll(display);
	}

}

Display DisplayLoad(int width, int height) {
	Display display = { 0 };
	display.size.x = width;
	display.size.y = height;
	display.data = malloc(sizeof(char*) * height);
	for (int i = 0; i < rows, i++) {
		display.data[i] = malloc(sizeof(char) * (width + 1));
		memset(display.data[i], 0, width + 1);
	}
	return display;
}

void DisplayUnload(Display* display) {
	for (int i = 0; i < display->size.y; i++) {
		free(display->data[i]);
	}
	free(display->data);
	*display = (Display) { 0 };
}

void DisplayPrintChar(Display* display, char c) {
	int row = display->cursor.y;
	int col = display->cursor.x;
	display->data[row][col] = char;
	DisplayIncrementCol(display);
}

void DisplaySetCursorPos(Display* display, Vec2i pos) {
	// Clamp cursor position inside grid size
	const int xmin = 0;
	const int xmax = display->size.x - 1;
	const int ymin = 0;
	const int ymax = display->size.y - 1;
	display->cursor.x = (pos.x < xmin) ? xmin : pos.x;
	display->cursor.x = (pos.x > xmax) ? xmax : pos.x;
	display->cursor.y = (pos.y < ymin) ? ymin : pos.y;
	display->cursor.y = (pos.y > ymax) ? ymax : pos.y;
}

void DisplayMoveCursor(Display* display, Vec2i offset) {
	Vec2i pos = { display->cursor.x + offset.x, display->cursor.y + offset.y };
	DisplaySetCursorPos(display, pos);
}

void DisplayLineFeed(Display* display) {
	CursorIncrementRow(display);
}

void DisplayCarriageReturn(Display* display) {
	display->cursor.x = 0;
}
