#include "game_internal.h"
// #include <windows.h> // Removed for Raylib

#include "raylib.h"

void game_loop(GameState* state) {
    state->isRunning = 1;

    // 초기 상태 렌더링
    game_render(state);

    while (!WindowShouldClose() && state->isRunning) {
        // 입력 처리 (Non-blocking)
        game_process_input(state);

        // 업데이트 (현재 로직상 input에서 대부분 처리되지만 나중에 분리 가능)
        // game_update(state); 

        // 렌더링 (매 프레임 호출)
        // game_render calls ui_clear_buffer (BeginDrawing) ... ui_present (EndDrawing)
        game_render(state);
    }
}
