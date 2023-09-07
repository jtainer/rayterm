// 
// rayterm - terminal emulator made with raylib
//
// 2023, Jonathan Tainer
//

#include <stdlib.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <raylib.h>
#include <raymath.h>

typedef struct vec2i {
	int x;
	int y;
} vec2i;

int get_pos_idx(vec2i, int);

int main() {
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
		int keycode = 0;
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
}

int get_pos_idx(vec2i pos, int cols) {
	return pos.y * cols + pos.x;
}
