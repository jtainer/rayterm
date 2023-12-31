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
#include <stdio.h>
#include <string.h>
#include <raylib.h>
#include <raymath.h>

#include "input_map.h"

typedef struct vec2i {
	int x;
	int y;
} vec2i;

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

		close(STDIN_FILENO);
		close(STDOUT_FILENO);
		close(STDERR_FILENO);

		dup2(slave, STDIN_FILENO);
		dup2(slave, STDOUT_FILENO);
		dup2(slave, STDERR_FILENO);

		const char cmd[] = "/bin/bash";
		char* const arg[] = { "/bin/bash", NULL };
		execvp(cmd, arg);
		printf("Failed to run shell\n");
		return -1;
	}

	int cols = 192;
	int rows = 54;

	char** line = malloc(sizeof(char*) * rows);
	for (int i = 0; i < rows; i++) {
		line[i] = malloc(cols + 1);
		memset(line[i], 0, cols);
		line[i][cols] = 0;
	}
	
	vec2i cursor_pos = { 0, 0 };
	
	int window_width = 1920;
	int window_height = 1080;
	const char window_title[] = "Terminal Emulator";
	
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_ALWAYS_RUN);
	InitWindow(window_width, window_height, window_title);
	SetTargetFPS(120);

	Font font_ttf = LoadFontEx("fonts/Flexi_IBM_VGA_True.ttf", 20, 0, 250);

	Rectangle char_dim = font_ttf.recs[0];

	while (!WindowShouldClose()) {

		// Send alphanumeric characters to shell
		int keycode = 0;
		while (keycode = GetKeyPressed()) {
			const char* str = get_key_seq(keycode, IsKeyDown(KEY_LEFT_SHIFT));
			const int len = strlen(str);
			printf("keycode = %d\tstr = %s\n", keycode, str);
			write(master, str, len);
		}
		

		char rxbyte = 0;
		int count = 0;
		while ((count = read(master, &rxbyte, 1)) > 0) {
			if (rxbyte == '\n') {
				cursor_pos.y += 1;
			}
			else if (rxbyte == '\r') {
				cursor_pos.x = 0;
			}
			else {
				line[cursor_pos.y][cursor_pos.x] = rxbyte;
				cursor_pos.x += 1;
				if (cursor_pos.x >= cols) {
					cursor_pos.y++;
					cursor_pos.x = 0;
				}
			}
		}

		int cursor_idx = get_pos_idx(cursor_pos, cols);

		BeginDrawing();
		ClearBackground(BLACK);
		DrawRectangle(cursor_pos.x * char_dim.width, cursor_pos.y * char_dim.height, char_dim.width, char_dim.height, WHITE);
		for (int i = 0; i < rows; i++) {
			Vector2 line_origin = { 0, i * char_dim.height };
			DrawTextEx(font_ttf, line[i], line_origin, (float)font_ttf.baseSize, 0, LIME);
		}
		EndDrawing();
	}


	CloseWindow();

	for (int i = 0; i < rows; i++) {
		free(line[i]);
	}
	free(line);

	kill(child_pid, SIGKILL);
	close(master);

	return 0;
}

int get_pos_idx(vec2i pos, int cols) {
	return pos.y * cols + pos.x;
}
