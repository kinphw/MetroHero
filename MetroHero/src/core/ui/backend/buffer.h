#ifndef UI_BUFFER_H
#define UI_BUFFER_H

#include <windows.h>

typedef struct {
    char ch[5];        // UTF-8 char (max 4 bytes + null)
    char color[32];    // ANSI Color char string
    int isDoubleWidth; // 0=single, 1=double, -1=padding
} Cell;

extern Cell* screenBuffer;
extern int bufferWidth;
extern int bufferHeight;
extern char globalColor[32];

void ui_init_buffer(void);
void ui_clear_buffer(void);
void ui_present(void);

// Basic low-level drawing (often used internally but exposed if needed)
void ui_draw_char_at(int x, int y, const char* c, const char* color);

#endif
