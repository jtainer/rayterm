// 
// Character display for terminal emulator
//
// 2023, Jonathan Tainer
//

#include "display.h"
#include <stdlib.h>
#include <string.h>

static void display_scroll(display_t*);
static void cursor_increment_col(display_t*);
static void cursor_increment_row(display_t*);

static void display_scroll(display_t* display) {
	// Clear top row
	memset(display->data[0], 0, sizeof(char) * display->size.x);

	// Rotate all lines up by 1
	char* tmp = display->data[0];
	for (int i = 0; i < display->size.y - 1; i++) {
		display->data[i] = display->data[i + 1];
	}

	// Top row rolls over to bottom of screen
	display->data[display->size.y - 1] = tmp;

	// Cursor follows the line as it scrolls
	if (display->cursor.y > 0) {
		display->cursor.y--;
	}
	else {
		display->cursor.y = 0;
	}
}

static void cursor_increment_col(display_t* display) {
	if (display->cursor.x < display->size.x) {
		display->cursor.x++;
	}
	else {
		display->cursor.x = 0;
		cursor_increment_row(display);
	}
}

static void cursor_increment_row(display_t* display) {
	display->cursor.y++;
	if (display->cursor.y >= display->size.y) {
		display_scroll(display);
	}

}

display_t display_load(int width, int height) {
	display_t display = { 0 };
	display.size.x = width;
	display.size.y = height;
	display.data = malloc(sizeof(char*) * height);
	for (int i = 0; i < height; i++) {
		display.data[i] = malloc(sizeof(char) * (width + 1));
		memset(display.data[i], 0, width + 1);
	}
	return display;
}

void display_unload(display_t* display) {
	for (int i = 0; i < display->size.y; i++) {
		free(display->data[i]);
	}
	free(display->data);
	*display = (display_t) { 0 };
}

void display_print_char(display_t* display, char c) {
	int row = display->cursor.y;
	int col = display->cursor.x;
	display->data[row][col] = c;
	cursor_increment_col(display);
}

void display_set_cursor(display_t* display, vec2i pos) {
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

void display_move_cursor(display_t* display, vec2i offset) {
	vec2i pos = { display->cursor.x + offset.x, display->cursor.y + offset.y };
	display_set_cursor(display, pos);
}

void display_line_feed(display_t* display) {
	cursor_increment_row(display);
}

void display_carriage_return(display_t* display) {
	display->cursor.x = 0;
}

