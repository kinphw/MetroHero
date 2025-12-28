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
            // 원본 크기: 3392x1248 -> 너무 큼
            // 목표 크기: 너비 800px 유지 (비율 고정)
            float targetWidth = 1200.0f;
            float scale = targetWidth / (float)titleTexture.width;
            float targetHeight = (float)titleTexture.height * scale; // 1248 * 0.23... ~= 293px

            // 중앙 정렬 좌표 계산
            float imgX = ((float)screenW - targetWidth) / 2.0f;
            float imgY = 100.0f; // 상단 여백

            // 소스 영역 (전체 이미지)
            Rectangle srcRec = { 0.0f, 0.0f, (float)titleTexture.width, (float)titleTexture.height };
            // 대상 영역 (리사이징된 크기)
            Rectangle destRec = { imgX, imgY, targetWidth, targetHeight };
            // 회전 중심 (좌상단 0,0)
            Vector2 origin = { 0.0f, 0.0f };

            DrawTexturePro(titleTexture, srcRec, destRec, origin, 0.0f, WHITE);
            
            // 텍스트 타이틀 (이미지 아래)
            // 이미지 높이(targetHeight)에 맞춰 텍스트 위치 조정
            // int textY = (int)(imgY + targetHeight + 20);
            // ui_draw_str_at((screenW - 200)/16, textY/16, "METRO HERO", "\033[93m");
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
