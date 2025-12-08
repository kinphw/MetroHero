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
#define VIEWPORT_W 80    // 40칸 × 2 (전각)
#define VIEWPORT_H 20

// 상태창 (오른쪽 상단)
#define STATUS_X 82
#define STATUS_Y 0
#define STATUS_W 38
#define STATUS_H 10

// 장비창 (오른쪽 중단)
#define EQUIP_X 82
#define EQUIP_Y 10
#define EQUIP_W 38
#define EQUIP_H 6

// 대화창 (상태창/장비창 위에 오버레이)
#define DIALOGUE_X 82
#define DIALOGUE_Y 0
#define DIALOGUE_W 38
#define DIALOGUE_H 17

// 로그창 (하단 전체)
#define LOG_X 0
#define LOG_Y 20
#define LOG_W 120
#define LOG_H 9
// ============================================

void ui_init(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    // ★ 전체화면 설정 (Alt+Enter 효과)
    // Windows 10 이상에서는 SendInput으로 Alt+Enter 키 입력 시뮬레이션
    keybd_event(VK_MENU, 0x38, 0, 0);  // Alt 누름
    keybd_event(VK_RETURN, 0x1c, 0, 0);  // Enter 누름
    keybd_event(VK_RETURN, 0x1c, KEYEVENTF_KEYUP, 0);  // Enter 뗌
    keybd_event(VK_MENU, 0x38, KEYEVENTF_KEYUP, 0);  // Alt 뗌

    // 커서 숨기기
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 1;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
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
        else if ((*s & 0xE0) == 0xC0) { width += 2; s += 2; }
        else if ((*s & 0xF0) == 0xE0) {
            if (*s == 0xE2) {
                unsigned char c2 = *(s + 1);
                if ((c2 >= 0x94 && c2 <= 0x9B) || c2 == 0x80) width += 1;
                else if (c2 == 0x86) width += 1; // 화살표 (U+21xx -> E2 86 xx)
                else width += 2;
            } else width += 2;
            s += 3;
        }
        else if ((*s & 0xF8) == 0xF0) { width += 2; s += 4; }
        else { s += 1; }
    }
    return width;
}

// 뷰포트 렌더링 래퍼
void ui_render_map_viewport(Map* m, Player* p) {
    map_draw_viewport(m, p, VIEWPORT_X, VIEWPORT_Y, 40, VIEWPORT_H);
}

void ui_draw_stats(const Player* p) {
    int x = STATUS_X;
    int y = STATUS_Y;
    int w = STATUS_W;
    int h = STATUS_H;
    
    const int CONTENT_WIDTH = w - 2;
    console_goto(x, y);
    const char* title = "─ 상태 ";
    printf("┌%s", title);
    int titleW = display_width(title);
    for (int i = 0; i < w - 2 - titleW; i++) printf("─");
    printf("┐");

    console_goto(x, y + 1);
    printf("│");
    for (int i = 0; i < CONTENT_WIDTH; i++) printf(" ");
    printf("│");

    console_goto(x, y + 2);
    printf("│ HP:");
    int hpBars = (p->hp * 10) / p->maxHp;
    for (int i = 0; i < 10; i++) printf(i < hpBars ? "█" : "░");
    int remaining = CONTENT_WIDTH - 14; 
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    console_goto(x, y + 3);
    char hpText[64];
    snprintf(hpText, sizeof(hpText), "     %3d / %3d", p->hp, p->maxHp);
    printf("│%s", hpText);
    remaining = CONTENT_WIDTH - display_width(hpText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    console_goto(x, y + 4);
    printf("│");
    for (int i = 0; i < CONTENT_WIDTH; i++) printf(" ");
    printf("│");

    console_goto(x, y + 5);
    char atkText[64];
    snprintf(atkText, sizeof(atkText), " 공격력: %2d~%2d", p->attackMin, p->attackMax);
    printf("│%s", atkText);
    remaining = CONTENT_WIDTH - display_width(atkText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    console_goto(x, y + 6);
    char defText[64];
    snprintf(defText, sizeof(defText), " 방어력:  %3d", p->defense);
    printf("│%s", defText);
    remaining = CONTENT_WIDTH - display_width(defText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    console_goto(x, y + 7);
    char dirText[128];
    const char* arrow = " ";
    if (p->dirY < 0) arrow = "↑";
    else if (p->dirY > 0) arrow = "↓";
    else if (p->dirX < 0) arrow = "←";
    else if (p->dirX > 0) arrow = "→";
    
    snprintf(dirText, sizeof(dirText), " 방향:    %s", arrow);
    printf("│%s", dirText);
    remaining = CONTENT_WIDTH - display_width(dirText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    for (int i = 8; i < h - 1; i++) {
        console_goto(x, y + i);
        printf("│");
        for (int j = 0; j < CONTENT_WIDTH; j++) printf(" ");
        printf("│");
    }
    console_goto(x, y + h - 1);
    printf("└");
    for (int i = 1; i < w - 1; i++) printf("─");

    printf("┘");
}

void ui_draw_equipment(const Player* p) {
    int x = EQUIP_X;
    int y = EQUIP_Y;
    int w = EQUIP_W;
    int h = EQUIP_H;
    
    const int CONTENT_WIDTH = w - 2;

    console_goto(x, y);
    const char* title = "─ 장비 ";
    printf("┌%s", title);
    int titleW = display_width(title);
    for (int i = 0; i < w - 2 - titleW; i++) printf("─");
    printf("┐");

    console_goto(x, y + 1);
    printf("│");
    for (int i = 0; i < CONTENT_WIDTH; i++) printf(" ");
    printf("│");

    console_goto(x, y + 2);
    char weaponText[128];
    snprintf(weaponText, sizeof(weaponText), " 무기:    %s", p->weaponName);
    printf("│%s", weaponText);
    int remaining = CONTENT_WIDTH - display_width(weaponText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    console_goto(x, y + 3);
    char armorText[128];
    snprintf(armorText, sizeof(armorText), " 방어구:  %s", p->armorName);
    printf("│%s", armorText);
    remaining = CONTENT_WIDTH - display_width(armorText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    console_goto(x, y + 4);
    char itemText[128];
    snprintf(itemText, sizeof(itemText), " 아이템:  %s", p->item1);
    printf("│%s", itemText);
    remaining = CONTENT_WIDTH - display_width(itemText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    console_goto(x, y + 5);
    printf("└");
    for (int i = 1; i < w - 1; i++) printf("─");
    printf("┘");
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

    const int CONTENT_WIDTH = w - 4;
    console_goto(x, y);
    const char* titleText = "─ 대화  ";
    printf("┌%s", titleText);
    int titleWidth = display_width(titleText);
    int dashes = w - 2 - titleWidth;
    for (int i = 0; i < dashes; i++) printf("─");
    printf("┐");

    int start = (log_index - (h - 2) + LOG_LINES) % LOG_LINES;
    for (int i = 0; i < h - 2; i++) {
        console_goto(x, y + 1 + i);
        printf("│ ");
        const char* logText = log_buf[(start + i) % LOG_LINES];
        printf("%s", logText);
        int displayLen = display_width(logText);
        int remaining = CONTENT_WIDTH - displayLen;
        for (int j = 0; j < remaining; j++) printf(" ");
        printf(" │");
    }
    console_goto(x, y + h - 1);
    printf("└");
    for (int i = 1; i < w - 1; i++) printf("─");
    printf("┘");
}

void ui_show_combat_effect(void) {
    // 전투 이펙트는 상태창 내부에 표시 (간단히 하드코딩된 오프셋 사용)
    // STATUS_X + 25, STATUS_Y + 2
    int x = STATUS_X + 25;
    int y = STATUS_Y + 2;

    console_goto(x, y - 1);
    printf(COLOR_BRIGHT_RED "전투중" COLOR_RESET);
    console_goto(x, y);
    printf(COLOR_BRIGHT_RED "  ⚔" COLOR_RESET);
    console_goto(x, y + 1);
    printf(COLOR_BRIGHT_RED " ⚔⚔⚔" COLOR_RESET);
    console_goto(x, y + 2);
    printf(COLOR_BRIGHT_RED "⚔⚔⚔⚔⚔" COLOR_RESET);
    console_goto(x, y + 3);
    printf(COLOR_BRIGHT_RED " ⚔⚔⚔" COLOR_RESET);
    console_goto(x, y + 4);
    printf(COLOR_BRIGHT_RED "  ⚔" COLOR_RESET);
}

void ui_hide_combat_effect(void) {
    int x = STATUS_X + 25;
    int y = STATUS_Y + 2;

    console_goto(x, y - 1);
    printf("      "); 
    for (int i = 0; i < 5; i++) {
        console_goto(x, y + i);
        printf("       ");
    }
}

void ui_draw_dialogue(const NPC* npc) {
    int x = DIALOGUE_X;
    int y = DIALOGUE_Y;
    int w = DIALOGUE_W;
    int h = DIALOGUE_H;
    
    const int CONTENT_WIDTH = w - 2;

    console_goto(x, y);
    const char* titleText = "─ 대화 ";
    int titleWidth = display_width(titleText);
    printf("┌%s", titleText);
    for (int i = 0; i < w - 2 - titleWidth; i++) printf("─");
    printf("┐");

    console_goto(x, y + 1);
    printf("│ ");
    char nameWithEmoji[256];
    snprintf(nameWithEmoji, sizeof(nameWithEmoji), "💬 %s", npc->name);
    printf("%s", nameWithEmoji);
    int nameWidth = display_width(nameWithEmoji);
    int remaining = CONTENT_WIDTH - 1 - nameWidth;
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    console_goto(x, y + 2);
    printf("├");
    for (int i = 1; i < w - 1; i++) printf("─");
    printf("┤");

    const char* dialogue = npc->dialogues[npc->currentDialogue];
    int lineStart = 0;
    int lineNum = 0;
    int maxLines = h - 6;

    for (int i = 3; i < h - 3 && lineNum < maxLines; i++) {
        console_goto(x, y + i);
        printf("│ ");
        
        int charsToPrint = 0;
        int currentWidth = 0;
        const char* dialoguePtr = dialogue + lineStart;
        int maxTextWidth = CONTENT_WIDTH - 1;

        while (*dialoguePtr) {
            unsigned char c = *dialoguePtr;
            int codeWidth = 0;
            int codeBytes = 0;

            if (c == '\033' || c == 0x1B) {
                const char* ansiStart = dialoguePtr;
                dialoguePtr++;
                if (*dialoguePtr == '[') {
                    dialoguePtr++;
                    while (*dialoguePtr && *dialoguePtr != 'm') dialoguePtr++;
                    if (*dialoguePtr == 'm') dialoguePtr++;
                }
                codeBytes = (int)(dialoguePtr - ansiStart);
                charsToPrint += codeBytes;
                continue;
            }

            if (c < 128) { codeWidth = 1; codeBytes = 1; }
            else if ((c & 0xE0) == 0xC0) { codeWidth = 2; codeBytes = 2; }
            else if ((c & 0xF0) == 0xE0) {
                 if (c == 0xE2) {
                     unsigned char c2 = *(dialoguePtr + 1);
                     if ((c2 >= 0x94 && c2 <= 0x97) || c2 == 0x80) codeWidth = 1;
                     else codeWidth = 2;
                } else codeWidth = 2;
                codeBytes = 3;
            }
            else if ((c & 0xF8) == 0xF0) { codeWidth = 2; codeBytes = 4; }
            else { codeWidth = 0; codeBytes = 1; }

            if (currentWidth + codeWidth > maxTextWidth) break;
            currentWidth += codeWidth;
            charsToPrint += codeBytes;
            dialoguePtr += codeBytes;
        }

        if (charsToPrint > 0) {
            for (int j = 0; j < charsToPrint; j++) putchar(dialogue[lineStart + j]);
            lineStart += charsToPrint;
        }

        remaining = CONTENT_WIDTH - 1 - currentWidth;
        for (int j = 0; j < remaining; j++) printf(" ");
        printf("│");
        lineNum++;
        if (dialogue[lineStart] == '\0') break;
    }

    for (int i = 3 + lineNum; i < h - 3; i++) {
        console_goto(x, y + i);
        printf("│");
        for (int j = 0; j < CONTENT_WIDTH; j++) printf(" ");
        printf("│");
    }

    console_goto(x, y + h - 3);
    printf("├");
    for (int i = 1; i < w - 1; i++) printf("─");
    printf("┤");

    console_goto(x, y + h - 2);
    char buttonText[64];
    if (npc->currentDialogue < npc->dialogueCount - 1) {
        if (npc->canTrade) snprintf(buttonText, sizeof(buttonText), " [0]다음  [T]거래  [X]닫기");
        else snprintf(buttonText, sizeof(buttonText), " [0]다음  [X]닫기");
    }
    else {
        if (npc->canTrade) snprintf(buttonText, sizeof(buttonText), " [0]끝  [T]거래  [X]닫기");
        else snprintf(buttonText, sizeof(buttonText), " [0]끝  [X]닫기");
    }

    printf("│");
    printf("%s", buttonText);
    int buttonWidth = display_width(buttonText);
    remaining = CONTENT_WIDTH - buttonWidth;
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    console_goto(x, y + h - 1);
    printf("└");
    for (int i = 1; i < w - 1; i++) printf("─");
    printf("┘");
}

void ui_clear_dialogue_area(void) {
    int x = DIALOGUE_X;
    int y = DIALOGUE_Y;
    int w = DIALOGUE_W;
    int h = DIALOGUE_H;
    for (int i = 0; i < h; i++) {
        console_goto(x, y + i);
        for (int j = 0; j < w; j++) printf(" ");
    }
}
