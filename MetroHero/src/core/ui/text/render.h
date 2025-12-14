#ifndef UI_RENDER_H
#define UI_RENDER_H

#include "raylib.h" // For Color struct

void ui_draw_str_at(int x, int y, const char* str, const char* color);
int ui_draw_text_clipped(int x, int y, int maxWidth, const char* text, const char* color);
void ui_draw_tile(int x, int y, const char* imagePath); // Raylib specific
void ui_load_font(void);
Color GetColorFromAnsi(const char* ansi);

#endif
