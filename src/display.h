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
	// Number of visible columns and rows indicated by size.
	// Row strings are actually 1 byte longer than size.x to store null char.
	Vec2i size;
	
	// Cursor position in character grid
	Vec2i cursor;
	
	// Characters are accessed by data[row_idx][col_idx]
	char** data;
} Display;

Display DisplayLoad(int width, int height);

void DisplayUnload(Display* display);

void DisplayPrintChar(Display* display, char c);

void DisplaySetCursorPos(Display* display, Vec2i pos);

void DisplayMoveCursor(Display* display, Vec2i offset);

void DisplayLineFeed(Display* display);

void DisplayCarriageReturn(Display* display);

#endif
