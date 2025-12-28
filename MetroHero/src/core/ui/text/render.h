#ifndef UI_RENDER_H
#define UI_RENDER_H

#include "raylib.h" // For Color struct

void ui_draw_str_at(int x, int y, const char* str, const char* color);
void ui_draw_str_ansi(int x, int y, const char* str); // ★ Added wrapper
int ui_draw_text_clipped(int x, int y, int maxWidth, const char* text, const char* color);
// Basic low-level drawing (often used internally but exposed if needed)
void ui_draw_image(int x, int y, int w, int h, const char* imagePath); // ★ Added
void ui_draw_tile(int x, int y, const char* imagePath); // Raylib specific
void ui_load_font(void);
Color GetColorFromAnsi(const char* ansi);

#endif
