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

static RenderTexture2D target;

#include "../text/render.h" // For ui_load_font

void ui_init_buffer(void) {
    const int screenWidth = SCREEN_W * 8;   // 210 * 8 = 1680
    const int screenHeight = SCREEN_H * 16; // 55 * 16 = 880

    InitWindow(screenWidth, screenHeight, "MetroHero");
    SetExitKey(0); // Disable ESC to exit (handled manually)
    SetTargetFPS(60);
    
    // Load custom font for Korean support
    ui_load_font();
    
    target = LoadRenderTexture(screenWidth, screenHeight);
    SetTextureFilter(target.texture, TEXTURE_FILTER_POINT);
}

void ui_begin_texture_mode(void) {
    BeginTextureMode(target);
}

void ui_end_texture_mode(void) {
    EndTextureMode();
}

void ui_clear_buffer(void) {
    BeginTextureMode(target);
    ClearBackground(BLACK);
    EndTextureMode();
}

void ui_present(void) {
    BeginDrawing();
    ClearBackground(BLACK);
    
    // Draw the backing texture to the screen
    // Note: RenderTextures are Y-flipped in default OpenGL coordinates
    Rectangle src = { 0, 0, (float)target.texture.width, (float)-target.texture.height };
    Rectangle dst = { 0, 0, (float)GetScreenWidth(), (float)GetScreenHeight() };
    Vector2 origin = { 0, 0 };
    
    DrawTexturePro(target.texture, src, dst, origin, 0.0f, WHITE);

    // Draw FPS for debug
    DrawText(TextFormat("%i FPS", GetFPS()), 10, 10, 20, GREEN);
    EndDrawing();
}

// These are legacy/unused in Raylib mode directly, 
// as render.c will now call DrawText directly instead of writing to buffer.
void ui_draw_char_at(int x, int y, const char* c, const char* color) {
    // No-op or implementation if needed
}
