#include <stddef.h> // NULL 정의 추가
#include "game_internal.h"
#include "../ui/ui.h"
#include "../ui/text/render.h" // For ui_draw_image
#include "../ui/panels/inventory_panel.h" // Added
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
    ui_render_map_viewport(&state->map, &state->player, state->effectX, state->effectY, state->effectTimer);

    // ★ 인벤토리 오버레이
    if (state->inInventory) {
        // Draw Inventory ON TOP of map (Map is background)
        ui_draw_inventory_viewport(&state->player, state->inventoryCursor);
    }

    // ★ In-Game Header Image (Logo + Stage)
    // Screen Width: 1680px
    // Single Image Size: 300x96 (assumed same size for both)
    // Total Width: 300 + 300 = 600
    int singleW = 300;
    int headerH = 96;
    int totalW = singleW * 2;
    
    int headerX = (SCREEN_W * 8 - totalW) / 2;
    int headerY = 0; 
    
    // Draw Title (Left)
    ui_draw_image(headerX, headerY, singleW, headerH, "assets/title/title_on.png");
    
    // Draw Stage (Right - Attached)
    ui_draw_image(headerX + singleW, headerY, singleW, headerH, "assets/title/title_s1.png");

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
    ui_draw_combat_log();

    // ★ 적 이미지 표시 (타이머가 남아있을 때만)
    if (state->enemyImageTimer > 0) {
        ui_draw_enemy_image(state->enemyImagePath);
    }

    // 배칭 종료
    ui_end_texture_mode();
    
    // 4. 화면 출력 (Present)
    ui_present();
}
