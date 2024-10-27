// 
// rayterm - terminal emulator made with raylib
//
// 2023, Jonathan Tainer
//

#define _XOPEN_SOURCE 600

#include <stdlib.h>
#include <fcntl.h>
#include <signal.h>
#include <unistd.h>
#include <sys/ioctl.h>
#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>

#include "input_map.h"
#include "output_parser.h"
#include "display.h"

int get_pos_idx(vec2i, int);

int main() {

	int master = posix_openpt(O_RDWR | O_NOCTTY | O_NONBLOCK);

	// Create child process to run shell
	int child_pid = fork();
	if (child_pid == -1) { // Failed to create child process
		return(-1);
	}
	if (child_pid == 0) { // Child process

		// Connect to slave terminal device
		unlockpt(master);
		grantpt(master);
		int slave = open(ptsname(master), O_RDWR | O_NONBLOCK);
		if (slave < 0) {
			return 0;
		}

		// Create a new session (has no controlling terminal)
		setsid();
		
		// Set the pseudoterminal to act as the controlling terminal
		ioctl(slave, TIOCSCTTY, 0);

		// Redirect STDIN, STDOUT, and STDERR to the pseudoterminal
		// Each of the file descriptors being redirected are implicitly closed by dup2
		dup2(slave, STDIN_FILENO);
		dup2(slave, STDOUT_FILENO);
		dup2(slave, STDERR_FILENO);

		const char cmd[] = "/bin/bash";
		char* const arg[] = { "/bin/bash", NULL };
		execvp(cmd, arg);
		printf("Failed to run shell\n");
		return -1;
	}

	// Create text display buffer
	int cols = 192;
	int rows = 54;
	display_t display = display_load(cols, rows);

	// Initialize output parser (handles escape sequences etc)
	output_parser_t output_parser = create_output_parser(256);

	// Initialize raylib for keyboard input and rendering
	int window_width = 1920;
	int window_height = 1080;
	const char window_title[] = "Terminal Emulator";
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_ALWAYS_RUN);
	InitWindow(window_width, window_height, window_title);
	SetTargetFPS(120);

	// Chose a monospace font that looks nice, need to make this configurable in the future
	Font font_ttf = LoadFontEx("fonts/Flexi_IBM_VGA_True.ttf", 20, 0, 250);
	Rectangle char_dim = font_ttf.recs[0];

	while (!WindowShouldClose()) {

		// Map key presses to the corresponding characters or control sequences
		int keycode = 0;
		while (keycode = GetKeyPressed()) {
			const char* str = get_key_seq(keycode, IsKeyDown(KEY_LEFT_SHIFT));
			const int len = strlen(str);
			printf("keycode = %d\tstr = %s\n", keycode, str);

			// Send remapped inputs to the shell
			write(master, str, len);
		}
		
		// Read in any data received from the shell and update the text display buffer
		char rxbyte = 0;
		int count = 0;
		while ((count = read(master, &rxbyte, 1)) > 0) {
			parse_output(&output_parser, &display, rxbyte);
		}

		// Render everything
		BeginDrawing();
		ClearBackground(BLACK);
		// Draw cursor
		DrawRectangle(
			display.cursor.x * char_dim.width,
			display.cursor.y * char_dim.height,
			char_dim.width, char_dim.height, WHITE
		);
		// Draw the text buffer
		for (int i = 0; i < rows; i++) {
			Vector2 line_origin = { 0, i * char_dim.height };
			DrawTextEx(font_ttf, display.data[i], line_origin, (float)font_ttf.baseSize, 0, LIME);
		}
		EndDrawing();
	}

	display_unload(&display);
	destroy_output_parser(&output_parser);

	CloseWindow();

	kill(child_pid, SIGKILL);
	close(master);

	return 0;
}

int get_pos_idx(vec2i pos, int cols) {
	return pos.y * cols + pos.x;
}
