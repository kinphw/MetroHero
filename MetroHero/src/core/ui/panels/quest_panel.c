#include "quest_panel.h"
#include "../layout.h"
#include "../widgets/box.h"
#include "../text/render.h"
#include "../../../world/glyph.h"
#include <string.h>
#include <stdio.h>

void ui_draw_quest_tracker(const GameState* state) {
    int x = QUEST_X;
    int y = QUEST_Y;
    int w = QUEST_W;
    int h = QUEST_H;
    
    ui_draw_box(x, y, w, h, "퀘스트");
    
    // State Logic
    const char* displayMsg = state->activeQuestMsg;
    const char* color = COLOR_BRIGHT_YELLOW;
    
    if (state->questState == 2) { 
        // Hidden Phase
        return; 
    }
    else if (state->questState == 1) {
        // Sparkle Phase (Completed)
        displayMsg = state->activeQuestMsg; // Show Old Quest
        
        // Blink / Sparkle Effect
        // Simple 0.1s toggle
        int tick = (int)(state->questTimer * 10);
        if (tick % 2 == 0) color = COLOR_BRIGHT_YELLOW;
        else color = COLOR_BRIGHT_WHITE;
    }

    if (displayMsg && strlen(displayMsg) > 0) {
        char buf[512];
        snprintf(buf, sizeof(buf), "%s%s", color, displayMsg);
        ui_draw_text_clipped(x + 2, y + 2, w - 4, buf, NULL);
    } else {
        ui_draw_text_clipped(x + 2, y + 2, w - 4, "현재 진행 중인 퀘스트가 없습니다.", COLOR_GRAY);
    }
}
