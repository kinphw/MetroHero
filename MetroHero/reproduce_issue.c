#include <stdio.h>
#include <string.h>
#include <windows.h>

// Copy of display_width from ui.c
int display_width(const char* str) {
    int width = 0;
    const unsigned char* s = (const unsigned char*)str;

    while (*s) {
        // ANSI escape sequences
        if (*s == '\033' || *s == 0x1B) {
            s++;
            if (*s == '[') {
                s++;
                while (*s && *s != 'm') {
                    s++;
                }
                if (*s == 'm') s++;
            }
            continue;
        }

        if (*s < 128) {
            width += 1;
            s += 1;
        }
        else if ((*s & 0xE0) == 0xC0) {
            width += 2;
            s += 2;
        }
        else if ((*s & 0xF0) == 0xE0) {
            width += 2;
            s += 3;
        }
        else if ((*s & 0xF8) == 0xF0) {
            width += 2;
            s += 4;
        }
        else {
            s += 1;
        }
    }
    return width;
}

void test_alignment(int w, const char* text) {
    printf("Testing with width %d\n", w);
    
    // Header logic
    const char* titleText = "─ 대화 ";
    int titleWidth = display_width(titleText);
    printf("Title text: '%s', Calculated Width: %d\n", titleText, titleWidth);
    
    // Simulate header printing
    int headerLen = 0;
    headerLen += 1; // ┌
    headerLen += titleWidth; // Title
    int dashes = w - 2 - titleWidth;
    printf("Dashes count: %d\n", dashes);
    // Assuming dash is width 1 for calculation in loop, but let's see what display_width says for dash
    int dashWidth = display_width("─");
    printf("Dash '─' width: %d\n", dashWidth);
    
    int totalHeaderWidth = 1 + titleWidth + (dashes * dashWidth) + 1;
    printf("Total Header Width (calculated): %d\n", totalHeaderWidth);

    // Body logic
    const int CONTENT_WIDTH = w - 2;
    printf("CONTENT_WIDTH: %d\n", CONTENT_WIDTH);
    
    // Simulate line printing
    int maxTextWidth = CONTENT_WIDTH - 2;
    int currentWidth = 0;
    const char* p = text;
    
    // Simulate the loop
    while (*p && currentWidth < maxTextWidth) {
        unsigned char c = *p;
         if (c < 128) {
            currentWidth += 1;
            p += 1;
        }
        else if ((c & 0xE0) == 0xC0) {
            currentWidth += 2;
            p += 2;
        }
        else if ((c & 0xF0) == 0xE0) {
            currentWidth += 2;
            p += 3;
        }
        else if ((c & 0xF8) == 0xF0) {
            currentWidth += 2;
            p += 4;
        }
        else {
            p += 1;
        }
    }
    
    printf("Text processed: '%.*s'\n", (int)(p - text), text);
    printf("Current Width: %d\n", currentWidth);
    
    int remaining = CONTENT_WIDTH - 2 - currentWidth;
    printf("Remaining spaces: %d\n", remaining);
    
    int totalBodyWidth = 2 + currentWidth + remaining + 1; // │ + space + text + spaces + │
    printf("Total Body Width (calculated): %d\n", totalBodyWidth);
    
    if (totalHeaderWidth != totalBodyWidth) {
        printf("MISMATCH! Header: %d, Body: %d\n", totalHeaderWidth, totalBodyWidth);
    } else {
        printf("MATCH!\n");
    }
    printf("--------------------------------------------------\n");
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    
    // Test with the problematic text
    const char* text = "니다...";
    test_alignment(40, text); // Arbitrary width
    
    // Test with full Korean line
    const char* fullText = "당신은 이 모험을 끝내고 집으로 돌아";
    test_alignment(40, fullText);

    return 0;
}
