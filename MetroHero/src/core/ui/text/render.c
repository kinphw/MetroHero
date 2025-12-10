#define _CRT_SECURE_NO_WARNINGS
#include "render.h"
#include "glyph.h"
#include "../backend/buffer.h" // Access screenBuffer, globalColor
#include <string.h>
#include <stdlib.h>

void ui_draw_str_at(int x, int y, const char* str, const char* color) {
    if (y < 0 || y >= bufferHeight) return;

    // Priority: Argument color > Embedded ANSI > Default
    // If 'color' arg is provided, use it.
    // If 'color' is NULL, we continue using globalColor (stateful).
    if (color) strcpy(globalColor, color);

    const char* s = str;
    int cx = x;

    while (*s) {
        if (cx >= bufferWidth) break;

        // ANSI Processing
        if (*s == '\033') {
            const char* end = strchr(s, 'm');
            if (end) {
                int len = (int)(end - s) + 1;
                if (len < 32) {
                    strncpy(globalColor, s, len);
                    globalColor[len] = '\0';
                }
                s = end + 1;
                continue;
            }
        }

        // Determine Byte Length & Width using Helper
        int byteLen = 1;
        int width = 1;
        
        ui_get_glyph_info(s, &byteLen, &width);
        
        // Write to Cell
        int index = y * bufferWidth + cx;
        
        strncpy(screenBuffer[index].ch, s, byteLen);
        screenBuffer[index].ch[byteLen] = '\0';
        strcpy(screenBuffer[index].color, globalColor);
        
        if (width == 2) {
            screenBuffer[index].isDoubleWidth = 1;
            cx++;
            if (cx < bufferWidth) {
                screenBuffer[index+1].isDoubleWidth = -1; // Padding
                screenBuffer[index+1].ch[0] = '\0';
                cx++;
            }
        } else {
            screenBuffer[index].isDoubleWidth = 0;
            cx++;
        }
        
        s += byteLen;
    }
}

int ui_draw_text_clipped(int x, int y, int maxWidth, const char* text, const char* color) {
    if (!text) return 0;

    int currentWidth = 0;
    const unsigned char* s = (const unsigned char*)text;
    int curX = x;

    while (*s) {
        if (currentWidth >= maxWidth) break;

        // Check ANSI
        if (*s == '\033' || *s == 0x1B) {
             const unsigned char* start = s;
             s++;
             if (*s == '[') {
                 while (*s && *s != 'm') s++;
                 if (*s == 'm') s++;
             }
             // Draw ANSI code directly (0 width)
             int len = (int)(s - start);
             char buf[32];
             if (len < 32) {
                 strncpy(buf, (const char*)start, len);
                 buf[len] = 0;
                 ui_draw_str_at(curX, y, buf, NULL); 
             }
             continue;
        }

        int charLen = 1;
        int charWidth = 1;

        ui_get_glyph_info((const char*)s, &charLen, &charWidth);

        if (currentWidth + charWidth > maxWidth) break;

        char buf[8] = {0};
        strncpy(buf, (const char*)s, charLen);

        ui_draw_str_at(curX, y, buf, color);

        curX += charWidth;
        currentWidth += charWidth;
        s += charLen;
    }

    return currentWidth; // Returns display width used
}
