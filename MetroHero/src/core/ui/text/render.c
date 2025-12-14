#define _CRT_SECURE_NO_WARNINGS
#include "render.h"
#include "raylib.h"
#include <string.h>
#include <stdio.h>

#define MAX_TEXTURES 64

// Simple Texture Cache
typedef struct {
    char path[128];
    Texture2D texture;
} TextureItem;

static TextureItem textures[MAX_TEXTURES];
static int textureCount = 0;

static Texture2D GetCachedTexture(const char* path) {
    if (!path) return (Texture2D){0};

    // Check cache
    for (int i = 0; i < textureCount; i++) {
        if (strcmp(textures[i].path, path) == 0) {
            return textures[i].texture;
        }
    }
    
    // Load new
    if (textureCount < MAX_TEXTURES) {
        Texture2D tex = LoadTexture(path);
        if (tex.id == 0) {
             // Failed to load
             // printf("Failed to load texture: %s\n", path);
        }
        strcpy(textures[textureCount].path, path);
        textures[textureCount].texture = tex;
        textureCount++;
        return tex;
    }
    
    return (Texture2D){0};
}

void ui_draw_tile(int x, int y, const char* imagePath) {
    if (!imagePath) return;
    
    Texture2D tex = GetCachedTexture(imagePath);
    
    // Grid coordinate to Pixel coordinate conversion
    // Grid X is in 8px units (since 1 tile = 2 chars width = 16px)
    // Grid Y is in 16px units
    int px = x * 8;
    int py = y * 16;

    if (tex.id != 0) {
        DrawTexture(tex, px, py, WHITE);
    } else {
        // Fallback: Magenta square
        DrawRectangle(px, py, 16, 16, MAGENTA);
    }
}

// Helper to map ANSI color codes roughly to Raylib colors
static Color GetColorFromAnsi(const char* ansi) {
    if (!ansi) return WHITE;
    if (strstr(ansi, "31")) return RED;
    if (strstr(ansi, "32")) return GREEN;
    if (strstr(ansi, "33")) return YELLOW;
    if (strstr(ansi, "34")) return BLUE;
    if (strstr(ansi, "35")) return MAGENTA;
    if (strstr(ansi, "36")) return SKYBLUE; // Cyan
    if (strstr(ansi, "90")) return DARKGRAY;
    if (strstr(ansi, "91")) return PINK;
    if (strstr(ansi, "93")) return GOLD;
    return WHITE;
}

void ui_draw_str_at(int x, int y, const char* str, const char* color) {
    if (!str) return;

    int px = x * 8;
    int py = y * 16;
    
    Color c = GetColorFromAnsi(color);
    
    // Raylib default font size is 10? 20 is good readability
    // Note: This won't perfectly match the grid like a monospaced console font
    // unless we load a monospaced font. For now, we use default.
    DrawText(str, px, py, 20, c);
}

int ui_draw_text_clipped(int x, int y, int maxWidth, const char* text, const char* color) {
    if (!text) return 0;
    ui_draw_str_at(x, y, text, color);
    return (int)strlen(text); 
}
