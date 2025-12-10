#define _CRT_SECURE_NO_WARNINGS
#include "stats.h"
#include "../backend/buffer.h" // For globals/colors if needed, though render handles it
#include "../text/render.h"
#include "../text/glyph.h"
#include "../widgets/box.h"
#include "../../../world/glyph.h" // For constants if needed, though colors are standard
#include <stdio.h>
#include <string.h>

// Layout constants - Localized
#define STATUS_X 120
#define STATUS_Y 0
#define STATUS_W 48
#define STATUS_H 14

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
    int hpBars = (p->hp * 10) / p->maxHp;
    if (hpBars > 10) hpBars = 10;
    int barX = x + 2 + 4;  // "HP: " = 4칸

    // 수정 - 문자열로 한 번에 그리기
    char hpBarStr[64] = "";
    for (int i = 0; i < 10; i++) {
        strcat(hpBarStr, i < hpBars ? "█" : "░");
    }
    ui_draw_str_at(barX, y + 2, hpBarStr, NULL);
    int barEndX = barX + display_width(hpBarStr);  // 동적 계산

    for (int i = barEndX; i < x + w - 2; i++) {
        ui_draw_str_at(i, y + 2, " ", NULL);
    }

    // HP Text
    snprintf(buf, sizeof(buf), "     %3d / %3d", p->hp, p->maxHp);
    ui_draw_text_clipped(x + 2, y + 3, w - 4, buf, NULL);

    // Attack
    snprintf(buf, sizeof(buf), " 공격력: %2d~%2d", p->attackMin, p->attackMax);
    ui_draw_text_clipped(x + 2, y + 5, w - 4, buf, NULL);

    // Defense
    snprintf(buf, sizeof(buf), " 방어력:  %3d", p->defense);
    ui_draw_text_clipped(x + 2, y + 6, w - 4, buf, NULL);

    // ★ Direction - 개별 출력 + 공백 채우기
    const char* arrow = " ";
    if (p->dirY < 0) arrow = "↑";
    else if (p->dirY > 0) arrow = "↓";
    else if (p->dirX < 0) arrow = "←";
    else if (p->dirX > 0) arrow = "→";

    // 수정 - 한 문자열로 그리고 동적 계산
    char dirLine[64];
    snprintf(dirLine, sizeof(dirLine), " 방향:    %s", arrow);
    ui_draw_str_at(x + 2, y + 7, dirLine, NULL);
    // 방향 뒤 공백 (동적 계산)
    int dirEndX = x + 2 + display_width(dirLine);
    for (int i = dirEndX; i < x + w - 1; i++) {
        ui_draw_str_at(i, y + 7, " ", NULL);
    }

    ui_draw_combat_effect_overlay_if_active(x, y);

    // ★ 테두리 모서리 보호
    const char* borderCol = "\033[0m";
    ui_draw_str_at(x, y, "┌", borderCol);
    ui_draw_str_at(x + w - 1, y, "┐", borderCol);
    ui_draw_str_at(x, y + h - 1, "└", borderCol);
    ui_draw_str_at(x + w - 1, y + h - 1, "┘", borderCol);
}
