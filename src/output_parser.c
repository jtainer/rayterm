// 
// Buffer output from shell, map byte sequences to appropriate display operations
//
// 2023, Jonathan Tainer
//

#include "output_parser.h"
#include <stdlib.h>
#include <string.h>

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

output_parser_t create_output_parser(int buf_size) {
	output_handler_t parser = { 0 };
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
void parse_output(output_parser_t* parser, char byte) {

	if (!parser->in_esc) {
		// Beginning of escape sequence
		if (byte == '\033') {
			parser->in_esc = true;
			esc_seq_append(parser, byte);
		}

		// Single byte control codes
		else if (byte >= 0x07 && byte <= 0x0D) {
			switch (byte) {
			case 0x07:
				break;
			case 0x08:
				break;
			case 0x09:
				break;
			case 0x0A:
				break;
			case 0x0C:
				break;

			}
		}

		// Display character normally
		else {
			
		}
	}
	
	// in_esc == true
	else {
		esc_seq_append(parser, byte);

		// Control Sequence Introducer
		if (byte == '\033' || byte == '[') {
			
		}

		// Parameter bytes
		else if (byte >= 0x30 && byte <= 0x3F) {
		
		}

		// Intermediate bytes
		else if (byte >= 0x20 && byte <= 0x2F) {
		
		}

		// Final byte
		else if (byte >= 0x40 && byte <= 0x7E) {
			// Process escape sequence
			
			esc_seq_reset(parser);
		}
	}
}
