// 
// Character display for terminal emulator
//
// 2023, Jonathan Tainer
//

#include "display.h"
#include <stdlib.h>
#include <string.h>
#include <sys/param.h>

static void display_scroll(display_t*);
static void cursor_increment_col(display_t*);
static void cursor_increment_row(display_t*);

static void display_scroll(display_t* display) {
	// Clear top row
	memset(display->data[0], ' ', sizeof(char) * display->size.x);

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
		memset(display.data[i], ' ', width + 1);
		display.data[i][width] = 0;
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
	pos.x = (pos.x < xmin) ? xmin : pos.x;
	pos.x = (pos.x > xmax) ? xmax : pos.x;
	pos.y = (pos.y < ymin) ? ymin : pos.y;
	pos.y = (pos.y > ymax) ? ymax : pos.y;
	display->cursor.x = pos.x;
	display->cursor.y = pos.y;
}

void display_move_cursor(display_t* display, vec2i offset) {
	vec2i pos = { display->cursor.x + offset.x, display->cursor.y + offset.y };
	display_set_cursor(display, pos);
}

void display_set_cursor_x(display_t* display, int x) {
	x = MAX(x, 0);
	x = MIN(x, display->size.x - 1);
	display->cursor.x = x;
	
}

void display_set_cursor_y(display_t* display, int y) {
	y = MAX(y, 0);
	y = MIN(y, display->size.y - 1);
	display->cursor.y = y;
}

void display_line_feed(display_t* display) {
	cursor_increment_row(display);
}

void display_carriage_return(display_t* display) {
	display->cursor.x = 0;
}

void display_clear_line(display_t* display) {
	int row = display->cursor.y;
	int col = display->cursor.x;
	memset(display->data[row], ' ', sizeof(char) * display->size.x);
}

void display_clear_line_forward(display_t* display) {
	int row = display->cursor.y;
	int col = display->cursor.x;
	memset(display->data[row] + col, ' ', sizeof(char) * (display->size.x - col));
}

void display_clear_line_backward(display_t* display) {
	int row = display->cursor.y;
	int col = display->cursor.x;
	memset(display->data[row], ' ', sizeof(char) * (col + 1));
}

void display_clear(display_t* display) {
	for (int i = 0; i < display->size.y; i++) {
		memset(display->data[i], 0, sizeof(char) * display->size.x);
	}
}

void display_clear_down(display_t* display) {
	for (int i = display->cursor.y; i < display->size.y; i++) {
		memset(display->data[i], 0, sizeof(char) * display->size.x);
	}
}

void display_clear_up(display_t* display) {
	for (int i = 0; i < display->size.y; i++) {
		memset(display->data[i], 0, sizeof(char) * display->size.x);
	}
}

