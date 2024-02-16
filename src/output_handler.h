// 
// Buffer output from shell, map byte sequences to appropriate display operations
//
// 2023, Jonathan Tainer
//

#ifndef OUTPUT_HANDLER_H
#define OUTPUT_HANDLER_H

#include <stdbool.h>
#include "display.h"

#define ESC_SEQ_MAX_LEN 256

typedef struct output_handler {
	char esc_seq[ESC_SEQ_MAX_LEN+1];
	int esc_seq_len;
	bool in_esc = false;
} output_handler_t;

output_handler_t create_output_handler();
void destroy_output_handler(output_handler_t* handler);

// This should be called each time a byte is read from the output stream
// Text is passed to the display without buffering
// Escape sequences are buffered and parsed internally
void process_output(output_handler_t* handler, char byte);

#endif

