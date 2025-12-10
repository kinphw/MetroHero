#include <stddef.h> // NULL 정의 추가
#include "game_internal.h"
#include "../ui/ui.h"
#include "../../world/glyph.h"

// 화면 렌더링 (초기화 및 업데이트)
void game_render(GameState* state) {
    // 1. 버퍼 클리어
    ui_clear_buffer();

    // 2. 뷰포트 그리기 (항상 그림)
    ui_render_map_viewport(&state->map, &state->player);

    // 3. UI 그리기 (항상 그림 - 대화창이 오버레이됨)
    if (!state->inDialogue) {
        ui_draw_stats(&state->player);
        ui_draw_equipment(&state->player);
    } else {
        if (state->currentNPC != NULL) {
            ui_draw_dialogue(state->currentNPC);
        }
    }
    ui_draw_log();
    // 대화 중일 때는 ui_draw_dialogue가 필요하지만, 
    // 기존 로직에서는 input 루프 안에서 그렸을 수 있음.
    // 하지만 더블 버퍼링에서는 매 프레임 다 그리는 게 정석.
    // game_input.c에서 입력을 대기하면서 그리는지 확인 필요.
    // 일단 여기서는 기본 UI만 그린다.
    
    // 4. 화면 출력 (Present)
    ui_present();
}
