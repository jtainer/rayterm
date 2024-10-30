// 
// Buffer output from shell, map byte sequences to appropriate display operations
//
// 2023, Jonathan Tainer
//

#include "output_parser.h"
#include <stdlib.h>
#include <string.h>
#include <stdio.h>
#include <ctype.h>
#include <sys/param.h>

static const char* escape_table_exact[] = {
	"A",	// Cursor up 1 cell
	"B", 	// Cursor down 1 cell
	"C",	// Cursor forward 1 cell
	"D", 	// Cursor back 1 cell

};

static const char* escape_table_format[] = {
	" %d A",	// Cursor up n cells
	" %d B",	// Cursor down n cells
	" %d C",	// Cursor forward n cells
	" %d D",	// Cursor back n cells
	" %d E",	// Move cursor to beginning of line, n lines down
	" %d F",	// Move cursor to beginning of line, n lines up
	" %d G",	// Move cursor to column n (default 1)
	" %d ; %d H",	// Move cursor to row n, column m
	" %d J",
	"%dK",
	"%dS",
	"%dT",

};

static void esc_seq_reset(output_parser_t* parser) {
	memset(parser->buf, 0, parser->max + 1);
	parser->len = 0;
	parser->in_esc = false;
	parser->esc_fmt = SEQ_NULL;
}

static void esc_seq_append(output_parser_t* parser, char c) {
	if (parser->len < parser->max) {
		parser->buf[parser->len] = c;
		parser->len++;
	}
}

// This will probably be the most complicated part of the entire project
static void esc_seq_parse(output_parser_t* parser) {

}

static ansi_sequence_t parse_ansi_sequence(const char* str) {
	ansi_sequence_t seq = { 0 };
	int n_params = 0;

	if (str == NULL) return seq;

	char byte = 0;
	while (byte = *str) {
		if (isdigit(byte)) {
			// Convert string to integer
			char* endptr = NULL;
			int val = strtol(str, &endptr, 10);
			if (n_params < ANSI_SEQ_MAX_PARAMS) {
				seq.param[n_params] = val;
			}
			// Advance pointer to next non-number character
			str = endptr;
		}
		else if (byte == ';') {
			// Parameters are delimited by semicolons
			// Zero space between semicolons indicates the default value for that parameter
			n_params++;
			str++;
		}
		else if (byte >= 0x40 && byte <= 0x7E) {
			seq.code = byte;
			str++;
		}
		else {
			str++;
		}
	}

	return seq;
}

output_parser_t create_output_parser(int buf_size) {
	output_parser_t parser = { 0 };
	parser.buf = calloc(buf_size+1, 1);
	if (parser.buf) parser.max = buf_size;
	return parser;
}

void destroy_output_parser(output_parser_t* parser) {
	free(parser->buf);
	*parser = (output_parser_t) { 0 };
}

// This should be called each time a byte is read from the output stream
// Text is passed to the display without buffering
// Escape sequences are buffered and parsed internally
void parse_output(output_parser_t* parser, display_t* display, char byte) {

	if (!parser->in_esc) {
		// First character of CSI
		if (byte == '\033') {
			parser->in_esc = true;
			esc_seq_append(parser, byte);
		}

		// Single byte control codes
		else if (byte >= 0x07 && byte <= 0x0D) {
			switch (byte) {
			case 0x07:
				// Bell
				break;
			case 0x08:
				// Backspace
				display_move_cursor(display, (vec2i) { -1, 0 });
				break;
			case 0x09: 
				// Tab
				// TODO: should always align cursor to multiples of 8, not just add 8
				display_move_cursor(display, (vec2i) { 8, 0 });
				break;
			case 0x0A:
				// Line feed
				display_line_feed(display);
				display_carriage_return(display);
				break;
			case 0x0C:
				// Form feed
				break;
			case 0x0D:
				// Carriage return
				display_carriage_return(display);
				break;
			}
		}

		// Display character normally
		else {
			display_print_char(display, byte);
		}
	}
	
	// in_esc == true
	else {
		esc_seq_append(parser, byte);

		// Figure out what kind of escape code it is
		if (parser->esc_fmt == SEQ_NULL) {
			// First byte of escape code
			if (byte == '\033') {
				
			}

			// Second byte of control sequence introducer
			else if (byte == '[') {
				parser->esc_fmt = SEQ_CSI;
			}

			// Second byte of operating system command sequence introducer
			else if (byte == ']') {
				parser->esc_fmt = SEQ_OSC;
			}
		}
		else if (parser->esc_fmt == SEQ_CSI) {
			// Parameter bytes
			if (byte >= 0x30 && byte <= 0x3F) {

			}
			// Intermediate bytes
			else if (byte >= 0x20 && byte <= 0x2F) {

			}
			// Final byte
			else if (byte >= 0x40 && byte <= 0x7E) {
				ansi_sequence_t command = parse_ansi_sequence(parser->buf);

				int n = 0;
				vec2i offset = { 0 };
				vec2i position = { 0 };
				switch (command.code) {
				case 'A':
					// Move cursor up n cells
					n = MAX(1, command.param[0]);
					offset.y = -n;
					display_move_cursor(display, offset);
					break;
				case 'B':
					// Move cursor down n cells
					n = MAX(1, command.param[0]);
					offset.y = n;
					display_move_cursor(display, offset);
					break;
				case 'C':
					// Move cursor right n cells
					n = MAX(1, command.param[0]);
					offset.x = n;
					display_move_cursor(display, offset);
					break;
				case 'D':
					// Move cursor left n cells
					n = MAX(1, command.param[0]);
					offset.x = -n;
					display_move_cursor(display, offset);
					break;
				case 'E':
					// Move cursor to beginning of line, n lines down
					n = MAX(1, command.param[0]);
					offset.y = n;
					display_move_cursor(display, offset);
					display_carriage_return(display);
					break;
				case 'F':
					// Move cursor to beginning of line, n lines up
					n = MAX(1, command.param[0]);
					offset.y = -n;
					display_move_cursor(display, offset);
					display_carriage_return(display);
					break;
				case 'G':
					// Set cursor x position
					// Value of 1 in the command corresponds to index 0
					n = MAX(1, command.param[0]) - 1;
					display_set_cursor_x(display, n);
					break;
				case 'H':
					// Set cursor absolute position
					// Value of 1 in the command corresponds to index 0
					position.x = MAX(1, command.param[0]) - 1;
					position.y = MAX(1, command.param[1]) - 1;
					display_set_cursor(display, position);
					break;
				case 'J':
					// Clear part of the display
					// n == 0 (default): clear from cursor to end of screen
					// n == 1: clear from cursor to beginning of screen
					// n == 2: clear entire screen and move cursor to top left
					// n == 3: clear entire screen and delete scrollback buffer
					if (n == 0) {
						display_clear_down(display);
					}
					else if (n == 1) {
						display_clear_up(display);
					}
					else if (n == 2) {
						position = (vec2i) { 0, 0 };
						display_clear(display);
						display_set_cursor(display, position);
					}
					else if (n == 3) {
						display_clear(display);
					}
					break;
				case 'K':
					if (command.param[0] == 0)
						display_clear_line_forward(display);
					else if (command.param[0] == 1)
						display_clear_line_backward(display);
					else if (command.param[0] == 2)
						display_clear_line(display);
					break;
				}
				
				esc_seq_reset(parser);
			}

		}
		else if (parser->esc_fmt == SEQ_OSC) {
			// End of OSC sequence
			if (byte == 0x07 || strstr(parser->buf, "\033\\")) {
				// Handle escape sequence here

				esc_seq_reset(parser);
			}
		}

	}
}
