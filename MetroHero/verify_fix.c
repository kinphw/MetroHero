#include <stdio.h>
#include <string.h>
#include <windows.h>
#include <assert.h>

// Mock display_width logic (Correct Reference - UPDATED)
int display_width(const char* str) {
    int width = 0;
    const unsigned char* s = (const unsigned char*)str;
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
        if (*s < 128) { width += 1; s += 1; }
        else if ((*s & 0xE0) == 0xC0) { width += 2; s += 2; }
        else if ((*s & 0xF0) == 0xE0) { 
             // U+25xx (Box) or U+20xx (Punctuation) or U+26xx (Symbols) -> Width 1 check
             if (*s == 0xE2) {
                 unsigned char c2 = *(s+1);
                 // 0x94~0x9B: Box, Block, Shapes, Misc Symbols
                 if ((c2 >= 0x94 && c2 <= 0x9B) || c2 == 0x80) width += 1;
                 else width += 2;
             } else {
                 width += 2; 
             }
             s += 3; 
        }
        else if ((*s & 0xF8) == 0xF0) { width += 2; s += 4; }
        else { s += 1; }
    }
    return width;
}

// Old Logic (Buggy)
void test_old_logic(const char* text, int maxTextWidth) {
    // ...
}

// New Logic (Fixed)
void test_new_logic(const char* text, int maxTextWidth) {
   // ...
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    
    printf("=== CASE 1: ANSI Colors ===\n");
    const char* ansi = "\033[31mHello\033[0m";
    printf("String: %s\n", ansi);
    printf("Width: %d (Expected: 5)\n", display_width(ansi));
    
    printf("\n=== CASE 2: Box Drawing Char ===\n");
    const char* box = "─"; // U+2500
    printf("String: %s\n", box);
    printf("Width: %d (Expected: 1)\n", display_width(box));

    printf("\n=== CASE 3: Ellipsis ===\n");
    const char* ell = "…"; // U+2026
    printf("String: %s\n", ell);
    printf("Width: %d (Expected: 1)\n", display_width(ell));

    printf("\n=== CASE 4: Symbols (Sword ⚔ / Star ★) ===\n");
    // ⚔ U+2694 (E2 9A 94)
    // ★ U+2605 (E2 98 85)
    // Note: In C string literal, hex escapes might be tricky if not careful, but UTF-8 string literal is better if source is UTF-8.
    // Assuming source is handled as UTF-8 or we manually construct.
    char sword[] = { 0xE2, 0x9A, 0x94, 0 };
    char star[] = { 0xE2, 0x98, 0x85, 0 };
    
    printf("Symbol: Sword (E2 9A 94)\n");
    printf("Width: %d (Expected: 1)\n", display_width(sword));
    
    printf("Symbol: Star (E2 98 85)\n");
    printf("Width: %d (Expected: 1)\n", display_width(star));
    
    return 0;
}
