// 
// Buffer output from shell, map byte sequences to appropriate display operations
//
// 2023, Jonathan Tainer
//

#ifndef OUTPUT_PARSER_H
#define OUTPUT_PARSER_H

#include <stdbool.h>
#include "display.h"

#define ESC_SEQ_MAX_LEN 256

typedef struct output_parser {
	char* buf;
	int max;
	int len;
	bool in_esc = false;
} output_parser_t;

output_parser_t create_output_parser(int buf_size);
void destroy_output_parser(output_parser_t* parser);

// This should be called each time a byte is read from the output stream
// Text is passed to the display without buffering
// Escape sequences are buffered and parsed internally
void parse_output(output_parser_t* parser, display_t* display, char byte);

#endif

