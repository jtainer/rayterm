// 
// Buffer output from shell, map byte sequences to appropriate display operations
//
// 2023, Jonathan Tainer
//

#include <stdint.h>

// Store shell output in circular buffer
#define SHELL_MSG_LEN_MAX	256
static char shell_msg_buf[SHELL_MSG_LEN_MAX+1] = { 0 };
static int shell_msg_len = 0;

typedef enum msg_state { MSG_LITERAL = 0, MSG_ESCAPE = 1 } msg_state;
static msg_state shell_msg_state = MSG_LITERAL;

static const char* escape_table_exact[] = {
	"A",	// Cursor up 1 cell
	"B", 	// Cursor down 1 cell
	"C",	// Cursor forward 1 cell
	"D", 	// Cursor back 1 cell

};

static const char* escape_table_format[] = {
	"%dA",	// Cursor up n cells
	"%dB",	// Cursor down n cells
	"%dC",	// Cursor forward n cells
	"%dD",	// Cursor back n cells
	"%dE",	// Move cursor to beginning of line, n lines down
	"%dF",	// Move cursor to beginning of line, n lines up
	"%dG",	//
	"%dH",
	"%dJ",
	"%dK",
	"%dS",
	"%dT",

};

void shell_disp_byte(char byte) {

	if (shell_msg_state == MSG_LITERAL) {
		
		// Beginning of escape sequence
		if (byte == '\033') {
			shell_msg_state = MSG_ESCAPE;
			memset(shell_msg_buf, 0, SHELL_MSG_LEN_MAX);
			shell_msg_len = 0;
		}

		// Single byte control codes
		else if (byte >= 0x07 && byte <= 0x1B) {
			
		}

		// Display character normally
		else {
			
		}
	}
	
	if (shell_msg_state == MSG_ESCAPE) {

		shell_msg_buf[shell_msg_len] = byte;
		shell_msg_len++;

		// CSI
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
			
		}
	}
}
