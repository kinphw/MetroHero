#include "game_internal.h"
#include "../ui/ui.h"
#include "../../world/glyph.h"

// 화면 렌더링 (초기화 및 업데이트)
void game_render(GameState* state) {
    // 이동했으면 뷰포트 다시 그리기
    if (state->prevX != state->player.x || state->prevY != state->player.y) {
        map_draw_viewport(&state->map, &state->player, VIEWPORT_X, VIEWPORT_Y, 40, VIEWPORT_H);
        state->prevX = state->player.x;
        state->prevY = state->player.y;
    }

    // 대화 모드가 아닐 때만 UI 갱신
    if (!state->inDialogue) {
        // 상태창 (HP 등)
        ui_draw_stats(&state->player, STATUS_X, STATUS_Y, STATUS_W, STATUS_H);
        
        // 장비창
        ui_draw_equipment(&state->player, EQUIP_X, EQUIP_Y, EQUIP_W, EQUIP_H);
        
        // 로그창 (항상 표시하지만 대화중엔 대화창이 덮을 수 있음)
        ui_draw_log(LOG_X, LOG_Y, LOG_W, LOG_H);
    }
    // 대화 모드일 때는 input에서 직접 ui_draw_dialogue를 호출하거나 여기서 상태를 보고 호출할 수도 있음
    // 현재 구조상 input에서 즉시 반응하여 그리는 경우가 많으나,
    // 렌더링 루프를 분리한다면 여기서 그리는 것이 맞음.
    // 하지만 기존 로직(Input-driven rendering)을 유지하기 위해 최소한의 갱신만 수행.
}
