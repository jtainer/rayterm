// 
// rayterm - terminal emulator made with raylib
//
// 2023, Jonathan Tainer
//

#include <stdlib.h>
#include <stdio.h>
#include <unistd.h>
#include <raylib.h>
#include <raymath.h>

typedef struct vec2i {
	int x;
	int y;
} vec2i;

int main() {
	int cols = 64;
	int rows = 24;
	char* buf = calloc(cols * rows, 1);
	int cursor_idx = 0;
	vec2i cursor_pos = { 0, 0 };
	
	int window_width = 640;
	int window_height = 480;
	const char window_title[] = "Terminal Emulator";
	
	SetConfigFlags(FLAG_VSYNC_HINT | FLAG_WINDOW_ALWAYS_RUN);
	InitWindow(window_width, window_height, window_title);
	SetTargetFPS(60);

	Font font_ttf = LoadFontEx("fonts/Flexi_IBM_VGA_True.ttf", 20, 0, 250);

	while (!WindowShouldClose()) {
		int c = 0;
		while (c = GetKeyPressed()) {
			buf[cursor_idx++] = c;
		}

		BeginDrawing();
		ClearBackground(BLACK);
		DrawTextEx(font_ttf, buf, Vector2Zero(), (float)font_ttf.baseSize, 0, LIME);
		EndDrawing();
	}
}
