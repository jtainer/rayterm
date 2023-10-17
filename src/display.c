// 
// Terminal emulator object
//
// 2023, Jonathan Tainer
//

#include "terminal.h"
#include <stdlib.h>
#include <string.h>

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

}

void DisplaySetCursorPos(Display* display, Vec2i pos) {

}

void DisplayMoveCursor(Display* display, Vec2i offset) {

}

void DisplayLineFeed(Display* display) {

}

void DisplayCarriageReturn(Display* display) {

}
