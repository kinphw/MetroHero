#include <windows.h>
#include <stdio.h>
#include <conio.h>
#include "debug.h"
#include "../core/ui/text/glyph.h"

void debug_console_info(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // ★ UTF-8 설정 먼저!
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);

    printf("╔════════════════════════════════════╗\n");
    printf("║     콘솔 상태 디버깅 정보          ║\n");
    printf("╚════════════════════════════════════╝\n\n");

    printf("버퍼 크기:   %d x %d\n", csbi.dwSize.X, csbi.dwSize.Y);
    printf("윈도우 크기: %d x %d\n",
        csbi.srWindow.Right - csbi.srWindow.Left + 1,
        csbi.srWindow.Bottom - csbi.srWindow.Top + 1);
    printf("커서 위치:   (%d, %d)\n",
        csbi.dwCursorPosition.X, csbi.dwCursorPosition.Y);
    printf("\n윈도우 좌표:\n");
    printf("  Left=%d, Top=%d\n", csbi.srWindow.Left, csbi.srWindow.Top);
    printf("  Right=%d, Bottom=%d\n", csbi.srWindow.Right, csbi.srWindow.Bottom);

    printf("\n━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━━\n");
    printf("아무 키나 누르면 계속...");
    _getch();
    printf("\n");
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