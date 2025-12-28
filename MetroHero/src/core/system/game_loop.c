#include "game_internal.h"
// #include <windows.h> // Removed for Raylib

#include "raylib.h"
#include "../ui/ui.h"
#include "../../world/glyph.h"

void game_loop(GameState* state) {
    state->isRunning = 1;

    // 초기 상태 렌더링
    ui_add_combat_log(COLOR_BRIGHT_RED "전투 시스템 준비 완료..." COLOR_RESET);
    game_render(state);

    while (!WindowShouldClose() && state->isRunning) {
        float dt = GetFrameTime();

        // 입력 처리 (Non-blocking)
        game_process_input(state);

        // ★ 타이머 업데이트
        if (state->effectTimer > 0) state->effectTimer -= dt;
        if (state->player.attackCooldown > 0) state->player.attackCooldown -= dt;

        // 렌더링 (매 프레임 호출)
        game_render(state);
    }
}
