// 
// rayterm - terminal emulator made with raylib
//
// 2023, Jonathan Tainer
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <signal.h>
#include <poll.h>
#include <raylib.h>
#include <raymath.h>

typedef struct vec2i {
	int x;
	int y;
} vec2i;

int get_pos_idx(vec2i, int);

int main() {

	// Create pipes to redirect stdin and stdout of shell
	int infd[2];
	int outfd[2];
	pipe(infd);
	pipe(outfd);

	// Create child process to run shell
	int child_pid = fork();
	if (child_pid == -1) { // Failed to create child process
		return(-1);
	}
	if (child_pid == 0) { // Child process
		close(STDOUT_FILENO);
		close(STDIN_FILENO);
		dup2(outfd[0], STDIN_FILENO);
		dup2(infd[1], STDOUT_FILENO);
		close(infd[0]);
		close(infd[1]);
		close(outfd[0]);
		close(outfd[1]);

		const char cmd[] = "bash";
		char* const arg[] = { "bash", "\0" };
		execvp(cmd, arg);
		return -1;
	}

	// Parent process
//	close(outfd[0]);
//	close(infd[1]);

	int cols = 64;
	int rows = 24;

	char** line = malloc(sizeof(char*) * rows);
	for (int i = 0; i < rows; i++) {
		line[i] = malloc(cols + 1);
		memset(line[i], ' ', cols);
		line[i][cols] = 0;
	}
	
	vec2i cursor_pos = { 0, 0 };
	
	int window_width = 640;
	int window_height = 480;
	const char window_title[] = "Terminal Emulator";
	
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_ALWAYS_RUN);
	InitWindow(window_width, window_height, window_title);
	SetTargetFPS(60);

	Font font_ttf = LoadFontEx("fonts/Flexi_IBM_VGA_True.ttf", 20, 0, 250);

	Rectangle char_dim = font_ttf.recs[0];

	while (!WindowShouldClose()) {
/*		int keycode = 0;
		while (keycode = GetCharPressed()) {
			switch (keycode) {
			case KEY_UP:
				cursor_pos.y--;
				break;
			case KEY_DOWN:
				cursor_pos.y++;
				break;
			case KEY_LEFT:
				cursor_pos.x--;
				break;
			case KEY_RIGHT:
				cursor_pos.x++;
				break;
			default:
				line[cursor_pos.y][cursor_pos.x] = keycode;
				cursor_pos.x++;
				if (cursor_pos.x >= cols) {
					cursor_pos.x = 0;
					cursor_pos.y++;
				}
				break;
			}
		}
*/

		if (IsKeyPressed(KEY_ENTER)) {
			write(outfd[1], "\n", 1);
			close(outfd[1]);
		}
		

		struct pollfd poll_ctrl = { infd[0], POLLIN, 0 }; 
		int poll_res = 0;
		while (1) {
			poll_res = poll(&poll_ctrl, 1, 10);
			if ((poll_res > 0) && (poll_ctrl.revents & POLLIN))
				read(infd[0], &line[0][cursor_pos.x++], 1);
			else
				break;
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

	return 0;
}

int get_pos_idx(vec2i pos, int cols) {
	return pos.y * cols + pos.x;
}
