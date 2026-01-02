#include "launcher.h"
#include "raylib.h"
#include "../audio/audio.h" // Added
#include "../ui/ui.h" // For ui_begin_texture_mode etc if needed, or just raylib
#include "../ui/layout.h"
#include "../ui/text/render.h"
#include "../ui/text/glyph.h" // For display_width
#include <stdio.h> // snprintf

// 타이틀 이미지 텍스처 (정적 캐싱)
static Texture2D titleTexture = { 0 };

static void load_title_image(void) {
    if (titleTexture.id == 0) {
        titleTexture = LoadTexture("assets/title/title.png");
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
    audio_play_music("assets/bgm/tunnel_ambience.mp3"); // Start Title BGM
    
    int selectedOption = 0;
    const int optionCount = 3;
    const char* options[] = { "새로 시작", "이어 하기", "종료" };
    
    // 메뉴 루프
    while (!WindowShouldClose()) {
        audio_update(); // Update Stream

        // --- Input Handling ---
        if (IsKeyPressed(KEY_DOWN)) {
            selectedOption = (selectedOption + 1) % optionCount;
        }
        if (IsKeyPressed(KEY_UP)) {
            selectedOption = (selectedOption - 1 + optionCount) % optionCount;
        }
        
        //if (IsKeyPressed(KEY_ENTER)) {
        if (IsKeyPressed(KEY_SPACE)) {
            if (selectedOption == 0) return GAME_NEW;
            if (selectedOption == 1) return GAME_LOAD; // 아직 구현 안됨
            if (selectedOption == 2) return GAME_EXIT;
        }

        // --- Drawing ---
        BeginDrawing();
        ClearBackground(BLACK);

        int screenW = SCREEN_W * 8; // 1680

        // 1. Title Image
        if (titleTexture.id != 0) {
            float targetWidth = 1200.0f;
            float scale = targetWidth / (float)titleTexture.width;
            float targetHeight = (float)titleTexture.height * scale;

            float imgX = ((float)screenW - targetWidth) / 2.0f;
            float imgY = 100.0f; 

            Rectangle srcRec = { 0.0f, 0.0f, (float)titleTexture.width, (float)titleTexture.height };
            Rectangle destRec = { imgX, imgY, targetWidth, targetHeight };
            Vector2 origin = { 0.0f, 0.0f };

            DrawTexturePro(titleTexture, srcRec, destRec, origin, 0.0f, WHITE);
        } else {
            int titleX = (SCREEN_W - 10) / 2;
            ui_draw_str_at(titleX, 10, "METRO HERO", "\033[93m");
        }
        
        // 2. Menu Options
        int menuStartY = 35; // Grid Unit
        int centerX = SCREEN_W / 2;
        
        for (int i = 0; i < optionCount; i++) {
            int y = menuStartY + (i * 3);
            char buf[64];
            const char* color = "\033[90m"; // Default Gray
            
            if (i == selectedOption) {
                color = "\033[97m"; // Bright White (Selected)
                snprintf(buf, sizeof(buf), "> %s", options[i]);
            } else {
                snprintf(buf, sizeof(buf), "  %s", options[i]);
            }
            
            // "이어하기" 비활성화 표시? (선택은 되지만 색상을 다르게 하거나)
            // 현재는 그냥 선택 가능하도록 둠 (사용자 요청: 직관적 이동)
            
            // 중앙 정렬을 위해 대략적 길이 계산 (한글 2칸, 공백 1칸..)
            // display_width 사용하면 좋음.
            int len = display_width(buf); 
            ui_draw_str_at(centerX - (len / 2), y, buf, color);
        }

        // 안내 문구 추가
        int guideY = menuStartY + (optionCount * 3) + 4;
        const char* guide = "- SPACE 키를 눌러 선택하세요 -";
        int guideLen = display_width(guide);
        ui_draw_str_at(centerX - (guideLen / 2), guideY, guide, "\033[90m"); // Dark Gray

        EndDrawing();
    }
    
    return GAME_EXIT;
}
