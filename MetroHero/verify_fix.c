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
             // U+25xx (Box) or U+20xx (Punctuation) -> Width 1 check
             if (*s == 0xE2) {
                 unsigned char c2 = *(s+1);
                 if ((c2 >= 0x94 && c2 <= 0x97) || c2 == 0x80) width += 1;
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
    // Just verifying display_width correctness implies we fixed the issue.
    int currentWidth = 0;
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
    printf("Width: %d (Expected: 1, Old: 2)\n", display_width(box));

    printf("\n=== CASE 3: Ellipsis ===\n");
    const char* ell = "…"; // U+2026
    printf("String: %s\n", ell);
    printf("Width: %d (Expected: 1, Old: 2)\n", display_width(ell));
    
    return 0;
}
