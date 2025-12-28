#define _CRT_SECURE_NO_WARNINGS
#include "render.h"
#include "../backend/buffer.h" // For ui_begin_texture_mode
#include "raylib.h"
#include <string.h>
#include <stdio.h>
#include <stdlib.h>

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

    // ui_begin_texture_mode(); // Optimization: Caller provides batch
    if (tex.id != 0) {
        Rectangle src = { 0.0f, 0.0f, (float)tex.width, (float)tex.height };
        Rectangle dst = { (float)px, (float)py, 16.0f, 16.0f };
        Vector2 origin = { 0.0f, 0.0f };
        DrawTexturePro(tex, src, dst, origin, 0.0f, WHITE);
    } else {
        // Fallback: Magenta square
        DrawRectangle(px, py, 16, 16, MAGENTA);
    }
    // ui_end_texture_mode();
}

// Helper to map ANSI color codes roughly to Raylib colors
Color GetColorFromAnsi(const char* ansi) {
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

static Font globalFont = { 0 };

void ui_load_font(void) {
    // Generate codepoints for:
    // 1. ASCII (32-126)
    // 2. Hangul Syllables (0xAC00-0xD7A3)
    // 3. Box Drawing (0x2500-0x257F) for UI borders
    // 4. Misc Symbols (0x2600-0x26FF) for ★ etc.
    
    int countASCII = 126 - 32 + 1;
    int countHangul = 0xD7A3 - 0xAC00 + 1;
    int countBox = 0x257F - 0x2500 + 1;
    int countSymbols = 0x26FF - 0x2600 + 1;
    int countEmojis = 0x1F9FF - 0x1F300 + 1;
    
    int codepointCount = countASCII + countHangul + countBox + countSymbols + countEmojis;
    int* codepoints = (int*)malloc(codepointCount * sizeof(int));
    
    int index = 0;
    // ASCII
    for (int i = 32; i <= 126; i++) {
        codepoints[index++] = i;
    }
    // Korean Hangul Syllables
    for (int i = 0xAC00; i <= 0xD7A3; i++) {
        codepoints[index++] = i;
    }
    // Box Drawing
    for (int i = 0x2500; i <= 0x257F; i++) {
        codepoints[index++] = i;
    }
    // Misc Symbols
    for (int i = 0x2600; i <= 0x26FF; i++) {
        codepoints[index++] = i;
    }
    // Emojis (Additional Range for 🐈 etc.)
    for (int i = 0x1F300; i <= 0x1F9FF; i++) {
        codepoints[index++] = i;
    }

    // Load font
    globalFont = LoadFontEx("assets/fonts/DNFBitBitv2.ttf", 20, codepoints, codepointCount);
    
    // Set texture filter to point for crisp scaling if needed, 
    // though usually Font textures need bilinear if not perfectly 1:1. 
    // Given the retro style, let's try bilinear first for smoother text or point for crispness.
    // Let's stick to default for now or POINT for pixel art style.
    SetTextureFilter(globalFont.texture, TEXTURE_FILTER_POINT);

    free(codepoints);
}

void ui_draw_str_at(int x, int y, const char* str, const char* color) {
    if (!str) return;

    int px = x * 8;
    int py = y * 16;
    
    Color c = GetColorFromAnsi(color);
    
    // ui_begin_texture_mode(); // Optimization: Caller provides batch
    if (globalFont.baseSize > 0) {
        // Use custom font
        // Position correction might be needed depending on the font baseline
        Vector2 pos = { (float)px, (float)py };
        DrawTextEx(globalFont, str, pos, 20, 0, c);
    } else {
        // Fallback
        DrawText(str, px, py, 20, c);
    }
    // ui_end_texture_mode();
}

#include "glyph.h" // For display_width / ui_get_glyph_info

void ui_draw_str_ansi(int x, int y, const char* str) {
    if (!str) return;

    int currentX = x;
    const char* ptr = str;
    char buffer[256] = { 0 };
    int bufIndex = 0;
    
    // Default color
    char currentColor[32] = "\033[0m"; 

    while (*ptr) {
        // ANSI Check
        if (*ptr == '\033') {
            // 1. Flush buffer if needed
            if (bufIndex > 0) {
                buffer[bufIndex] = '\0';
                ui_draw_str_at(currentX, y, buffer, currentColor);
                
                // Calculate width and advance X
                int w = display_width(buffer);
                currentX += w;
                
                bufIndex = 0;
            }

            // 2. Parse ANSI
            const char* start = ptr;
            ptr++;
            if (*ptr == '[') {
                while (*ptr && *ptr != 'm') ptr++;
                if (*ptr == 'm') ptr++;
            }
            
            // Extract code
            int len = (int)(ptr - start);
            if (len < 31) {
                strncpy(currentColor, start, len);
                currentColor[len] = '\0';
            }
            continue;
        }

        // Buffer text
        if (bufIndex < 255) {
            buffer[bufIndex++] = *ptr++;
        } else {
             // Buffer full safety
             ptr++; 
        }
    }

    // Flush remaining
    if (bufIndex > 0) {
        buffer[bufIndex] = '\0';
        ui_draw_str_at(currentX, y, buffer, currentColor);
    }
}

int ui_draw_text_clipped(int x, int y, int maxWidth, const char* text, const char* color) {
    if (!text) return 0;

    // ★ If color is provided, force specific color (ignore ANSI inside) -> Use original
    if (color != NULL) {
        ui_draw_str_at(x, y, text, color);
    } 
    // ★ If color is NULL, assume ANSI parsing is desired
    else {
        ui_draw_str_ansi(x, y, text);
    }
    
    // Return length (or potentially display width, but original returned strlen)
    return (int)strlen(text); 
}
