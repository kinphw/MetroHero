#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <string.h>
#include <windows.h>

// ==========================================
// TEST TARGET: ui_get_glyph_info
// (Copied from src/core/ui/ui.c for standalone testing)
// ==========================================
void ui_get_glyph_info(const char* s, int* byteLen, int* displayWidth) {
    unsigned char c = (unsigned char)*s;
    
    if (c < 128) {
        *byteLen = 1;
        *displayWidth = 1;
    }
    else if ((c & 0xE0) == 0xC0) {
        *byteLen = 2;
        // *displayWidth = 1; // 2-byte UTF-8 is usually 1 width (Latin Extended, etc)
        *displayWidth = 2; // User modified
    }
    else if ((c & 0xF0) == 0xE0) {
        *byteLen = 3;
        // 3-byte characters
        if (c == 0xE2) {
            unsigned char c2 = (unsigned char)*(s + 1);
            
             if ((c2 >= 0x94 && c2 <= 0x9B) || c2 == 0x80) {
                 *displayWidth = 1;  // Box Drawing, Misc Symbols (⚔ 포함)
             }
             else if (c2 == 0x86) {
                 *displayWidth = 2; // Arrows
             }
             else if (c2 == 0x96) {
                 *displayWidth = 2; // Block Elements? (Check specifics)
             }
             else if (c2 >= 0x94 && c2 <= 0x97) {
                 *displayWidth = 1; // Box Drawing, Blocks, Geom Shapes
            }
            else {
                *displayWidth = 2;
            }
        } else {
            *displayWidth = 2; // Korean/Chinese/Japanese etc.
        }
    }
    else if ((c & 0xF8) == 0xF0) {
        *byteLen = 4;
        *displayWidth = 2; // Emojis
    }
    else {
        *byteLen = 1; // Invalid? Treat as 1 byte 1 width
        *displayWidth = 1; 
    }
}
// ==========================================

void print_hex(const char* s, int len) {
    printf("[");
    for(int i=0; i<len; i++) {
        printf("%02X", (unsigned char)s[i]);
        if(i < len-1) printf(" ");
    }
    printf("]");
}

void test_char(const char* name, const char* str) {
    int byteLen = 0;
    int width = 0;
    ui_get_glyph_info(str, &byteLen, &width);
    
    printf("%-15s: %s ", name, str);
    print_hex(str, byteLen);
    printf(" -> Len: %d, Width: %d\n", byteLen, width);
}

int main() {
    SetConsoleOutputCP(CP_UTF8);
    
    printf("=== Glyph Width Helper Debugger ===\n\n");
    
    // Test Cases
    test_char("ASCII 'A'", "A");
    test_char("Korean '한'", "한");
    test_char("Box '─'", "─");
    test_char("Box '│'", "│");
    test_char("Box '┌'", "┌");
    test_char("Shape '■'", "■"); // E2 96 A0
    test_char("Shape '▲'", "▲"); // E2 96 B2
    test_char("Sword '⚔'", "⚔"); // E2 9A 94
    test_char("Skull '☠'", "☠"); // E2 98 A0
    test_char("Block '█'", "█"); // E2 96 88
    test_char("Shade '░'", "░"); // E2 96 91
    test_char("Arrow '→'", "→"); // E2 86 92


    printf("\n=== Visual Width Test ===\n");
    printf("12345678901234567890\n");  // 눈금자 (20칸)
    printf("││││││││││││││││││││\n");  // Box 20개
    printf("────────────────────\n");  // 가로선 20개
    printf("→→→→→→→→→→\n");            // 화살표 10개
    printf("████████████████████\n");  // 블록 20개
    printf("░░░░░░░░░░░░░░░░░░░░\n");  // 빈블록 20개
    printf("⚔⚔⚔⚔⚔⚔⚔⚔⚔⚔⚔⚔⚔⚔⚔⚔⚔⚔⚔⚔\n");  // 검 20개
    printf("가나다라마바사아자차\n");  // 한글 10개
    printf("12345678901234567890\n");  // 눈금자 (20칸)    
    
    printf("\n=== Custom Input (Type a char and Enter) ===\n");
    char buf[256];
    while(1) {
        printf("> ");
        if (!fgets(buf, sizeof(buf), stdin)) break;
        
        // Remove newline
        buf[strcspn(buf, "\n")] = 0;
        if(strlen(buf) == 0) continue;
        if(strcmp(buf, "exit") == 0) break;

        const char* p = buf;
        while (*p) {
            int byteLen=0, width=0;
            ui_get_glyph_info(p, &byteLen, &width);
            
            char tmp[5] = {0};
            strncpy(tmp, p, byteLen);
            
            printf("Char: %s ", tmp);
            print_hex(tmp, byteLen);
            printf(" -> Width: %d\n", width);
            
            p += byteLen;
        }
    }
    
    return 0;
}
