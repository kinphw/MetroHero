#include "glyph.h"

void ui_get_glyph_info(const char* s, int* byteLen, int* displayWidth) {
    unsigned char c = (unsigned char)*s;

    if (c < 128) {
        *byteLen = 1;
        *displayWidth = 1;
    }
    else if ((c & 0xE0) == 0xC0) {
        *byteLen = 2;
        *displayWidth = 2;
    }
    else if ((c & 0xF0) == 0xE0) {
        *byteLen = 3;
        if (c == 0xE2) {
            unsigned char c2 = (unsigned char)*(s + 1);

            // Block Elements (E2 96): █░ - 폭 2 ★ 먼저 체크!
            if (c2 == 0x96) {
                //*displayWidth = 2;
				*displayWidth = 1;
            }
            // Box Drawing (E2 94/95): ─│┌┐ - 폭 1
            else if (c2 == 0x94 || c2 == 0x95) {
                *displayWidth = 1;
            }
            // Arrows (E2 86): →↑↓← - 폭 1
            else if (c2 == 0x86) {
                *displayWidth = 1;
            }
            // Misc Symbols (E2 98/9A/9B): ☠⚔ 등 - 폭 1
            else if (c2 == 0x98 || c2 == 0x9A || c2 == 0x9B) {
                *displayWidth = 1;
            }
            // General Punctuation (E2 80) - 폭 1
            else if (c2 == 0x80) {
                *displayWidth = 1;
            }
            // 기타 - 폭 2
            else {
                *displayWidth = 2;
            }
        }
        else {
            *displayWidth = 2;
        }
    }
    else if ((c & 0xF8) == 0xF0) {
        *byteLen = 4;
        *displayWidth = 2;
    }
    else {
        *byteLen = 1;
        *displayWidth = 1;
    }
}

int display_width(const char* str) {
    int width = 0;
    const char* s = str;
    int byteLen, padding;
    
    while (*s) {
        if (*s == '\033' || *s == 0x1B) {
            s++;
            if (*s == '[') {
                s++;
                while (*s && *s != 'm') s++;
                if (*s == 'm') s++;
            }
            continue;
        }
        
        ui_get_glyph_info(s, &byteLen, &padding); // Use padding as temp width var
        width += padding;
        s += byteLen;
    }
    return width;
}
