#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include "ui.h"
#include "../../entity/player.h"
#include "../../world/glyph.h"

// ============================================
// Internal UI Layout Constants
// ============================================
#define TOP_H 20

// 뷰포트 영역 (왼쪽)
#define VIEWPORT_X 0
#define VIEWPORT_Y 0
#define VIEWPORT_W 118    // Reduced slightly to be safe? Or keep 120? Let's Keep 118 for safety margin.
#define VIEWPORT_H 30

// 상태창 (오른쪽 상단)
#define STATUS_X 120      // Moved left
#define STATUS_Y 0
#define STATUS_W 48       // Reduced from 50
#define STATUS_H 14

// 장비창 (오른쪽 중단)
#define EQUIP_X 120
#define EQUIP_Y 14
#define EQUIP_W 48
#define EQUIP_H 16

// 대화창 (상태창/장비창 위에 오버레이)
#define DIALOGUE_X 120
#define DIALOGUE_Y 0
#define DIALOGUE_W 48
#define DIALOGUE_H 30

// 로그창 (하단 전체)
// Width 168: 120 + 48 = 168.
#define LOG_X 0
#define LOG_Y 30
#define LOG_W 168
#define LOG_H 12
// ============================================

// ============================================
// Double Buffering Engine
// ============================================
// ============================================
// ANSI Double Buffering Engine
// ============================================

typedef struct {
    char ch[5];        // UTF-8 char (max 4 bytes + null)
    char color[32];    // ANSI Color char string
    int isDoubleWidth; // 0=single, 1=double, -1=padding
} Cell;

static Cell* screenBuffer = NULL;
static int bufferWidth = 0;
static int bufferHeight = 0;
// Current global color state for drawing
static char globalColor[32] = "\033[0m"; 
static int combatEffectFrames = 0; 

void ui_init_buffer(void) {
    // 1. Setup Console
    // Enable ANSI sequences for Windows 10+
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);
    
    // Set UTF-8 Output
    SetConsoleOutputCP(CP_UTF8);

    // Fullscreen setup (Alt+Enter simulation)
    keybd_event(VK_MENU, 0x38, 0, 0);
    keybd_event(VK_RETURN, 0x1c, 0, 0);
    keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);
    Sleep(100);

    // 2. Allocate Buffer
    bufferWidth = 168; 
    bufferHeight = 42; 
    screenBuffer = (Cell*)malloc(sizeof(Cell) * bufferWidth * bufferHeight);
    
    // Hide Cursor
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 1;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(hOut, &info);

    ui_clear_buffer();
}

void ui_clear_buffer(void) {
    if (!screenBuffer) return;
    for (int i = 0; i < bufferWidth * bufferHeight; i++) {
        strcpy(screenBuffer[i].ch, " ");
        strcpy(screenBuffer[i].color, "\033[0m"); 
        screenBuffer[i].isDoubleWidth = 0;
    }
    strcpy(globalColor, "\033[0m"); // Reset global color state
}

void ui_present(void) {
    if (!screenBuffer) return;

    // Estimate buffer size: Width * Height * (Char(4) + Color(12)) + Newlines
    static char* outBuf = NULL;
    static int outBufSize = 0;
    int requiredSize = bufferWidth * bufferHeight * 20; 
    
    if (outBuf == NULL || outBufSize < requiredSize) {
        free(outBuf);
        outBuf = (char*)malloc(requiredSize);
        outBufSize = requiredSize;
    }

    char* p = outBuf;
    
    // Reset cursor to top-left
    p += sprintf(p, "\033[H"); 
    
    char lastColor[32] = "";
    
    for (int y = 0; y < bufferHeight; y++) {
        for (int x = 0; x < bufferWidth; x++) {
            int idx = y * bufferWidth + x;
            Cell* c = &screenBuffer[idx];
            
            if (c->isDoubleWidth == -1) continue; // Skip padding
            
            // Only print color if changed
            if (strcmp(c->color, lastColor) != 0) {
                p += sprintf(p, "\033[0m%s", c->color); // Reset before applying new color to avoid BG bleed
                strcpy(lastColor, c->color);
            }
            
            p += sprintf(p, "%s", c->ch);
        }
        if (y < bufferHeight - 1) {
             p += sprintf(p, "\n");
        }
    }
    
    // Write whole frame at once
    DWORD written;
    WriteConsoleA(GetStdHandle(STD_OUTPUT_HANDLE), outBuf, (DWORD)(p - outBuf), &written, NULL);
}

void ui_draw_char_at(int x, int y, const char* c, const char* color) {
    if (x < 0 || x >= bufferWidth || y < 0 || y >= bufferHeight) return;
    
    int index = y * bufferWidth + x;
    
    // If double width, need check? Assuming single char is single width for now
    // But caller might pass multibyte char.
    // ui_draw_char_at is usually used for single glyphs like borders.
    
    strncpy(screenBuffer[index].ch, c, 4);
    screenBuffer[index].ch[4] = '\0';
    
    if (color) strcpy(screenBuffer[index].color, color);
    else strcpy(screenBuffer[index].color, "\033[0m");
    
    screenBuffer[index].isDoubleWidth = 0; 
}

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

        // Determine Byte Length & Width
        int byteLen = 1;
        int width = 1;
        
        unsigned char c = (unsigned char)*s;
        
        if (c < 128) {
            byteLen = 1;
            width = 1;
        }
        else if ((c & 0xE0) == 0xC0) {
            byteLen = 2; 
            width = 1; // Most 2-byte UTF8 are 1 width? Greek/Cyrillic etc.
        }
        else if ((c & 0xF0) == 0xE0) {
            byteLen = 3;
            // Asian chars (Korean/Chinese/Japanese) are usually here and Width 2
            // Box Drawing are also here (E2 94 xx) but Width 1
            if (c == 0xE2) {
                // U+25xx Box Drawing: E2 94 xx / E2 95 xx -> Width 1
                if (*(s+1) >= 0x94 && *(s+1) <= 0x95) width = 1;
                // U+2591 Light Shade (░): E2 96 91 -> Width 1 (Fix for HP bar)
                else if (*(s+1) == 0x96 && *(s+2) == 0x91) width = 1;
                else width = 2; 
            }
            else width = 2; 
        }
        else if ((c & 0xF8) == 0xF0) {
            byteLen = 4;
            width = 2; // Emojis
        }
        
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

// Old functions wrapper or replacement
void ui_init(void) {
    ui_init_buffer();
}

void console_clear_fast(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);

    DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD count;
    COORD homeCoords = { 0, 0 };

    FillConsoleOutputCharacter(hOut, ' ', cellCount, homeCoords, &count);
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, cellCount, homeCoords, &count);
    SetConsoleCursorPosition(hOut, homeCoords);
}

void console_goto(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}

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
        else if ((*s & 0xE0) == 0xC0) { width += 1; s += 2; } // 2-bytes are usually width 1
        else if ((*s & 0xF0) == 0xE0) {
            if (*s == 0xE2) {
                unsigned char c2 = *(s + 1);
                // U+21xx Arrow (E2 86 xx): Treat as Width 2 (User Feedback)
                // U+25xx Box Drawing (E2 94/95 xx): Width 1
                if (c2 >= 0x94 && c2 <= 0x95) width += 1;
                // U+2591 Light Shade (E2 96 91): Width 1
                else if (c2 == 0x96 && *(s+2) == 0x91) width += 1;
                else width += 2;
            } else width += 2;
            s += 3;
        }
        else if ((*s & 0xF8) == 0xF0) { width += 2; s += 4; }
        else { s += 1; }
    }
    return width;
}

// ★ Returns the actual display width used (number of columns)
int ui_draw_text_clipped(int x, int y, int maxWidth, const char* text, const char* color) {
    if (!text) return 0;

    // Using a temp buffer to accumulate valid chars that fit in maxWidth
    // But direct drawing is easier with our char-by-char system

    int currentWidth = 0;
    const unsigned char* s = (const unsigned char*)text;
    int curX = x;

    // If color provided, set it once
    // (Actual ui_draw_str_at handles color parsing per char, but we can just pass base color)
    // We will leverage ui_draw_str_at for chunks, but we need width checks.

    // Easier: Implement a version of ui_draw_str_at that stops at width limit.
    // Or just parse here and call ui_draw_str_at for fitting substrings?
    // No, character fragmentation issues.

    // Let's iterate and call ui_draw_str_at for small chunks or single chars if needed,
    // OR just modify ui_draw_str_at to take a limit?
    // ui_draw_str_at checks bufferWidth.
    // Let's implement logic here manually calling ui_draw_str_at for each "glyph".

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
                 ui_draw_str_at(curX, y, buf, NULL); // NULL color to preserve embedded? No, ui_draw_str_at handles execution
                 // But wait, ui_draw_str_at sets global color.
             }
             continue;
        }

        int charLen = 1;
        int charWidth = 1;

        if (*s < 128) {
            charWidth = 1;
            charLen = 1;
        }
        else if ((*s & 0xE0) == 0xC0) {
            charWidth = 1; // 2-byte usually width 1
            charLen = 2;
        }
        else if ((*s & 0xF0) == 0xE0) {
            charLen = 3;
             if (*s == 0xE2) {
                unsigned char c2 = *(s + 1);
                if (c2 == 0x86) charWidth = 1;
                else if (c2 >= 0x94 && c2 <= 0x95) charWidth = 1;
                else charWidth = 2;
            } else charWidth = 2;
        }
        else if ((*s & 0xF8) == 0xF0) {
            charWidth = 2;
            charLen = 4;
        }
        else { s++; continue; } // Invalid?

        if (currentWidth + charWidth > maxWidth) break;

        char buf[8] = {0};
        strncpy(buf, (const char*)s, charLen);

        // Pass base color only if not inside ANSI stream?
        // For simplicity, we assume `color` argument is the base state.
        // ui_draw_str_at handles it.

        ui_draw_str_at(curX, y, buf, color);

        curX += charWidth;
        currentWidth += charWidth;
        s += charLen;

        // Only set color for the first char to establish state?
        // No, ui_draw_str_at sets global color if arg provided.
        // If we provide color for EVERY char, it resets global color every time.
        // We should only provide color for the first call?
        // Actually ui_draw_str_at logic:
        // if (color) strcpy(globalColor, color);
        // This makes it "Stateless" per call if color is provided.
        // Good for consistent text color.
    }

    return currentWidth; // ★ 실제 사용된 표시 폭 반환
}

// 뷰포트 렌더링 래퍼
void ui_render_map_viewport(Map* m, Player* p) {
    // Reset color before map render to avoid bleed
    strcpy(globalColor, "\033[0m");
    map_draw_viewport(m, p, VIEWPORT_X, VIEWPORT_Y, 60, VIEWPORT_H);
}

void ui_draw_box(int x, int y, int w, int h, const char* title) {
    // Force default color for border
    const char* borderCol = "\033[0m"; 
    
    // Corners
    ui_draw_str_at(x, y, "┌", borderCol);
    ui_draw_str_at(x + w - 1, y, "┐", borderCol);
    ui_draw_str_at(x, y + h - 1, "└", borderCol);
    ui_draw_str_at(x + w - 1, y + h - 1, "┘", borderCol);

    // Top/Bottom edges
    for (int i = 1; i < w - 1; i++) {
        ui_draw_str_at(x + i, y, "─", borderCol);
        ui_draw_str_at(x + i, y + h - 1, "─", borderCol);
    }

    // Side edges
    for (int i = 1; i < h - 1; i++) {
        ui_draw_str_at(x, y + i, "│", borderCol);
        ui_draw_str_at(x + w - 1, y + i, "│", borderCol);
    }

    // Title
    if (title) {
        ui_draw_str_at(x + 1, y, "─ ", borderCol);

        // Draw title (use default color or bright white?)
        int titleActualWidth = ui_draw_text_clipped(x + 3, y, w - 5, title, "\033[97m"); // Bright White

        if (3 + titleActualWidth < w - 1) {
            ui_draw_str_at(x + 3 + titleActualWidth, y, " ─", borderCol);
        }

        // ★ 오른쪽 위 모서리 보호
        ui_draw_str_at(x + w - 1, y, "┐", borderCol);
    }

    // ★ 오른쪽 아래 모서리 보호
    ui_draw_str_at(x + w - 1, y + h - 1, "┘", borderCol);
}

void ui_draw_stats(const Player* p) {
    int x = STATUS_X;
    int y = STATUS_Y;
    int w = STATUS_W;
    int h = STATUS_H;
    
    ui_draw_box(x, y, w, h, "상태");

    char buf[128];

    // ★ HP Bar - 개별 문자로 그리기 (정확한 폭 제어)
    ui_draw_str_at(x + 2, y + 2, "HP: ", NULL);
    int hpBars = (p->hp * 10) / p->maxHp;
    if (hpBars > 10) hpBars = 10;
    int barX = x + 2 + 4;  // "HP: " = 4칸
    for (int i = 0; i < 10; i++) {
        ui_draw_str_at(barX + i * 2, y + 2, i < hpBars ? "█" : "░", NULL);
    }
    // HP 바 뒤 공간 채우기
    int barEndX = barX + 10 * 2;  // 10개 블록 * 2칸
    for (int i = barEndX; i < x + w - 2; i++) {
        ui_draw_str_at(i, y + 2, " ", NULL);
    }

    // HP Text
    snprintf(buf, sizeof(buf), "     %3d / %3d", p->hp, p->maxHp);
    ui_draw_text_clipped(x + 2, y + 3, w - 4, buf, NULL);

    // Attack
    snprintf(buf, sizeof(buf), " 공격력: %2d~%2d", p->attackMin, p->attackMax);
    ui_draw_text_clipped(x + 2, y + 5, w - 4, buf, NULL);

    // Defense
    snprintf(buf, sizeof(buf), " 방어력:  %3d", p->defense);
    ui_draw_text_clipped(x + 2, y + 6, w - 4, buf, NULL);

    // ★ Direction - 개별 출력 + 공백 채우기
    const char* arrow = " ";
    if (p->dirY < 0) arrow = "↑";
    else if (p->dirY > 0) arrow = "↓";
    else if (p->dirX < 0) arrow = "←";
    else if (p->dirX > 0) arrow = "→";

    ui_draw_str_at(x + 2, y + 7, " 방향:    ", NULL);
    ui_draw_str_at(x + 2 + 10, y + 7, arrow, NULL);
    // 방향 뒤 공간 채우기
    int dirEndX = x + 2 + 10 + 2;  // " 방향:    " (10칸) + 화살표 (2칸)
    for (int i = dirEndX; i < x + w - 2; i++) {
        ui_draw_str_at(i, y + 7, " ", NULL);
    }

    // Combat Effect Overlay
    if (combatEffectFrames > 0) {
        int ex = STATUS_X + 25;
        int ey = STATUS_Y + 2;

        ui_draw_str_at(ex, ey - 1, "전투중", COLOR_BRIGHT_RED);
        ui_draw_str_at(ex, ey,     "  ⚔",   COLOR_BRIGHT_RED);
        ui_draw_str_at(ex, ey + 1, " ⚔⚔⚔", COLOR_BRIGHT_RED);
        ui_draw_str_at(ex, ey + 2, "⚔⚔⚔⚔⚔", COLOR_BRIGHT_RED);
        ui_draw_str_at(ex, ey + 3, " ⚔⚔⚔", COLOR_BRIGHT_RED);
        ui_draw_str_at(ex, ey + 4, "  ⚔",   COLOR_BRIGHT_RED);

        combatEffectFrames--;
    }

    // ★ 테두리 모서리 보호
    const char* borderCol = "\033[0m";
    ui_draw_str_at(x, y, "┌", borderCol);
    ui_draw_str_at(x + w - 1, y, "┐", borderCol);
    ui_draw_str_at(x, y + h - 1, "└", borderCol);
    ui_draw_str_at(x + w - 1, y + h - 1, "┘", borderCol);
}

void ui_draw_equipment(const Player* p) {
    int x = EQUIP_X;
    int y = EQUIP_Y;
    int w = EQUIP_W;
    int h = EQUIP_H;

    ui_draw_box(x, y, w, h, "장비");

    char buf[128];
    snprintf(buf, sizeof(buf), " 무기:    %s", p->weaponName);
    ui_draw_text_clipped(x + 2, y + 2, w - 4, buf, NULL);

    snprintf(buf, sizeof(buf), " 방어구:  %s", p->armorName);
    ui_draw_text_clipped(x + 2, y + 3, w - 4, buf, NULL);

    snprintf(buf, sizeof(buf), " 아이템:  %s", p->item1);
    ui_draw_text_clipped(x + 2, y + 4, w - 4, buf, NULL);

    // ★ 테두리 모서리 보호
    const char* borderCol = "\033[0m";
    ui_draw_str_at(x, y, "┌", borderCol);
    ui_draw_str_at(x + w - 1, y, "┐", borderCol);
    ui_draw_str_at(x, y + h - 1, "└", borderCol);
    ui_draw_str_at(x + w - 1, y + h - 1, "┘", borderCol);
}

#define LOG_LINES 200
char log_buf[LOG_LINES][256];
int log_index = 0;

void ui_add_log(const char* msg) {
    snprintf(log_buf[log_index], sizeof(log_buf[log_index]), "%s", msg);
    log_index = (log_index + 1) % LOG_LINES;
}

void ui_draw_log(void) {
    int x = LOG_X;
    int y = LOG_Y;
    int w = LOG_W;
    int h = LOG_H;

    ui_draw_box(x, y, w, h, "대화");

    int start = (log_index - (h - 2) + LOG_LINES) % LOG_LINES;
    for (int i = 0; i < h - 2; i++) {
        // Log text with potential ANSI
        const char* logText = log_buf[(start + i) % LOG_LINES];

        // ★ 텍스트 그리기 - 실제 사용된 표시 폭을 반환받음
        int actualWidth = ui_draw_text_clipped(x + 2, y + 1 + i, w - 4, logText, NULL);

        // ★ 남은 공간 명시적으로 공백으로 채우기 (오른쪽 테두리 정렬)
        int textEndX = x + 2 + actualWidth;
        for (int j = textEndX; j < x + w - 2; j++) {
            ui_draw_str_at(j, y + 1 + i, " ", NULL);
        }
    }

    // ★ 테두리 모서리 보호
    const char* borderCol = "\033[0m";
    ui_draw_str_at(x, y, "┌", borderCol);
    ui_draw_str_at(x + w - 1, y, "┐", borderCol);
    ui_draw_str_at(x, y + h - 1, "└", borderCol);
    ui_draw_str_at(x + w - 1, y + h - 1, "┘", borderCol);
}

void ui_show_combat_effect(void) {
    combatEffectFrames = 10; // 10 frames duration if not refreshed
    
    // Draw immediately for current frame updates
    int x = STATUS_X + 25;
    int y = STATUS_Y + 2;

    ui_draw_str_at(x, y - 1, "전투중", COLOR_BRIGHT_RED);
    ui_draw_str_at(x, y,     "  ⚔",   COLOR_BRIGHT_RED);
    ui_draw_str_at(x, y + 1, " ⚔⚔⚔", COLOR_BRIGHT_RED);
    ui_draw_str_at(x, y + 2, "⚔⚔⚔⚔⚔", COLOR_BRIGHT_RED);
    ui_draw_str_at(x, y + 3, " ⚔⚔⚔", COLOR_BRIGHT_RED);
    ui_draw_str_at(x, y + 4, "  ⚔",   COLOR_BRIGHT_RED);
    
    // Note: Caller might need to ui_present() if blocking, 
    // but combat logic in loop will eventually present via game_render
}

void ui_hide_combat_effect(void) {
    combatEffectFrames = 0;
    // Don't need to clear immediately, next frame checks flag
}

void ui_draw_dialogue(const NPC* npc) {
    int x = DIALOGUE_X;
    int y = DIALOGUE_Y;
    int w = DIALOGUE_W;
    int h = DIALOGUE_H;
    
    ui_draw_box(x, y, w, h, "대화");

    // Name
    char nameBuf[256];
    snprintf(nameBuf, sizeof(nameBuf), "💬 %s", npc->name);
    ui_draw_text_clipped(x + 2, y + 1, w - 4, nameBuf, NULL);

    // Separator
    ui_draw_str_at(x, y + 2, "├", NULL);
    for (int i = 1; i < w - 1; i++) ui_draw_str_at(x + i, y + 2, "─", NULL);
    ui_draw_str_at(x + w - 1, y + 2, "┤", NULL);

    // Dialogue Content
    const char* dialogue = npc->dialogues[npc->currentDialogue];
    int lineStart = 0;
    int lineNum = 0;
    int maxLines = h - 6;

    for (int i = 3; i < h - 3 && lineNum < maxLines; i++) {
        char lineBuf[256] = "";  // Temporary buffer for current line
        
        int currentWidth = 0;
        const char* dialoguePtr = dialogue + lineStart;
        int maxTextWidth = w - 4; // CONTENT_WIDTH - 2 due to padding

        int charsProcessed = 0;

        while (*dialoguePtr) {
            // ANSI Check
            if (*dialoguePtr == '\033' || *dialoguePtr == 0x1B) {
                // Copy ANSI code to buffer
                const char* ansiStart = dialoguePtr;
                dialoguePtr++;
                if (*dialoguePtr == '[') {
                    while (*dialoguePtr && *dialoguePtr != 'm') dialoguePtr++;
                    if (*dialoguePtr == 'm') dialoguePtr++;
                }
                int codeBytes = (int)(dialoguePtr - ansiStart);
                strncat(lineBuf, ansiStart, codeBytes);
                charsProcessed += codeBytes;
                continue;
            }
            
            // Width Check
            int charW = 1;
            int charBytes = 1;
             if ((*dialoguePtr & 0xE0) == 0xC0) { charBytes = 2; charW = 1; } // Fixed width 1
             else if ((*dialoguePtr & 0xF0) == 0xE0) { charBytes = 3; charW = 2; } 
             else if ((*dialoguePtr & 0xF8) == 0xF0) { charBytes = 4; charW = 2; }
             
             if (currentWidth + charW > maxTextWidth) break;
             
             strncat(lineBuf, dialoguePtr, charBytes);
             currentWidth += charW;
             charsProcessed += charBytes;
             dialoguePtr += charBytes;
        }
        
        ui_draw_text_clipped(x + 2, y + i, w - 4, lineBuf, NULL);
        
        lineStart += charsProcessed;
        lineNum++;
        if (dialogue[lineStart] == '\0') break;
    }

    // Bottom Separator
    ui_draw_str_at(x, y + h - 3, "├", NULL);
    for (int i = 1; i < w - 1; i++) ui_draw_str_at(x + i, y + h - 3, "─", NULL);
    ui_draw_str_at(x + w - 1, y + h - 3, "┤", NULL);

    // Buttons
    char buttonText[64];
    if (npc->currentDialogue < npc->dialogueCount - 1) {
        if (npc->canTrade) snprintf(buttonText, sizeof(buttonText), " [0]다음  [T]거래  [X]닫기");
        else snprintf(buttonText, sizeof(buttonText), " [0]다음  [X]닫기");
    }
    else {
        if (npc->canTrade) snprintf(buttonText, sizeof(buttonText), " [0]끝  [T]거래  [X]닫기");
        else snprintf(buttonText, sizeof(buttonText), " [0]끝  [X]닫기");
    }
    ui_draw_text_clipped(x + 1, y + h - 2, w - 2, buttonText, NULL);
}

void ui_clear_dialogue_area(void) {
    int x = DIALOGUE_X;
    int y = DIALOGUE_Y;
    int w = DIALOGUE_W;
    int h = DIALOGUE_H;
    for (int i = 0; i < h; i++) {
        char spaces[128] = "";
        for (int j = 0; j < w; j++) strcat(spaces, " ");
        ui_draw_str_at(x, y + i, spaces, NULL);
    }
}
