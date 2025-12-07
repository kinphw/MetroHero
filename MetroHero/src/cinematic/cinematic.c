#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "cinematic.h"
#include "../core/ui.h"
#include "../world/glyph.h"
#include "../data/story.h"

// ============================================
// 내부 상수
// ============================================

#define CINE_WIDTH  120
#define CINE_HEIGHT 30

// 텍스트 표시 영역 (테두리 제외)
#define CONTENT_X       4
#define CONTENT_Y       3
#define CONTENT_WIDTH   112
#define CONTENT_HEIGHT  24

// 하단 안내 메시지 위치 (CINE_HEIGHT - 3 = 27)
// 컨텐츠는 이 위치보다 2칸 위까지만 사용 가능 (여유 공간 확보)
#define CONTENT_MAX_Y   (CINE_HEIGHT - 5)  // = 25

// ============================================
// 유틸리티 함수
// ============================================

// 화면 전체 지우기
static void cinematic_clear(void) {
    console_clear_fast();
}

// 지정 위치로 커서 이동
static void cinematic_goto(int x, int y) {
    console_goto(x, y);
}

// ms 단위 대기
static void cinematic_delay(int ms) {
    Sleep(ms);
}

// 키 입력 체크 (논블로킹)
static int cinematic_key_pressed(void) {
    return _kbhit();
}

// 키 입력 읽기
static int cinematic_get_key(void) {
    return _getch();
}

// ============================================
// 테두리 그리기
// ============================================

void cinematic_draw_frame(const char* borderColor) {
    const char* color = borderColor ? borderColor : COLOR_WHITE;

    cinematic_clear();

    // 상단 테두리
    cinematic_goto(0, 0);
    printf("%s", color);
    printf("╔");
    for (int i = 1; i < CINE_WIDTH - 1; i++) printf("═");
    printf("╗");

    // 좌우 테두리
    for (int y = 1; y < CINE_HEIGHT - 1; y++) {
        cinematic_goto(0, y);
        printf("║");
        cinematic_goto(CINE_WIDTH - 1, y);
        printf("║");
    }

    // 하단 테두리
    cinematic_goto(0, CINE_HEIGHT - 1);
    printf("╚");
    for (int i = 1; i < CINE_WIDTH - 1; i++) printf("═");
    printf("╝");

    printf("%s", COLOR_RESET);
}

// ============================================
// 중앙 정렬 텍스트 출력
// ============================================

void cinematic_print_centered(int y, const char* text, const char* color) {
    int textWidth = display_width(text);
    int x = (CINE_WIDTH - textWidth) / 2;
    if (x < CONTENT_X) x = CONTENT_X;

    cinematic_goto(x, y);
    if (color) printf("%s", color);
    printf("%s", text);
    if (color) printf("%s", COLOR_RESET);
}

// ============================================
// 타이핑 효과 텍스트 출력
// ============================================

void cinematic_print_typewriter(int x, int y, const char* text, int charDelay) {
    cinematic_goto(x, y);

    const unsigned char* s = (const unsigned char*)text;

    while (*s) {
        // ESC 스킵 체크
        if (cinematic_key_pressed()) {
            int key = cinematic_get_key();
            if (key == 27) {  // ESC
                // 남은 텍스트 한번에 출력
                printf("%s", (const char*)s);
                return;
            }
        }

        // ANSI 이스케이프 시퀀스 처리
        if (*s == '\033' || *s == 0x1B) {
            // 이스케이프 시퀀스 전체 출력
            while (*s && *s != 'm') {
                putchar(*s++);
            }
            if (*s == 'm') putchar(*s++);
            continue;
        }

        // UTF-8 문자 처리
        if (*s < 128) {
            putchar(*s++);
        }
        else if ((*s & 0xE0) == 0xC0) {
            putchar(*s++);
            if (*s) putchar(*s++);
        }
        else if ((*s & 0xF0) == 0xE0) {
            putchar(*s++);
            if (*s) putchar(*s++);
            if (*s) putchar(*s++);
        }
        else if ((*s & 0xF8) == 0xF0) {
            putchar(*s++);
            if (*s) putchar(*s++);
            if (*s) putchar(*s++);
            if (*s) putchar(*s++);
        }
        else {
            putchar(*s++);
        }

        fflush(stdout);
        cinematic_delay(charDelay);
    }
}

// ============================================
// 스크롤 텍스트 (스타워즈 스타일)
// ============================================

void cinematic_scroll_text(const char** lines, int lineCount, int speed) {
    // 스크롤 영역: 화면 중앙 부분
    int scrollTop = 6;
    int scrollBottom = CINE_HEIGHT - 6;
    int scrollHeight = scrollBottom - scrollTop;

    // 모든 라인을 아래에서 위로 스크롤
    for (int offset = scrollHeight; offset > -lineCount; offset--) {
        // ESC 스킵 체크
        if (cinematic_key_pressed()) {
            int key = cinematic_get_key();
            if (key == 27) return;
        }

        // 스크롤 영역 지우기
        for (int y = scrollTop; y < scrollBottom; y++) {
            cinematic_goto(CONTENT_X, y);
            for (int i = 0; i < CONTENT_WIDTH; i++) printf(" ");
        }

        // 현재 보이는 라인들 출력
        for (int i = 0; i < lineCount; i++) {
            int screenY = scrollTop + offset + i;

            if (screenY >= scrollTop && screenY < scrollBottom) {
                // 중앙에서 멀어질수록 어두운 색상
                int distFromCenter = abs(screenY - (scrollTop + scrollHeight / 2));
                const char* fadeColor;

                if (distFromCenter < 3) {
                    fadeColor = COLOR_BRIGHT_WHITE;
                }
                else if (distFromCenter < 5) {
                    fadeColor = COLOR_WHITE;
                }
                else if (distFromCenter < 7) {
                    fadeColor = COLOR_GRAY;
                }
                else {
                    fadeColor = COLOR_DARK_GRAY;
                }

                cinematic_print_centered(screenY, lines[i], fadeColor);
            }
        }

        cinematic_delay(speed);
    }
}

// ============================================
// 키 입력 대기
// ============================================

int cinematic_wait_key(void) {
    // ★ 안내 메시지는 cinematic_play()에서 이미 표시됨
    // 여기서는 키 입력만 대기

    while (1) {
        int key = cinematic_get_key();

        if (key == ' ' || key == 13) {  // SPACE 또는 ENTER
            return 0;  // 계속
        }
        else if (key == 27) {  // ESC
            return 1;  // 스킵
        }
    }
}

// ============================================
// 페이드 효과
// ============================================

void cinematic_fade_in(int duration) {
    // 단계별 페이드 인 (3단계)
    int stepDelay = duration / 3;

    // 어두운 상태에서 시작
    // (실제 구현은 전체 화면 색상 변경이 어려우므로 간단히 딜레이만)
    cinematic_delay(stepDelay);
    cinematic_delay(stepDelay);
    cinematic_delay(stepDelay);
}

void cinematic_fade_out(int duration) {
    int stepDelay = duration / 3;
    cinematic_delay(stepDelay);
    cinematic_delay(stepDelay);
    cinematic_delay(stepDelay);
}

// ============================================
// 메인 시네마틱 재생 함수
// ============================================

void cinematic_play(const Cinematic* cine) {
    if (!cine || !cine->lines || cine->lineCount <= 0) return;

    // 화면 준비
    cinematic_draw_frame(cine->borderColor);

    // 제목 표시
    if (cine->title) {
        cinematic_print_centered(2, cine->title, COLOR_BRIGHT_YELLOW);

        // 제목 아래 구분선
        cinematic_goto(4, 3);
        printf("%s", COLOR_YELLOW);
        for (int i = 0; i < CONTENT_WIDTH; i++) printf("─");
        printf("%s", COLOR_RESET);
    }

    // ★ 하단 안내 메시지 표시 (처음부터 항상 표시)
    if (cine->showSkipHint) {
        cinematic_print_centered(CINE_HEIGHT - 3,
            "[SPACE] 계속    [ESC] 스킵", COLOR_GRAY);
    }

    // 각 라인 처리
    int currentY = cine->title ? 5 : 3;
    int skipped = 0;

    for (int i = 0; i < cine->lineCount && !skipped; i++) {
        const CinematicLine* line = &cine->lines[i];

        // ★ ESC 키로 언제든지 스킵 가능
        if (cinematic_key_pressed()) {
            int key = cinematic_get_key();
            if (key == 27) {  // ESC
                skipped = 1;
                break;
            }
        }

        // ★ 하단 안내 메시지 영역 보호: Y가 CONTENT_MAX_Y를 넘으면 출력 중단
        if (currentY > CONTENT_MAX_Y) {
            break;
        }

        switch (line->style) {
            case STYLE_TITLE:
                cinematic_print_centered(currentY, line->text, COLOR_BRIGHT_YELLOW);
                currentY += 2;
                break;

            case STYLE_SUBTITLE:
                cinematic_print_centered(currentY, line->text, COLOR_YELLOW);
                currentY += 2;
                break;

            case STYLE_TYPEWRITER: {
                int textWidth = display_width(line->text);
                int x = (CINE_WIDTH - textWidth) / 2;
                if (x < CONTENT_X) x = CONTENT_X;

                const char* color = cine->textColor ? cine->textColor : COLOR_WHITE;
                printf("%s", color);
                cinematic_print_typewriter(x, currentY, line->text,
                    cine->scrollSpeed > 0 ? cine->scrollSpeed : 30);
                printf("%s", COLOR_RESET);
                currentY += 2;
                break;
            }

            case STYLE_SCROLL_UP:
                // 스크롤은 별도 처리 필요 (라인 배열 전체 전달)
                break;

            case STYLE_FADE_IN:
                cinematic_fade_in(500);
                cinematic_print_centered(currentY, line->text,
                    cine->textColor ? cine->textColor : COLOR_WHITE);
                currentY += 2;
                break;

            case STYLE_NORMAL:
            default:
                cinematic_print_centered(currentY, line->text,
                    cine->textColor ? cine->textColor : COLOR_WHITE);
                currentY += 1;
                break;
        }

        // 라인 후 대기
        if (line->delayAfter > 0) {
            // ★ 딜레이 중에도 ESC로 스킵 가능하도록 짧은 단위로 나눠서 체크
            int remainingDelay = line->delayAfter;
            const int checkInterval = 50;  // 50ms마다 체크

            while (remainingDelay > 0 && !skipped) {
                int waitTime = (remainingDelay < checkInterval) ? remainingDelay : checkInterval;
                cinematic_delay(waitTime);
                remainingDelay -= waitTime;

                if (cinematic_key_pressed()) {
                    int key = cinematic_get_key();
                    if (key == 27) {  // ESC
                        skipped = 1;
                        break;
                    }
                }
            }
        }
        else if (line->delayAfter == 0 && line->style != STYLE_NORMAL) {
            // 키 입력 대기
            skipped = cinematic_wait_key();
        }
    }

    // 스킵하지 않았으면 최종 키 대기
    if (!skipped && cine->showSkipHint) {
        cinematic_wait_key();
    }

    // 페이드 아웃
    cinematic_fade_out(300);
}

// ============================================
// 미리 정의된 시네마틱
// ============================================

void cinematic_play_intro(void) {
    cinematic_play(&STORY_INTRO);
}

void cinematic_play_stage_start(int stageNum) {
    if (stageNum >= 1 && stageNum <= 3) {
        cinematic_play(&STORY_STAGES[stageNum - 1]);
    }
}

void cinematic_play_stage_clear(int stageNum) {
    if (stageNum >= 1 && stageNum <= 3) {
        cinematic_play(&STORY_STAGE_CLEAR[stageNum - 1]);
    }
}

void cinematic_play_ending(int endingType) {
    if (endingType == 0) {
        cinematic_play(&STORY_ENDING_GOOD);
    }
    else {
        cinematic_play(&STORY_ENDING_BAD);
    }
}
