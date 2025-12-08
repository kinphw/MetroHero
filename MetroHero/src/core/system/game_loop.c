#include "game_internal.h"
#include <windows.h> // Sleep? or just raw loop

void game_loop(GameState* state) {
    state->isRunning = 1;

    // 초기 상태 렌더링
    game_render(state);

    while (state->isRunning) {
        // 입력 처리
        game_process_input(state);

        // 업데이트 (현재 로직상 input에서 대부분 처리되지만 나중에 분리 가능)
        // game_update(state); 

        // 렌더링
        game_render(state);
    }
}
