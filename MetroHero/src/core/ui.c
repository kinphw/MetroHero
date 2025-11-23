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


// ★ 개선된 display_width 함수
int display_width(const char* str) {
    int width = 0;
    const unsigned char* s = (const unsigned char*)str;

    while (*s) {
        // ANSI 이스케이프 시퀀스 건너뛰기
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
            // UTF-8 4바이트 문자 (이모지) = 2칸
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

// ★ 개선된 상태창 그리기
void ui_draw_stats(const Player* p, int x, int y, int w, int h) {
    const int CONTENT_WIDTH = 34;  // 테두리 안쪽 실제 폭

    console_goto(x, y);
    printf("┌─ 상태  ──────────────────────────┐");

    console_goto(x, y + 1);
    printf("│                                  │");

    // HP 바
    console_goto(x, y + 2);
    printf("│ HP:");
    int hpBars = (p->hp * 10) / p->maxHp;
    for (int i = 0; i < 10; i++) {
        printf(i < hpBars ? "█" : "░");
    }
    // HP 바는 14칸 (4 + 10)
    int remaining = CONTENT_WIDTH - 14;
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    // HP 수치
    console_goto(x, y + 3);
    char hpText[64];
    snprintf(hpText, sizeof(hpText), "     %3d / %3d", p->hp, p->maxHp);
    printf("│%s", hpText);
    remaining = CONTENT_WIDTH - display_width(hpText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    console_goto(x, y + 4);
    printf("│                                  │");

    // 공격력
    console_goto(x, y + 5);
    char atkText[64];
    snprintf(atkText, sizeof(atkText), " 공격력: %2d~%2d", p->attackMin, p->attackMax);
    printf("│%s", atkText);
    remaining = CONTENT_WIDTH - display_width(atkText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    // 방어력
    console_goto(x, y + 6);
    char defText[64];
    snprintf(defText, sizeof(defText), " 방어력:  %3d", p->defense);
    printf("│%s", defText);
    remaining = CONTENT_WIDTH - display_width(defText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    // 빈 공간
    for (int i = 7; i < h - 1; i++) {
        console_goto(x, y + i);
        printf("│                                  │");
    }

    console_goto(x, y + h - 1);
    printf("└──────────────────────────────────┘");
}

// ★ 개선된 장비창 그리기
void ui_draw_equipment(const Player* p, int x, int y, int w, int h) {
    const int CONTENT_WIDTH = 34;

    console_goto(x, y);
    printf("┌─ 장비  ──────────────────────────┐");

    console_goto(x, y + 1);
    printf("│                                  │");

    // 무기
    console_goto(x, y + 2);
    char weaponText[128];
    snprintf(weaponText, sizeof(weaponText), " 무기:    %s", p->weaponName);
    printf("│%s", weaponText);
    int remaining = CONTENT_WIDTH - display_width(weaponText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    // 방어구
    console_goto(x, y + 3);
    char armorText[128];
    snprintf(armorText, sizeof(armorText), " 방어구:  %s", p->armorName);
    printf("│%s", armorText);
    remaining = CONTENT_WIDTH - display_width(armorText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    // 아이템
    console_goto(x, y + 4);
    char itemText[128];
    snprintf(itemText, sizeof(itemText), " 아이템:  %s", p->item1);
    printf("│%s", itemText);
    remaining = CONTENT_WIDTH - display_width(itemText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

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

// ★ 개선된 로그창 그리기
void ui_draw_log(int x, int y, int w, int h) {
    const int CONTENT_WIDTH = w - 4;  // "│ " + " │" = 4칸

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

        // 남은 공간을 공백으로 채우기
        int displayLen = display_width(logText);
        int remaining = CONTENT_WIDTH - displayLen;

        for (int j = 0; j < remaining; j++) {
            printf(" ");
        }

        printf(" │");
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

// ★ 대화창 그리기
void ui_draw_dialogue(const NPC* npc, int x, int y, int w, int h) {
    const int CONTENT_WIDTH = 34;

    // 상단 테두리
    console_goto(x, y);
    printf("┌─ 대화 ");
    for (int i = 7; i < w - 2; i++) printf("─");
    printf("┐");

    // NPC 이름 표시
    console_goto(x, y + 1);
    char nameText[128];
    snprintf(nameText, sizeof(nameText), " 💬 %s", npc->name);
    printf("│%s", nameText);
    int remaining = CONTENT_WIDTH - display_width(nameText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    // 구분선
    console_goto(x, y + 2);
    printf("├");
    for (int i = 1; i < w - 1; i++) printf("─");
    printf("┤");

    // 현재 대화 내용 가져오기
    const char* dialogue = npc->dialogues[npc->currentDialogue];

    // 대화 내용을 여러 줄로 나누기 (간단한 구현)
    char line[128];
    int lineStart = 0;
    int lineNum = 0;
    int maxLines = h - 6;  // 테두리, 이름, 버튼 공간 제외

    for (int i = 3; i < h - 3 && lineNum < maxLines; i++) {
        console_goto(x, y + i);
        printf("│ ");

        // 한 줄에 들어갈 만큼만 추출 (간단 버전)
        int charsToPrint = 0;
        int currentWidth = 0;
        const char* dialoguePtr = dialogue + lineStart;

        while (*dialoguePtr && currentWidth < 30) {
            unsigned char c = *dialoguePtr;

            if (c < 128) {
                currentWidth += 1;
                charsToPrint += 1;
                dialoguePtr += 1;
            }
            else if ((c & 0xE0) == 0xC0) {
                currentWidth += 2;
                charsToPrint += 2;
                dialoguePtr += 2;
            }
            else if ((c & 0xF0) == 0xE0) {
                currentWidth += 2;
                charsToPrint += 3;
                dialoguePtr += 3;
            }
            else if ((c & 0xF8) == 0xF0) {
                currentWidth += 2;
                charsToPrint += 4;
                dialoguePtr += 4;
            }
            else {
                dialoguePtr += 1;
            }
        }

        // 추출한 부분 출력
        if (charsToPrint > 0) {
            for (int j = 0; j < charsToPrint; j++) {
                putchar(dialogue[lineStart + j]);
            }
            lineStart += charsToPrint;
        }

        // 남은 공간 채우기
        remaining = CONTENT_WIDTH - 2 - currentWidth;
        for (int j = 0; j < remaining; j++) printf(" ");
        printf("│");

        lineNum++;

        // 대화가 끝났으면 종료
        if (dialogue[lineStart] == '\0') break;
    }

    // 남은 빈 줄 채우기
    for (int i = 3 + lineNum; i < h - 3; i++) {
        console_goto(x, y + i);
        printf("│                                  │");
    }

    // 구분선
    console_goto(x, y + h - 3);
    printf("├");
    for (int i = 1; i < w - 1; i++) printf("─");
    printf("┤");

    // ★ 버튼 안내 개선
    console_goto(x, y + h - 2);
    char buttonText[64];
    if (npc->currentDialogue < npc->dialogueCount - 1) {
        // 아직 대화가 남았을 때
        if (npc->canTrade) {
            snprintf(buttonText, sizeof(buttonText), " [0]다음  [T]거래  [X]닫기");
        }
        else {
            snprintf(buttonText, sizeof(buttonText), " [0]다음  [X]닫기");
        }
    }
    else {
        // 마지막 대화일 때
        if (npc->canTrade) {
            snprintf(buttonText, sizeof(buttonText), " [0]끝  [T]거래  [X]닫기");
        }
        else {
            snprintf(buttonText, sizeof(buttonText), " [0]끝  [X]닫기");
        }
    }

    printf("│%s", buttonText);
    remaining = CONTENT_WIDTH - display_width(buttonText);
    for (int i = 0; i < remaining; i++) printf(" ");
    printf("│");

    // 하단 테두리
    console_goto(x, y + h - 1);
    printf("└");
    for (int i = 1; i < w - 1; i++) printf("─");
    printf("┘");
}

// ★ 대화창 영역 지우기 (상태창/장비창 복원)
void ui_clear_dialogue_area(int x, int y, int w, int h) {
    for (int i = 0; i < h; i++) {
        console_goto(x, y + i);
        for (int j = 0; j < w; j++) {
            printf(" ");
        }
    }
}
