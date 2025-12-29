#define _CRT_SECURE_NO_WARNINGS
#include "cinematic.h"
#include "../core/ui/ui.h"
#include "../core/audio/audio.h" // Added
#include "../world/glyph.h"
#include "../world/map_data.h" // For get_stage_data
#include "raylib.h"
#include <stdio.h>
#include <string.h>


// ============================================
// Global Cinematics (Moved from story.h)
// ============================================

// Cinematic Image Texture
static Texture2D cinematicTexture = { 0 };

static void cinematic_load_image(const char* path) {
    if (!path) return;
    
    // 이미 로드된 이미지가 다르면 해제 후 로드 (또는 같은 이미지면 스킵)
    // 여기선 단순하게 매번 로드하거나 체크.
    // 기존 텍스처 해제
    if (cinematicTexture.id != 0) {
        UnloadTexture(cinematicTexture);
        cinematicTexture.id = 0;
    }

    cinematicTexture = LoadTexture(path);
    if (cinematicTexture.id != 0) {
        SetTextureFilter(cinematicTexture, TEXTURE_FILTER_BILINEAR);
    }
}

static void cinematic_unload_image(void) {
    if (cinematicTexture.id != 0) {
        UnloadTexture(cinematicTexture);
        cinematicTexture.id = 0;
    }
}

// Key buffering for peeking (Moved here for scope visibility)
static int HACK_bufferedKey = 0;

// static const CinematicLine INTRO_LINES[] = {
// 	{ "", STYLE_NORMAL, 500, "assets/cinematic/1_1.png" }, // ★ Image Start
// 	{ "갑자기 당신은 눈을 떴다...", STYLE_TYPEWRITER, 800, NULL },
// 	{ "", STYLE_NORMAL, 300, NULL },
// 	{ "이곳은...", STYLE_TYPEWRITER, 800, NULL },
// 	{ "", STYLE_NORMAL, 300, NULL },
// 	{ "성균관대역....?", STYLE_TYPEWRITER, 1000, NULL },
// 	{ "", STYLE_NORMAL, 300, NULL },
// 	{ "당신은 몸을 일으켜 주위를 둘러본다...", STYLE_TYPEWRITER, 0, NULL },
// };

static const CinematicLine INTRO_LINES[] = {
    { "", STYLE_NORMAL, 500, "assets/cinematic/1_1.png" }, // ★ Image Start

    { "눈을 떠보니... 어? 여기는 성균관대역?", STYLE_TYPEWRITER, 400, NULL },
    { "", STYLE_NORMAL, 200, NULL },

    { "그런데 뭔가 이상해! 불이 깜빡거리고 아무도 없어!", STYLE_TYPEWRITER, 400, NULL },    
    { "", STYLE_NORMAL, 200, NULL },

    { "바닥에는 쓰레기가 뒹굴고... 으스스한 느낌이야.", STYLE_TYPEWRITER, 500, NULL },
    { "", STYLE_NORMAL, 200, NULL },

    { "큰일 났다! 지하철역이 괴물들에게 점령당한 것 같아!", STYLE_TYPEWRITER, 400, NULL },
    { "", STYLE_NORMAL, 200, NULL },

    { "하지만 난 겁쟁이가 아니지!", STYLE_TYPEWRITER, 300, NULL },
    { "", STYLE_NORMAL, 200, NULL },

    { "용기를 내서 이곳을 탈출하고 사람들을 구해야 해!", STYLE_TYPEWRITER, 400, NULL },
    { "", STYLE_NORMAL, 200, NULL },

    { "가자! 메트로 히어로 출동!", STYLE_TYPEWRITER, 800, NULL },
};

static const Cinematic STORY_INTRO = {
    "★  M E T R O   H E R O  ★",
    INTRO_LINES,
    sizeof(INTRO_LINES) / sizeof(INTRO_LINES[0]),
    15,     // 타이핑 속도 (빠름)
    1,      // 스킵 힌트 표시
    COLOR_BRIGHT_CYAN,
    COLOR_WHITE
};

// --- Good Ending ---
static const CinematicLine ENDING_GOOD_LINES[] = {
    { "", STYLE_NORMAL, 500, NULL },
    { "당신은 해냈다.", STYLE_TYPEWRITER, 1000, NULL },
    { "", STYLE_NORMAL, 500, NULL },
    { "지하철의 어둠은 물러가고,", STYLE_TYPEWRITER, 800, NULL },
    { "시민들은 다시 안전하게 이동할 수 있게 되었다.", STYLE_TYPEWRITER, 1000, NULL },
    { "", STYLE_NORMAL, 500, NULL },
    { "하지만 영웅의 이야기는", STYLE_TYPEWRITER, 600, NULL },
    { "누구에게도 알려지지 않았다...", STYLE_TYPEWRITER, 1000, NULL },
    { "", STYLE_NORMAL, 800, NULL },
    { "그것이 진정한 영웅의 길.", STYLE_TYPEWRITER, 0, NULL },
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
    { "", STYLE_NORMAL, 500, NULL },
    { "어둠이 당신을 삼켰다...", STYLE_TYPEWRITER, 1000, NULL },
    { "", STYLE_NORMAL, 500, NULL },
    { "지하철의 수호자는 쓰러졌고,", STYLE_TYPEWRITER, 800, NULL },
    { "도시는 영원한 어둠 속에 남겨졌다.", STYLE_TYPEWRITER, 1000, NULL },
    { "", STYLE_NORMAL, 800, NULL },
    { "하지만 희망은 사라지지 않는다.", STYLE_TYPEWRITER, 600, NULL },
    { "언젠가 새로운 영웅이 나타날 것이다...", STYLE_TYPEWRITER, 0, NULL },
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

// Center the cinematic window
#define CINE_WIDTH  186
#define CINE_HEIGHT 47

#define CINE_X      ((SCREEN_W - CINE_WIDTH) / 2)
#define CINE_Y      ((SCREEN_H - CINE_HEIGHT) / 2)

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
        audio_update(); // Keep music playing
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

    // Draw Graphical Border instead of Text Border (Font might lack box chars)
    // Convert grid coords to pixels
    int px = CINE_X * 8;
    int py = CINE_Y * 16;
    int pw = CINE_WIDTH * 8;
    int ph = CINE_HEIGHT * 16;
    
    // Draw thick border
    // Adjust logic to match Color string if possible, or just use white/colored rect
    Color rectColor = GetColorFromAnsi(borderColor);
    
    // Draw outer box
    DrawRectangleLines(px, py, pw, ph, rectColor);
    DrawRectangleLines(px + 1, py + 1, pw - 2, ph - 2, rectColor); // Double thickness
    
    // Legacy text drawing removed to prevent invisible/broken chars
    /*
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
    */
    
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

// ============================================
// 타이핑 효과 텍스트 출력
// ============================================

int cinematic_print_typewriter(int x, int y, const char* text, const char* color, int charDelay) {
    const unsigned char* s = (const unsigned char*)text;
    int curX = x;

    while (*s) {
        // ESC 스킵 체크
        if (cinematic_key_pressed()) {
            int key = cinematic_get_key();
            if (key == 27) {  // ESC -> Scene Skip
                return 1;
            }
            if (key == ' ' || key == 13) { // SPACE/ENTER -> Skip Typing (Finish line)
                // 남은 텍스트 한번에 출력
                ui_draw_str_at(curX, y, (const char*)s, color);
                ui_present();
                return 0; // Continue scene normally
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
    return 0;
}

// ============================================
// 스크롤 텍스트 (스타워즈 스타일)
// ============================================

int cinematic_scroll_text(const char** lines, int lineCount, int speed) {
    // 스크롤 영역: 화면 중앙 부분
    int scrollTop = CINE_Y + 6;
    int scrollBottom = CINE_Y + CINE_HEIGHT - 6;
    int scrollHeight = scrollBottom - scrollTop;

    // 모든 라인을 아래에서 위로 스크롤
    for (int offset = scrollHeight; offset > -lineCount; offset--) {
        // ESC 스킵 체크
        if (cinematic_key_pressed()) {
            int key = cinematic_get_key();
            if (key == 27) return 1;
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
    return 0;
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

    // ★ 초기 이미지 로드 (첫 라인부터 체크하므로 루프 안에서 처리됨)
    // 하지만 첫 프레임 전에 그려야 할 수도 있으므로 여기서 미리 그리지 않고 루프 내에서 처리.
    // 단, 배경 클리어(draw_frame) 직후 이미지가 그려져야 함.
    
    // 루프 시작 전 기존 이미지 초기화? (시네마틱 시작 시점)
    cinematic_unload_image();

    for (int i = 0; i < cine->lineCount && !skipped; i++) {
        const CinematicLine* line = &cine->lines[i];

        // ★ 이미지 변경 체크
        if (line->imagePath != NULL) {
            cinematic_load_image(line->imagePath);
        }

        // ★ 이미지 그리기 (매 라인/프레임마다...가 아니라 텍스처 모드 안에서? 아님 ui_present 전에?)
        // ui_present는 화면을 갱신하므로, 매번 이미지를 다시 그려야 할 수 있음.
        // 현재 구조상 '텍스트 타이핑 루프' 내에서도 ui_present가 호출됨.
        // 따라서 이미지는 '배경'으로서 계속 그려져야 함.
        // 하지만 타이핑 함수(cinematic_print_typewriter)는 ui_present만 호출함.
        // ui_present 내부에서 전체를 다시 그리지 않는다면(즉 백버퍼가 유지된다면) 한 번 그리면 됨.
        // MetroHero의 ui_present는 TextureMode의 텍스처를 화면에 뿌리는 역할일 확률이 높음.
        // ui_begin_texture_mode()로 그린 것은 텍스처에 남음.
        // 즉, 이미지가 변경되었을 때 한 번만 "덧그리면" 됨.
        
        if (line->imagePath != NULL) {
             ui_begin_texture_mode();
             // 배경 이미지를 그릴 위치: 상단 중앙 (타이틀 화면과 유사하게)
             // Y=100 (Title Screen)
             // Frame is ClearBackground-ed black.
             if (cinematicTexture.id != 0) {
                // 검은 배경 위, 텍스트 아래.
                // 하지만 이미 텍스트가 있을 수 있으니 주의. (보통 라인 단위로 출력하므로 겹치진 않음, 스크롤 제외)
                
                float targetWidth = 1200.0f;
                float scale = targetWidth / (float)cinematicTexture.width;
                float targetHeight = (float)cinematicTexture.height * scale;

                float imgX = ((float)(SCREEN_W * 8) - targetWidth) / 2.0f;
                float imgY = 100.0f; 
                
                // 프레임 테두리가 덮이지 않게 하려면?
                // 프레임은 처음에 한 번 그려짐. 이미지를 그리면 프레임 내부(또는 위)에 그려짐.
                // 텍스트는 그 위에 그려짐.
                
                // Draw Texture directly
                Rectangle srcRec = { 0.0f, 0.0f, (float)cinematicTexture.width, (float)cinematicTexture.height };
                Rectangle destRec = { imgX, imgY, targetWidth, targetHeight };
                Vector2 origin = { 0.0f, 0.0f };
                
                DrawTexturePro(cinematicTexture, srcRec, destRec, origin, 0.0f, WHITE);
             }
             ui_end_texture_mode();
             // Don't present yet, text will follow
        }

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
                
                // If typewriter returns 1, it means ESC was pressed for full skip
                if (cinematic_print_typewriter(x, currentY, line->text, color,
                    cine->scrollSpeed > 0 ? cine->scrollSpeed : 30)) {
                    skipped = 1;
                }
                
                currentY += 2;
                break;
            }

            case STYLE_SCROLL_UP:
                if (cinematic_scroll_text((const char**)line->text, 10, cine->scrollSpeed)) {
                    skipped = 1;
                }
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
        if (!skipped && line->delayAfter > 0) {
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
        else if (!skipped && line->delayAfter == 0 && line->style != STYLE_NORMAL) {
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
    
    // 이미지 정리
    cinematic_unload_image();
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
