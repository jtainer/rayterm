// 
// Character display for terminal emulator
//
// 2023, Jonathan Tainer
//

#ifndef DISPLAY_H
#define DISPLAY_H

typedef struct vec2i {
	int x, y;
} vec2i;

typedef struct display {
	// Number of visible columns and rows indicated by size.
	// Row strings are actually 1 byte longer than size.x to store null char.
	vec2i size;
	
	// Cursor position in character grid
	vec2i cursor;
	
	// Characters are accessed by data[row_idx][col_idx]
	char** data;
} display_t;

display_t display_load(int width, int height);

void display_unload(display_t* display);

void display_print_char(display_t* display, char c);

void display_set_cursor(display_t* display, vec2i pos);

void display_move_cursor(display_t* display, vec2i offset);

void display_line_feed(display_t* display);

void display_carriage_return(display_t* display);

#endif
