#define _CRT_SECURE_NO_WARNINGS
#include "buffer.h"
#include "../layout.h"
#include "raylib.h"
#include <stdio.h>
#include <stdlib.h>

// Raylib backend doesn't use a character buffer
// converting grid coordinates to pixels:
// Char size: 8x16 (fits 16x16 tiles in 2 chars width)

// Legacy globals for linker compatibility
Cell* screenBuffer = NULL;
int bufferWidth = 0;
int bufferHeight = 0;
char globalColor[32] = "";

void ui_init_buffer(void) {
    const int screenWidth = SCREEN_W * 8;   // 210 * 8 = 1680
    const int screenHeight = SCREEN_H * 16; // 55 * 16 = 880

    InitWindow(screenWidth, screenHeight, "MetroHero");
    SetTargetFPS(60);
    
    // Load default font? GetFontDefault() is used by DrawText
}

void ui_clear_buffer(void) {
    if (WindowShouldClose()) {
        // Handle exit elsewhere or flag?
    }
    BeginDrawing();
    ClearBackground(BLACK);
}

void ui_present(void) {
    // Draw FPS for debug
    DrawText(TextFormat("%i FPS", GetFPS()), 10, 10, 20, GREEN);
    EndDrawing();
}

// These are legacy/unused in Raylib mode directly, 
// as render.c will now call DrawText directly instead of writing to buffer.
void ui_draw_char_at(int x, int y, const char* c, const char* color) {
    // No-op or implementation if needed
}
