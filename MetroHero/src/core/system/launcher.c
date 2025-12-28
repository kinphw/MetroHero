#include "launcher.h"
#include "raylib.h"
#include "../ui/ui.h" // For ui_begin_texture_mode etc if needed, or just raylib
#include "../ui/layout.h"
#include "../ui/text/render.h"

// 타이틀 이미지 텍스처 (정적 캐싱)
static Texture2D titleTexture = { 0 };

static void load_title_image(void) {
    if (titleTexture.id == 0) {
        titleTexture = LoadTexture("assets/title.png");
        if (titleTexture.id == 0) {
            // 실패 시 로깅?
        } else {
             // 텍스처 필터 설정 (선택사항)
             SetTextureFilter(titleTexture, TEXTURE_FILTER_BILINEAR);
        }
    }
}

static void unload_title_image(void) {
    // 텍스처는 프로그램 종료 시까지 유지하거나, 메뉴 나갈 때 해제?
    // 반복 호출 시 재로딩 방지. 여기선 명시적 해제 함수가 필요하다면 사용.
}

GameMode game_menu(void) {
    load_title_image();
    
    // 메뉴 루프
    while (!WindowShouldClose()) {
        // --- Input Handling ---
        if (IsKeyPressed(KEY_ONE)) return GAME_NEW;
        if (IsKeyPressed(KEY_TWO)) return GAME_LOAD;
        if (IsKeyPressed(KEY_Q)) return GAME_EXIT;

        // --- Drawing ---
        BeginDrawing();
        ClearBackground(BLACK);

        int screenW = SCREEN_W * 8; // 1680
        // int screenH = SCREEN_H * 16; // 880

        // 1. Title Image
        if (titleTexture.id != 0) {
            // 중앙 정렬
            int imgX = (screenW - titleTexture.width) / 2;
            int imgY = 100; // 상단 여백
            DrawTexture(titleTexture, imgX, imgY, WHITE);
            
            // 텍스트 타이틀 (이미지 아래)
            // DrawText("METRO HERO", ...); // 사용자가 같이 출력 원함
            // 이미지 크기에 따라 위치 조정 필요. 일단 이미지 아래에.
            int textY = imgY + titleTexture.height + 20;
            ui_draw_str_at((screenW - 200)/16, textY/16, "METRO HERO", "\033[93m"); // Bright Yellow? Grid coord approx
        } else {
            // 이미지 없으면 텍스트만 크게?
            const char* title = "METRO HERO";
            // ui_draw_text_clipped or custom large text
            // Just basic centered text for now
            int titleLen = 10;
            int titleX = (SCREEN_W - titleLen) / 2;
            ui_draw_str_at(titleX, 10, title, "\033[93m");
        }
        
        // 2. Menu Options
        int menuY = 35; // Grid Unit (approx middle-bottom)
        int centerX = SCREEN_W / 2;
        
        ui_draw_str_at(centerX - 10, menuY,     "[1] 새로 시작", "\033[97m");
        ui_draw_str_at(centerX - 10, menuY + 3, "[2] 이어 하기", "\033[90m"); // Gray (Not impl yet)
        ui_draw_str_at(centerX - 10, menuY + 6, "[Q] 종료", "\033[97m");

        EndDrawing();
    }
    
    return GAME_EXIT;
}
