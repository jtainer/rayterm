// 
// Buffer output from shell, map byte sequences to appropriate display operations
//
// 2023, Jonathan Tainer
//

#include "output_handler.h"
#include <string.h>

// Static buffer for escape sequences
// Only buffers output after the beginning of an escape sequence is detected
#define ESC_SEQ_MAX_LEN 256
static char esc_seq[ESC_SEQ_MAX_LEN+1] = { 0 };
static int esc_seq_len = 0;
static bool in_esc = false;

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

static void esc_seq_reset(output_handler_t* handler) {
	memset(handler->esc_seq, 0, ESC_SEQ_MAX_LEN + 1);
	handler->esc_seq_len = 0;
	handler->in_esc = false;
}

static void esc_seq_append(output_handler_t* handler, char c) {
	if (handler->esc_seq_len < ESC_SEQ_MAX_LEN) {
		handler->esc_seq[handler->esc_seq_len] = c;
		handler->esc_seq_len++;
	}
}

// This will probably be the most complicated part of the entire project
static void esc_seq_parse(output_handler_t* handler) {

}

// This should be called each time a byte is read from the output stream
// Text is passed to the display without buffering
// Escape sequences are buffered and parsed internally
void process_output(char byte) {

	if (!in_esc) {
		// Beginning of escape sequence
		if (byte == '\033') {
			in_esc = true;
			esc_seq_append(byte);
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
		esc_seq_append(byte);

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
			
			esc_seq_reset();
		}
	}
}
