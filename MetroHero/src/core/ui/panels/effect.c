#include "effect.h"
#include "../text/render.h"
#include "../../../world/glyph.h" // For Colors

static int combatEffectFrames = 0;

void ui_show_combat_effect(void) {
    combatEffectFrames = 10; // 10 frames duration
    // No direct draw here, logic handles update in gameloop which calls render
}

void ui_hide_combat_effect(void) {
    combatEffectFrames = 0;
}

void ui_draw_combat_effect_overlay_if_active(int statusX, int statusY) {
    if (combatEffectFrames > 0) {
        int ex = statusX + 25;
        int ey = statusY + 2;

        ui_draw_str_at(ex, ey - 1, "전투중", COLOR_BRIGHT_RED);
        ui_draw_str_at(ex, ey,     "  ⚔",   COLOR_BRIGHT_RED);
        ui_draw_str_at(ex, ey + 1, " ⚔⚔⚔", COLOR_BRIGHT_RED);
        ui_draw_str_at(ex, ey + 2, "⚔⚔⚔⚔⚔", COLOR_BRIGHT_RED);
        ui_draw_str_at(ex, ey + 3, " ⚔⚔⚔", COLOR_BRIGHT_RED);
        ui_draw_str_at(ex, ey + 4, "  ⚔",   COLOR_BRIGHT_RED);

        combatEffectFrames--;
    }
}
