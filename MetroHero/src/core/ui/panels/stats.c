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

    // 1. Level & Name
    snprintf(buf, sizeof(buf), "Lv.%d", p->level);
    ui_draw_str_at(x + 2, y + 2, buf, COLOR_BRIGHT_YELLOW);

    // 2. HP Bar
    int barX = (x + 2) * 8; 
    int barY = (y + 3) * 16 + 4; // y+3 line
    int barW = 100;
    int barH = 10;
    
    // Background
    DrawRectangle(barX, barY, barW, barH, DARKGRAY);
    // Foreground (Red)
    if (p->maxHp > 0) {
        float pct = (float)p->hp / (float)p->maxHp;
        if (pct < 0) pct = 0; if (pct > 1) pct = 1;
        DrawRectangle(barX, barY, (int)(barW * pct), barH, RED);
    }
    DrawRectangleLines(barX, barY, barW, barH, WHITE);
    
    // HP Text
    snprintf(buf, sizeof(buf), "HP %d/%d", p->hp, p->maxHp);
    ui_draw_text_clipped(x + 2, y + 4, w - 4, buf, NULL);

    // 3. XP Bar
    int expBarY = (y + 5) * 16 + 4;
    
    // Background
    DrawRectangle(barX, expBarY, barW, barH, DARKGRAY);
    // Foreground (Blue)
    if (p->expNext > 0) {
        float pct = (float)p->exp / (float)p->expNext;
        if (pct < 0) pct = 0; if (pct > 1) pct = 1;
        DrawRectangle(barX, expBarY, (int)(barW * pct), barH, BLUE);
    } else {
        // Max Level?
        DrawRectangle(barX, expBarY, barW, barH, GOLD);
    }
    DrawRectangleLines(barX, expBarY, barW, barH, WHITE);

    // XP Text
    if (p->expNext > 0)
        snprintf(buf, sizeof(buf), "XP %d/%d", p->exp, p->expNext);
    else
        snprintf(buf, sizeof(buf), "XP MAX");
        
    ui_draw_text_clipped(x + 2, y + 6, w - 4, buf, NULL);

    // 4. Attack
    snprintf(buf, sizeof(buf), "공격력 %d~%d", p->attackMin, p->attackMax);
    ui_draw_text_clipped(x + 2, y + 8, w - 4, buf, NULL);



    ui_draw_combat_effect_overlay_if_active(x, y);

    // ★ 테두리 모서리 보호
    // const char* borderCol = "\033[0m";
    // ui_draw_str_at(x, y, "┌", borderCol);
    // ui_draw_str_at(x + w - 1, y, "┐", borderCol);
    // ui_draw_str_at(x, y + h - 1, "└", borderCol);
    // ui_draw_str_at(x + w - 1, y + h - 1, "┘", borderCol);
}
