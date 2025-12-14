#define _CRT_SECURE_NO_WARNINGS
#include "cinematic.h"
#include "../core/ui/ui.h"
#include "../world/glyph.h"
#include "../world/map_data.h" // For get_stage_data
#include "raylib.h"
#include <stdio.h>
#include <string.h>


// ============================================
// Global Cinematics (Moved from story.h)
// ============================================

// Key buffering for peeking (Moved here for scope visibility)
static int HACK_bufferedKey = 0;

// --- Intro ---
static const CinematicLine INTRO_LINES[] = {
	{ "", STYLE_NORMAL, 500 },
	{ "갑자기 당신은 눈을 떴다...", STYLE_TYPEWRITER, 800 },
	{ "", STYLE_NORMAL, 300 },
	{ "이곳은...", STYLE_TYPEWRITER, 800 },
	{ "", STYLE_NORMAL, 300 },
	{ "성균관대역....?", STYLE_TYPEWRITER, 1000 },
	{ "", STYLE_NORMAL, 300 },
	{ "당신은 몸을 일으켜 주위를 둘러본다...", STYLE_TYPEWRITER, 0 },
};

static const Cinematic STORY_INTRO = {
    "★  M E T R O   H E R O  ★",
    INTRO_LINES,
    sizeof(INTRO_LINES) / sizeof(INTRO_LINES[0]),
    30,     // 타이핑 속도
    1,      // 스킵 힌트 표시
    COLOR_BRIGHT_CYAN,
    COLOR_WHITE
};

// --- Good Ending ---
static const CinematicLine ENDING_GOOD_LINES[] = {
    { "", STYLE_NORMAL, 500 },
    { "당신은 해냈다.", STYLE_TYPEWRITER, 1000 },
    { "", STYLE_NORMAL, 500 },
    { "지하철의 어둠은 물러가고,", STYLE_TYPEWRITER, 800 },
    { "시민들은 다시 안전하게 이동할 수 있게 되었다.", STYLE_TYPEWRITER, 1000 },
    { "", STYLE_NORMAL, 500 },
    { "하지만 영웅의 이야기는", STYLE_TYPEWRITER, 600 },
    { "누구에게도 알려지지 않았다...", STYLE_TYPEWRITER, 1000 },
    { "", STYLE_NORMAL, 800 },
    { "그것이 진정한 영웅의 길.", STYLE_TYPEWRITER, 0 },
};

static const Cinematic STORY_ENDING_GOOD = {
    "★  T H E   E N D  ★",
    ENDING_GOOD_LINES,
    sizeof(ENDING_GOOD_LINES) / sizeof(ENDING_GOOD_LINES[0]),
    35,
    1,
    COLOR_BRIGHT_YELLOW,
    COLOR_WHITE
};

// --- Bad Ending ---
static const CinematicLine ENDING_BAD_LINES[] = {
    { "", STYLE_NORMAL, 500 },
    { "어둠이 당신을 삼켰다...", STYLE_TYPEWRITER, 1000 },
    { "", STYLE_NORMAL, 500 },
    { "지하철의 수호자는 쓰러졌고,", STYLE_TYPEWRITER, 800 },
    { "도시는 영원한 어둠 속에 남겨졌다.", STYLE_TYPEWRITER, 1000 },
    { "", STYLE_NORMAL, 800 },
    { "하지만 희망은 사라지지 않는다.", STYLE_TYPEWRITER, 600 },
    { "언젠가 새로운 영웅이 나타날 것이다...", STYLE_TYPEWRITER, 0 },
};

static const Cinematic STORY_ENDING_BAD = {
    "G A M E   O V E R",
    ENDING_BAD_LINES,
    sizeof(ENDING_BAD_LINES) / sizeof(ENDING_BAD_LINES[0]),
    40,
    1,
    COLOR_BRIGHT_RED,
    COLOR_GRAY
};


// ============================================
// 내부 상수 (Layout.h 기반 동적 조정)
// ============================================

#define CINE_X      SCREEN_PADDING_LEFT
#define CINE_Y      SCREEN_PADDING_TOP

// 테두리가 그려질 실제 크기 (화면 전체 덮지 않고 패딩 감안)
// 우측/하단 여백도 좌측/상단과 비슷하게 줌
#define CINE_WIDTH  (SCREEN_W - CINE_X - 4) 
#define CINE_HEIGHT (SCREEN_H - CINE_Y - 2)

// 텍스트 표시 영역 (테두리 내부)
#define CONTENT_X       (CINE_X + 4)
#define CONTENT_Y       (CINE_Y + 3)
#define CONTENT_WIDTH   (CINE_WIDTH - 8)
#define CONTENT_HEIGHT  (CINE_HEIGHT - 6)

// 하단 안내 메시지 위치
#define CONTENT_MAX_Y   (CONTENT_Y + CONTENT_HEIGHT - 1)

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

// ms 단위 대기 helper that handles window closing
static void cinematic_delay(int ms) {
    if (ms <= 0) return;
    
    // Use Timer based wait for better input handling
    double startTime = GetTime();
    while ((GetTime() - startTime) * 1000.0 < ms) {
        PollInputEvents();
        
        // Capture key if pressed during delay (Robust filtering)
        if (HACK_bufferedKey == 0) {
             int k = GetKeyPressed();
             if (k != 0) HACK_bufferedKey = k;
        }

        if (WindowShouldClose()) {
            CloseWindow();
            exit(0); // Force exit on close request during cinematic
        }
        
        // Yield slightly to prevent 100% CPU, but keep responsive
        WaitTime(0.005f); 
    }
}

// Key buffering for peeking declared at top of file

static int internal_poll_key(void) {
    if (HACK_bufferedKey == 0) {
        int k = GetKeyPressed();
        if (k != 0) HACK_bufferedKey = k;
    }
    return HACK_bufferedKey;
}

// 키 입력 체크 (논블로킹) - Peeks
static int cinematic_key_pressed(void) {
    if (WindowShouldClose()) {
        CloseWindow();
        exit(0);
    }
    return internal_poll_key() != 0;
}

// 키 입력 읽기 - Consumes and Maps
static int cinematic_get_key(void) {
    int k = internal_poll_key();
    HACK_bufferedKey = 0; // Consume

    // Map Raylib keys to legacy ASCII/Scancodes
    if (k == KEY_SPACE) return ' ';
    if (k == KEY_ENTER || k == KEY_KP_ENTER) return 13;
    if (k == KEY_ESCAPE) return 27;
    
    return k;
}

// ============================================
// 테두리 그리기
// ============================================

void cinematic_draw_frame(const char* borderColor) {
    ui_begin_texture_mode();
    ClearBackground(BLACK);

    const char* color = borderColor ? borderColor : COLOR_WHITE;

    int right = CINE_X + CINE_WIDTH - 1;
    int bottom = CINE_Y + CINE_HEIGHT - 1;

    // 상단 테두리
    ui_draw_str_at(CINE_X, CINE_Y, "╔", color);
    for (int i = CINE_X + 1; i < right; i++) ui_draw_str_at(i, CINE_Y, "═", color);
    ui_draw_str_at(right, CINE_Y, "╗", color);

    // 좌우 테두리
    for (int y = CINE_Y + 1; y < bottom; y++) {
        ui_draw_str_at(CINE_X, y, "║", color);
        ui_draw_str_at(right, y, "║", color);
    }

    // 하단 테두리
    ui_draw_str_at(CINE_X, bottom, "╚", color);
    for (int i = CINE_X + 1; i < right; i++) ui_draw_str_at(i, bottom, "═", color);
    ui_draw_str_at(right, bottom, "╝", color);
    
    ui_end_texture_mode();
    ui_present();
}

// ============================================
// 중앙 정렬 텍스트 출력
// ============================================

void cinematic_print_centered(int y, const char* text, const char* color) {
    // This function is usually called and then presented by caller, OR should present itself?
    // In legacy it just drew to buffer.
    // To support immediate mode:
    ui_begin_texture_mode();
    
    int textWidth = display_width(text);
    int x = CINE_X + (CINE_WIDTH - textWidth) / 2; // Add global offset
    if (x < CONTENT_X) x = CONTENT_X;

    ui_draw_str_at(x, y, text, color);
    
    ui_end_texture_mode();
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
        
        // Draw (Batched per char is fine basically, but need begin/end)
        ui_begin_texture_mode();
        ui_draw_str_at(curX, y, buf, color);
        ui_end_texture_mode();
        
        ui_present();
        
        s += charLen;
        curX += charWidth;

        cinematic_delay(charDelay);
    }
}

// ============================================
// 스크롤 텍스트 (스타워즈 스타일)
// ============================================

void cinematic_scroll_text(const char** lines, int lineCount, int speed) {
    // 스크롤 영역: 화면 중앙 부분
    int scrollTop = CINE_Y + 6;
    int scrollBottom = CINE_Y + CINE_HEIGHT - 6;
    int scrollHeight = scrollBottom - scrollTop;

    // 모든 라인을 아래에서 위로 스크롤
    for (int offset = scrollHeight; offset > -lineCount; offset--) {
        // ESC 스킵 체크
        if (cinematic_key_pressed()) {
            int key = cinematic_get_key();
            if (key == 27) return;
        }

        // 스크롤 영역 지우기
        ui_begin_texture_mode();
        for (int y = scrollTop; y < scrollBottom; y++) {
             ui_draw_str_at(CONTENT_X, y, "                                                                                                                ", NULL); 
        }
        ui_end_texture_mode();
        // Don't present yet, draw lines first

        // 현재 보이는 라인들 출력
        for (int i = 0; i < lineCount; i++) {
            int screenY = scrollTop + offset + i;

            if (screenY >= scrollTop && screenY < scrollBottom) {
                
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

                // Actually cinematic_print_centered now batches internally!
                // This is problematic if we want to batch whole frame.
                // We should unwrap print_centered or just let it happen (slight overhead but okay for 10 lines)
                // Wait, print_centered does Begin/End.
                // If we do Begin here, nested Begin is invalid in Raylib?
                // Raylib supports nested texture mode? ALLOWED but warnings/perf issues if targeting same?
                // Actually Raylib: EndTextureMode() pops from stack? No, it just ends current.
                // Safe bet: Don't nest.
                // Let print_centered handle its own drawing.
                cinematic_print_centered(screenY, lines[i], fadeColor);
            }
        }

        ui_present();
        cinematic_delay(speed);
    }
}

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
                ui_present(); // Explicit present mostly needed after updates
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
        cinematic_print_centered(CINE_Y + 2, cine->title, COLOR_BRIGHT_YELLOW);

        // 제목 아래 구분선
        char sep[1024] = "";
        for (int i = 0; i < CONTENT_WIDTH; i++) strcat(sep, "─");
        
        ui_draw_str_at(CONTENT_X, CINE_Y + 3, sep, COLOR_YELLOW);
        ui_present();
    }

    // ★ 하단 안내 메시지 표시 (처음부터 항상 표시)
    if (cine->showSkipHint) {
        cinematic_print_centered(CINE_HEIGHT - 3,
            "[SPACE] 계속    [ESC] 스킵", COLOR_GRAY);
    }

    // 각 라인 처리
    int currentY = cine->title ? (CONTENT_Y + 2) : CONTENT_Y;
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
                int x = CINE_X + (CINE_WIDTH - textWidth) / 2;
                if (x < CONTENT_X) x = CONTENT_X;

                const char* color = cine->textColor ? cine->textColor : COLOR_WHITE;
                
                cinematic_print_typewriter(x, currentY, line->text, color,
                    cine->scrollSpeed > 0 ? cine->scrollSpeed : 30);
                
                currentY += 2;
                break;
            }

            case STYLE_SCROLL_UP:
                cinematic_scroll_text((const char**)line->text, 10, cine->scrollSpeed); 
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
// 미리 정의된 시네마틱 (Using StageData or Globals)
// ============================================

void cinematic_play_intro(void) {
    cinematic_play(&STORY_INTRO);
}

void cinematic_play_stage_start(int stageNum) {
    const StageData* data = get_stage_data(stageNum);
    if (data && data->intro) {
        cinematic_play(data->intro);
    }
}

void cinematic_play_stage_clear(int stageNum) {
    const StageData* data = get_stage_data(stageNum);
    if (data && data->outro) {
        cinematic_play(data->outro);
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
