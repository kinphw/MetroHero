#include <stdio.h>
// #include <windows.h> // Removed
// #include <conio.h>   // Removed
#include "debug.h"
#include "../core/ui/text/glyph.h"

void debug_console_info(void) {
    printf("Console debug info disabled for Raylib mode.\n");
}

// Helper struct for test cases
typedef struct {
    const char* name;
    const char* str;
} GlyphTestCase;

void debug_glyph(void) {
    printf("=== Glyph Width Helper Debugger ===\n\n");

    GlyphTestCase tests[] = {
        {"ASCII 'A'", "A"},
        {"Korean '한'", "한"},
        {"Box '─'", "─"},
        {"Box '│'", "│"},
        {"Box '┌'", "┌"},
        {"Shape '■'", "■"},
        {"Shape '▲'", "▲"},
        {"Sword '⚔'", "⚔"},
        {"Skull '☠'", "☠"},
        {"Block '█'", "█"},
        {"Shade '░'", "░"},
        {"Arrow '→'", "→"}
    };

    for (int i = 0; i < sizeof(tests) / sizeof(tests[0]); i++) {
        const char* name = tests[i].name;
        const char* s = tests[i].str;
        
        int len, width;
        ui_get_glyph_info(s, &len, &width);
        
        // Hex Byte printing
        printf("%-14s : %s [", name, s);
        for (int j = 0; j < len; j++) {
            printf("%02X", (unsigned char)s[j]);
            if (j < len - 1) printf(" ");
        }
        printf("] -> Len: %d, Width: %d\n", len, width);
    }
}

#ifdef UNIT_TEST_GLYPH
int main(void) {
    SetConsoleOutputCP(65001);
    debug_glyph();
    return 0;
}
#endif