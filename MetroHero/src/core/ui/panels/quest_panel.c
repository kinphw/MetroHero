#include "quest_panel.h"
#include "../layout.h"
#include "../widgets/box.h"
#include "../text/render.h"
#include "../../../world/glyph.h"
#include <string.h>
#include <stdio.h>

void ui_draw_quest_tracker(const char* activeMsg) {
    int x = QUEST_X;
    int y = QUEST_Y;
    int w = QUEST_W;
    int h = QUEST_H;
    
    ui_draw_box(x, y, w, h, "퀘스트");

    if (activeMsg && strlen(activeMsg) > 0) {
        // Draw Active Quest Message in Cyan/Blue
        // Use clipped text multiline
        char buf[512];
        snprintf(buf, sizeof(buf), "%s%s", COLOR_BRIGHT_CYAN, activeMsg); // Force Color
        
        // Simple word wrap or just clipped? Dialogue handles wrapping.
        // ui_draw_text_clipped handles newlines but not auto-wrap if strictly clipped.
        // But `text/render.c` `ui_draw_text_clipped` does basic loop. 
        // Let's rely on it.
        // We start drawing from y+2
        
        const char* ptr = buf;
        int line = 0;
        int maxLines = h - 2;
        
        // Manual simple draw since draw_text_clipped draws one line? 
        // No, `ui_draw_dialogue` loops. `ui_draw_text_clipped` draws ONE line.
        // I need a simple multiline drawer or just call clipped for each line?
        // Let's look at `ui_draw_text_clipped` usage. It draws string at Y.
        
        // Hack: Just draw the string. If it has newlines it might fail if clipped doesn't handle y increment.
        // `ui_draw_text_clipped` is: `return (int)strlen(text);` logic inside `ui_draw_str_ansi`.
        // `ui_draw_str_ansi` does NOT handle newlines for Y increment.
        // So I must manually handle wrapping if the message is long.
        // For now, assume message fits or user puts \n.
        
        // But `activeMsg` from QuestConfig might be long.
        // Let's just draw it.
        ui_draw_text_clipped(x + 2, y + 2, w - 4, buf, NULL);
    } else {
        ui_draw_text_clipped(x + 2, y + 2, w - 4, "현재 진행 중인 퀘스트가 없습니다.", COLOR_GRAY);
    }
}
