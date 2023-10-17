// 
// Terminal emulator object
//
// 2023, Jonathan Tainer
//

#ifndef DISPLAY_H
#define DISPLAY_H

typedef struct Vec2i {
	int x, y;
} Vec2i;

typedef struct Display {
	Vec2i size;	// Number of columns and rows (doesnt count null char at end of row)
	Vec2i cursor;
	char** data;	// Access characters by data[row_index][column_index]
} Display;

Display DisplayLoad(int width, int height);

void DisplayUnload(Display* display);

void DisplayPrintChar(Display* display, char c);

void DisplaySetCursorPos(Display* display, Vec2i pos);

void DisplayMoveCursor(Display* display, Vec2i offset);

void DisplayLineFeed(Display* display);

void DisplayCarriageReturn(Display* display);

#endif
