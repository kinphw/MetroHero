#include "system_menu.h"
#include "../layout.h"
#include "../text/render.h"
#include "../widgets/box.h"
#include "raylib.h"
#include <stdio.h>

void ui_draw_system_menu(const GameState* state) {
    // Center within Viewport
    int w = 24;
    int h = 10;
    int x = VIEWPORT_X + (VIEWPORT_W - w) / 2;
    int y = VIEWPORT_Y + (VIEWPORT_H - h) / 2;

    // 1. Draw Opaque Background (Cover Map)
    // Convert grid to pixels
    int px = x * GRID_W;
    int py = y * GRID_H;
    int pw = w * GRID_W;
    int ph = h * GRID_H;

    // Semi-transparent black for focus
    DrawRectangle(0, 0, GetScreenWidth(), GetScreenHeight(), Fade(BLACK, 0.5f));
    
    // Solid background for menu
    DrawRectangle(px, py, pw, ph, BLACK);

    // 2. Draw Box with Title
    ui_draw_box(x, y, w, h, "MENU");

    // 3. List Options
    const char* options[] = {
        "인벤토리",
        "저장하기",
        "불러오기",
        "게임종료"
    };
    int optionCount = 4;

    int startY = y + 2;
    for (int i = 0; i < optionCount; i++) {
        char buf[64];
        const char* color = (i == state->systemMenuCursor) ? "\033[93m" : "\033[90m"; // Yellow / Gray
        
        if (i == state->systemMenuCursor) {
            snprintf(buf, sizeof(buf), "> %s", options[i]);
        } else {
            snprintf(buf, sizeof(buf), "  %s", options[i]);
        }
        
        // Center text in box
        int textX = x + 2; 
        ui_draw_str_at(textX, startY + (i * 2), buf, color);
    }
    
    // Guide
    ui_draw_str_at(x + 2, y + h - 2, "Select: SPACE", "\033[90m");
}
