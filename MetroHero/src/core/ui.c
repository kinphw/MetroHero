#include <windows.h>
#include <stdio.h>
#include "ui.h"
#include "../entity/player.h"
#include "../world/glyph.h"

void ui_init(void) {

    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    // 콘솔을 UTF-8로 설정
    SetConsoleOutputCP(CP_UTF8);
    SetConsoleCP(CP_UTF8);

    // ★★★ ANSI 색상 활성화 (이 부분이 꼭 필요!) ★★★
    DWORD dwMode = 0;
    GetConsoleMode(hOut, &dwMode);
    dwMode |= ENABLE_VIRTUAL_TERMINAL_PROCESSING;
    SetConsoleMode(hOut, dwMode);

    // ★ 순서 중요: 먼저 윈도우 크기를 최소로 줄임
    //SMALL_RECT minWindow = { 0, 0, 1, 1 };
    //SetConsoleWindowInfo(hOut, TRUE, &minWindow);

    //// ★ 그 다음 버퍼 크기 설정
    //COORD bufferSize = { SCREEN_W, SCREEN_H };
    //SetConsoleScreenBufferSize(hOut, bufferSize);

    //// ★ 마지막으로 윈도우 크기를 원하는 크기로 설정
    //SMALL_RECT windowSize = { 0, 0, SCREEN_W - 1, SCREEN_H - 1 };
    //SetConsoleWindowInfo(hOut, TRUE, &windowSize);

    // 커서 숨기기(선택)
    CONSOLE_CURSOR_INFO info;
    info.dwSize = 1;
    info.bVisible = FALSE;
    SetConsoleCursorInfo(GetStdHandle(STD_OUTPUT_HANDLE), &info);
}

//void console_clear_fast(void) {
//    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);
//    COORD pos = { 0, 0 };
//    SetConsoleCursorPosition(hOut, pos);
//}

void console_clear_fast(void) {
    HANDLE hOut = GetStdHandle(STD_OUTPUT_HANDLE);

    CONSOLE_SCREEN_BUFFER_INFO csbi;
    GetConsoleScreenBufferInfo(hOut, &csbi);

    DWORD cellCount = csbi.dwSize.X * csbi.dwSize.Y;
    DWORD count;
    COORD homeCoords = { 0, 0 };

    // 화면 전체를 공백으로 채우기
    FillConsoleOutputCharacter(hOut, ' ', cellCount, homeCoords, &count);

    // 화면 속성 리셋
    FillConsoleOutputAttribute(hOut, csbi.wAttributes, cellCount, homeCoords, &count);

    // 커서 위치 리셋
    SetConsoleCursorPosition(hOut, homeCoords);
}


void console_goto(int x, int y) {
    COORD pos = { x, y };
    SetConsoleCursorPosition(GetStdHandle(STD_OUTPUT_HANDLE), pos);
}


// 화면에 표시되는 실제 폭 계산 (한글 = 2칸, 영문 = 1칸)
int display_width(const char* str) {
    int width = 0;
    const unsigned char* s = (const unsigned char*)str;

    while (*s) {
        // ★ ANSI 이스케이프 시퀀스 건너뛰기
        if (*s == '\033' || *s == 0x1B) {  // ESC 문자
            s++;
            if (*s == '[') {  // CSI 시퀀스 시작
                s++;
                // 'm'을 만날 때까지 건너뛰기
                while (*s && *s != 'm') {
                    s++;
                }
                if (*s == 'm') s++;
            }
            continue;
        }

        if (*s < 128) {
            // ASCII (1바이트) = 1칸
            width += 1;
            s += 1;
        }
        else if ((*s & 0xE0) == 0xC0) {
            // UTF-8 2바이트 문자 = 2칸
            width += 2;
            s += 2;
        }
        else if ((*s & 0xF0) == 0xE0) {
            // UTF-8 3바이트 문자 (한글 등) = 2칸
            width += 2;
            s += 3;
        }
        else if ((*s & 0xF8) == 0xF0) {
            // UTF-8 4바이트 문자 = 2칸
            width += 2;
            s += 4;
        }
        else {
            s += 1;
        }
    }
    return width;
}

// ui.c에 추가

void ui_draw_stats(const Player* p, int x, int y, int w, int h) {
    console_goto(x, y);
    printf("┌─ 상태  ──────────────────────────┐");

    console_goto(x, y + 1);
    printf("│                                  │");

    // HP 바 그리기
    console_goto(x, y + 2);
    printf("│ HP:");
    int hpBars = (p->hp * 10) / p->maxHp;
    for (int i = 0; i < 10; i++) {
        if (i < hpBars)
            printf("█");
        else
            printf("░");
    }
    printf("                  │");

    // HP 수치
    console_goto(x, y + 3);
    printf("│     %3d / %3d                    │", p->hp, p->maxHp);

    console_goto(x, y + 4);
    printf("│                                  │");

    // 공격력 범위 표시
    console_goto(x, y + 5);
    printf("│ 공격력: %2d~%2d                  │", p->attackMin, p->attackMax);

    // 방어력
    console_goto(x, y + 6);
    printf("│ 방어력:  %3d                     │", p->defense);

    // 나머지 빈 공간
    for (int i = 7; i < h - 1; i++) {
        console_goto(x, y + i);
        printf("│                                  │");
    }

    console_goto(x, y + h - 1);
    printf("└──────────────────────────────────┘");
}

// ui_draw_equipment 수정 (높이 조정)
void ui_draw_equipment(const Player* p, int x, int y, int w, int h) {
    console_goto(x, y);
    printf("┌─ 장비  ──────────────────────────┐");

    console_goto(x, y + 1);
    printf("│                                  │");

    console_goto(x, y + 2);
    printf("│ 무기:    %s", p->weaponName);
    int len = display_width(p->weaponName);
    for (int i = 0; i < 25 - len; i++) putchar(' ');
    printf(" │");

    console_goto(x, y + 3);
    printf("│ 방어구:  %s", p->armorName);
    len = display_width(p->armorName);
    for (int i = 0; i < 25 - len; i++) putchar(' ');
    printf(" │");

    console_goto(x, y + 4);
    printf("│ 아이템:  %s", p->item1);
    len = display_width(p->item1);
    for (int i = 0; i < 25 - len; i++) putchar(' ');
    printf(" │");

    console_goto(x, y + 5);
    printf("└──────────────────────────────────┘");
}

#define LOG_LINES 200
char log_buf[LOG_LINES][256];
int log_index = 0;

void ui_add_log(const char* msg) {
    snprintf(log_buf[log_index], sizeof(log_buf[log_index]), "%s", msg);
    log_index = (log_index + 1) % LOG_LINES;
}

//void ui_draw_log(int x, int y, int w, int h) {
//    // 상단 테두리
//    console_goto(x, y);
//    printf("┌─ 대화  ");
//    for (int i = 8; i < w - 2; i++) printf("─");
//    printf("┐");
//
//    int start = (log_index - (h - 2) + LOG_LINES) % LOG_LINES;
//
//    // 로그 내용
//    for (int i = 0; i < h - 2; i++) {
//        console_goto(x, y + 1 + i);
//        printf("│ ");
//
//        const char* logText = log_buf[(start + i) % LOG_LINES];
//        printf("%s", logText);
//
//        // ★ 오른쪽 │ 제거 - 공백으로 남은 줄 전부 덮기
//        int displayLen = display_width(logText);
//        int remaining = w - 2 - displayLen;  // "│ " = 2칸만 고려
//
//        for (int j = 0; j < remaining; j++) {
//            printf(" ");
//        }
//
//        //// ★ 화면 표시 폭 정확히 계산
//        //int displayLen = display_width(logText);
//        //int contentWidth = w - 4;  // "│ " + " │" = 4칸
//        //int remaining = contentWidth - displayLen;
//
//        //// ★ 남은 공간을 공백으로 채우기
//        //for (int j = 0; j < remaining; j++) {
//        //    printf(" ");
//        //}
//
//        //printf(" │");
//    }
//
//    // 하단 테두리
//    console_goto(x, y + h - 1);
//    printf("└");
//    for (int i = 1; i < w - 1; i++) printf("─");
//    printf("┘");
//}

void ui_draw_log(int x, int y, int w, int h) {
    // 상단 테두리
    console_goto(x, y);
    printf("┌─ 대화  ");
    for (int i = 8; i < w - 2; i++) printf("─");
    printf("┐");

    int start = (log_index - (h - 2) + LOG_LINES) % LOG_LINES;

    // 로그 내용
    for (int i = 0; i < h - 2; i++) {
        console_goto(x, y + 1 + i);
        printf("│ ");

        const char* logText = log_buf[(start + i) % LOG_LINES];
        printf("%s", logText);

        // ★ 남은 공간을 공백으로 채우기
        int displayLen = display_width(logText);
        int contentWidth = w - 4;  // "│ " + " │" = 4칸
        int remaining = contentWidth - displayLen;

        for (int j = 0; j < remaining + 1; j++) {
            printf(" ");
        }

        printf("│");
    }

    // 하단 테두리
    console_goto(x, y + h - 1);
    printf("└");
    for (int i = 1; i < w - 1; i++) printf("─");
    printf("┘");
}

// ui.c에 추가
void ui_draw_combat_effect(int x, int y) {

    // ★ 전투중 텍스트 (칼 이펙트 위)
    console_goto(x, y - 1);
    printf(COLOR_BRIGHT_RED "전투중" COLOR_RESET);

    // 기존 칼 모양
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

void ui_clear_combat_effect(int x, int y) {

    // ★ "전투중" 지우기
    console_goto(x, y - 1);
    printf("      ");   // "전투중" 3글자 = UTF-8 6칸

    // 기존 칼 이펙트 지우기
    for (int i = 0; i < 5; i++) {
        console_goto(x, y + i);
        printf("       ");
    }
}

