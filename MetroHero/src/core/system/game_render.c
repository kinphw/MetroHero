#include <stddef.h> // NULL 정의 추가
#include "game_internal.h"
#include "../ui/ui.h"
#include "../../world/glyph.h"
#include "raylib.h"

// 화면 렌더링 (초기화 및 업데이트)
// 화면 렌더링 (초기화 및 업데이트)
void game_render(GameState* state) {
    // 1. 버퍼 클리어 & 배칭 시작
    ui_begin_texture_mode();
    ClearBackground(BLACK); // ui_clear_buffer calls this internally + begin/end. But we need to keep the session open.
    // We removed ui_clear_buffer internal usage or we just use explicit clear here for optimization.
    // Since ui_clear_buffer does Begin/End, we should NOT use it if we want to batch.
    
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
    
    // 배칭 종료
    ui_end_texture_mode();
    
    // 4. 화면 출력 (Present)
    ui_present();
}
