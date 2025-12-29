#define _CRT_SECURE_NO_WARNINGS
#include "stats.h"
#include "../backend/buffer.h" // For globals/colors if needed, though render handles it
#include "../text/render.h"
#include "../text/glyph.h"
#include "../widgets/box.h"
#include "../../../world/glyph.h" // For constants if needed, though colors are standard
#include <stdio.h>
#include <string.h>

#include "../layout.h"

// Layout constants - Localized (removed)

extern int combatEffectFrames; // Accessed from effect.c ... wait, cyclic dependency or shared state?
// combatEffectFrames was static in ui.c. Now it belongs to effect module.
// ui_draw_stats draws the combat effect overlay? 
// Original code had `ui_draw_stats` drawing the combat overlay.
// Refactor: Move combat overlay drawing to `effect.c` and call it from `draw_stats` or separate?
// Better: `ui_draw_stats` calls `ui_draw_combat_effect_overlay` from `effect.h`.

#include "effect.h" // New dependency

void ui_draw_stats(const Player* p) {
    int x = STATUS_X;
    int y = STATUS_Y;
    int w = STATUS_W;
    int h = STATUS_H;
    
    ui_draw_box(x, y, w, h, "상태");

    char buf[128];

    // ★ HP Bar - 개별 문자로 그리기 (정확한 폭 제어)
    ui_draw_str_at(x + 2, y + 2, "HP: ", NULL);
    // ★ HP Bar - Raylib 도형 그리기
    ui_draw_str_at(x + 2, y + 2, "HP: ", NULL);
    
    // Calculate Pixels
    // "HP: " is 4 chars width = 4 * 16px (full width) / 2 = 32px? 
    // display_width("HP: ") returns grid units (4). 4 * 8px = 32px.
    int hpLabelWidthPx = 4 * 8; 
    
    int barX = (x + 2) * 8 + hpLabelWidthPx; // Start after "HP: "
    int barY = (y + 2) * 16;
    
    // Bar dimensions
    // Width: Fill remaining space inside box with some padding?
    // Previous loop was 10 chars -> 10 * 8 = 80px.
    int barW = 80; 
    int barH = 14; // Slightly smaller than 16 to fit nicely vertically
    int barYOffset = 1; // Center vertically in the cell (16 - 14)/2 = 1
    
    // Draw Background (Dark Red/Gray)
    DrawRectangle(barX, barY + barYOffset, barW, barH, DARKGRAY);
    
    // Draw Foreground (Red/Green based on HP?)
    if (p->maxHp > 0) {
        float hpPercent = (float)p->hp / (float)p->maxHp;
        if (hpPercent < 0) hpPercent = 0;
        if (hpPercent > 1) hpPercent = 1;
        
        int fillW = (int)(barW * hpPercent);
        
        // Color choice: Red for HP usually
        DrawRectangle(barX, barY + barYOffset, fillW, barH, RED);
    }
    
    // Draw Border
    DrawRectangleLines(barX, barY + barYOffset, barW, barH, WHITE);

    // HP Text
    snprintf(buf, sizeof(buf), "     %3d / %3d", p->hp, p->maxHp);
    ui_draw_text_clipped(x + 2, y + 3, w - 4, buf, NULL);

    // Attack
    snprintf(buf, sizeof(buf), " 공격력: %2d~%2d", p->attackMin, p->attackMax);
    ui_draw_text_clipped(x + 2, y + 5, w - 4, buf, NULL);



    ui_draw_combat_effect_overlay_if_active(x, y);

    // ★ 테두리 모서리 보호
    // const char* borderCol = "\033[0m";
    // ui_draw_str_at(x, y, "┌", borderCol);
    // ui_draw_str_at(x + w - 1, y, "┐", borderCol);
    // ui_draw_str_at(x, y + h - 1, "└", borderCol);
    // ui_draw_str_at(x + w - 1, y + h - 1, "┘", borderCol);
}
