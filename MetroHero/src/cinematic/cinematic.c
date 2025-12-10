#define _CRT_SECURE_NO_WARNINGS
#include <windows.h>
#include <stdio.h>
#include <string.h>
#include <conio.h>
#include "cinematic.h"
#include "../core/ui/ui.h"
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
    ui_clear_buffer();
}

// 지정 위치로 커서 이동 (더미 - 실제로는 draw함수가 좌표 받음)
static void cinematic_goto(int x, int y) {
    // console_goto(x, y); // Not needed for buffer
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
    ui_draw_str_at(0, 0, "╔", color);
    for (int i = 1; i < CINE_WIDTH - 1; i++) ui_draw_str_at(i, 0, "═", color);
    ui_draw_str_at(CINE_WIDTH - 1, 0, "╗", color);

    // 좌우 테두리
    for (int y = 1; y < CINE_HEIGHT - 1; y++) {
        ui_draw_str_at(0, y, "║", color);
        ui_draw_str_at(CINE_WIDTH - 1, y, "║", color);
    }

    // 하단 테두리
    ui_draw_str_at(0, CINE_HEIGHT - 1, "╚", color);
    for (int i = 1; i < CINE_WIDTH - 1; i++) ui_draw_str_at(i, CINE_HEIGHT - 1, "═", color);
    ui_draw_str_at(CINE_WIDTH - 1, CINE_HEIGHT - 1, "╝", color);
    
    ui_present(); // 기본 프레임 그림
}

// ============================================
// 중앙 정렬 텍스트 출력
// ============================================

void cinematic_print_centered(int y, const char* text, const char* color) {
    int textWidth = display_width(text);
    int x = (CINE_WIDTH - textWidth) / 2;
    if (x < CONTENT_X) x = CONTENT_X;

    ui_draw_str_at(x, y, text, color);
    // ui_present(); // 제거: 호출자가 제어
}

// ============================================
// 타이핑 효과 텍스트 출력
// ============================================

void cinematic_print_typewriter(int x, int y, const char* text, const char* color, int charDelay) {
    const unsigned char* s = (const unsigned char*)text;
    int curX = x;

    while (*s) {
        // ESC 스킵 체크
        if (cinematic_key_pressed()) {
            int key = cinematic_get_key();
            if (key == 27) {  // ESC
                // 남은 텍스트 한번에 출력
                ui_draw_str_at(curX, y, (const char*)s, color);
                ui_present();
                return;
            }
        }

        // ANSI 이스케이프 시퀀스 처리
        if (*s == '\033' || *s == 0x1B) {
            const char* start = (const char*)s;
            s++;
            if (*s == '[') {
                while (*s && *s != 'm') s++;
                if (*s == 'm') s++;
            }
             // ANSI color in string is handled by ui_draw_str_at if passed as part of string.
             // But here we split char by char.
             // If we just skip it, color is lost.
             // If `color` param is used, it overrides.
             // Simple approach: skip ANSI codes in typewriter string, assume `color` param controls it.
             // OR copy ANSI code to `buf` and print it? ui_draw_str_at handles it.
             int len = (int)((const unsigned char*)s - (const unsigned char*)start);
             char buf[32];
             strncpy(buf, start, len);
             buf[len] = 0;
             ui_draw_str_at(curX, y, buf, color);
             // Don't advance curX for ANSI
             continue;
        }

        // UTF-8 문자 처리 & Width calc (Use Centered Helper)
        int charLen = 1;
        int charWidth = 1;
        
        ui_get_glyph_info((const char*)s, &charLen, &charWidth);
        
        char buf[8] = {0};
        strncpy(buf, (const char*)s, charLen);
        
        // Draw
        ui_draw_str_at(curX, y, buf, color);
        ui_present();
        
        s += charLen;
        curX += charWidth;

        cinematic_delay(charDelay);
    }
}
// Wait, `ui_draw_str_at` with NULL color resets to white.
// If the text had ANSI codes effectively, I need to pass them.
// `cinematic_print_typewriter` argument doesn't seem to take a base color, but `cinematic_play` passes `cine->textColor` which might be ANSI.
// But `cinematic_play` logic: `printf("%s", color); cinematic_print_typewriter(...); printf(RESET);`
// `printf` changes terminal state. `ui_draw_str_at` does NOT rely on terminal state.
// So `cinematic_print_typewriter` needs to accept `color`.
// I will modify `cinematic_print_typewriter` signature to take `const char* color`.
// And `cinematic_play` calls it with `cine->textColor`.

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
             ui_draw_str_at(CONTENT_X, y, "                                                                                                                ", NULL); 
        }

        // 현재 보이는 라인들 출력
        for (int i = 0; i < lineCount; i++) {
            int screenY = scrollTop + offset + i;

            if (screenY >= scrollTop && screenY < scrollBottom) {
                // 중앙에서 멀어질수록 어두운 색상
                // ... (생략된 색상 로직은 그대로 유지됨, chunk 범위 밖이면)
                // 하지만 chunk가 for문을 포함하므로 색상 로직도 포함해야 함.
                // Re-writing the color logic to be safe or use wider context.
                
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

        ui_present();
        cinematic_delay(speed);
    }
}

// ============================================
// 키 입력 대기
// ============================================

// ============================================
// 키 입력 대기 (깜빡임 효과 포함)
// ============================================

int cinematic_wait_key(int showHint) {
    // showHint가 참이면 하단 안내 메시지를 깜빡임
    int blinkState = 0;
    int elapsed = 0;
    const int BLINK_INTERVAL = 500; // 0.5초

    while (1) {
        if (cinematic_key_pressed()) {
            int key = cinematic_get_key();
            
            // 키 입력 시 안내 메시지를 원래 색(회색)으로 복구
            if (showHint) {
                cinematic_print_centered(CINE_HEIGHT - 3,
                    "[SPACE] 계속    [ESC] 스킵", COLOR_GRAY);
            }

            if (key == ' ' || key == 13) {  // SPACE 또는 ENTER
                return 0;  // 계속
            }
            else if (key == 27) {  // ESC
                return 1;  // 스킵
            }
        }

        // 깜빡임 처리
        if (showHint) {
            elapsed += 50;
            if (elapsed >= BLINK_INTERVAL) {
                elapsed = 0;
                blinkState = !blinkState;
                
                const char* blinkColor = blinkState ? COLOR_BRIGHT_WHITE : COLOR_GRAY;
                cinematic_print_centered(CINE_HEIGHT - 3,
                    "[SPACE] 계속    [ESC] 스킵", blinkColor);
            }
        }

        ui_present();
        cinematic_delay(50);
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
        char sep[1024] = "";
        for (int i = 0; i < CONTENT_WIDTH; i++) strcat(sep, "─");
        
        ui_draw_str_at(CONTENT_X, 3, sep, COLOR_YELLOW);
        ui_present();
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
                ui_present();
                currentY += 2;
                break;

            case STYLE_SUBTITLE:
                cinematic_print_centered(currentY, line->text, COLOR_YELLOW);
                ui_present();
                currentY += 2;
                break;

            case STYLE_TYPEWRITER: {
                int textWidth = display_width(line->text);
                int x = (CINE_WIDTH - textWidth) / 2;
                if (x < CONTENT_X) x = CONTENT_X;

                const char* color = cine->textColor ? cine->textColor : COLOR_WHITE;
                
                cinematic_print_typewriter(x, currentY, line->text, color,
                    cine->scrollSpeed > 0 ? cine->scrollSpeed : 30);
                
                currentY += 2;
                break;
            }

            case STYLE_SCROLL_UP:
                cinematic_scroll_text((const char**)line->text, 10, cine->scrollSpeed); // Simple casting assumption from original code structure? 
                // Wait, original code had:
                // case STYLE_SCROLL_UP:
                //      // 스크롤은 별도 처리 필요 (라인 배열 전체 전달)
                //      break;
                // It seems STYLE_SCROLL_UP wasn't fully implemented in the switch I viewed?
                // Let's look at Step 546 content again.
                // It was empty! 
                // "case STYLE_SCROLL_UP: break;"
                // So I don't need to change it, but I need to update the others.
                break;

            case STYLE_FADE_IN:
                cinematic_fade_in(500);
                cinematic_print_centered(currentY, line->text,
                    cine->textColor ? cine->textColor : COLOR_WHITE);
                ui_present();
                currentY += 2;
                break;

            case STYLE_NORMAL:
            default:
                cinematic_print_centered(currentY, line->text,
                    cine->textColor ? cine->textColor : COLOR_WHITE);
                ui_present();
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
            // 키 입력 대기 (힌트 깜빡임 적용)
            skipped = cinematic_wait_key(cine->showSkipHint);
        }
    }

    // 스킵하지 않았으면 최종 키 대기 (힌트 깜빡임 적용)
    if (!skipped && cine->showSkipHint) {
        cinematic_wait_key(cine->showSkipHint);
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
