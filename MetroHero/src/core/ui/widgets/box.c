#include "box.h"
#include "../text/render.h"

void ui_draw_box(int x, int y, int w, int h, const char* title) {
    // Force default color for border
    const char* borderCol = "\033[0m"; 
    
    // Corners
    ui_draw_str_at(x, y, "┌", borderCol);
    ui_draw_str_at(x + w - 1, y, "┐", borderCol);
    ui_draw_str_at(x, y + h - 1, "└", borderCol);
    ui_draw_str_at(x + w - 1, y + h - 1, "┘", borderCol);

    // Top/Bottom edges
    for (int i = 1; i < w - 1; i++) {
        ui_draw_str_at(x + i, y, "─", borderCol);
        ui_draw_str_at(x + i, y + h - 1, "─", borderCol);
    }

    // Side edges
    for (int i = 1; i < h - 1; i++) {
        ui_draw_str_at(x, y + i, "│", borderCol);
        ui_draw_str_at(x + w - 1, y + i, "│", borderCol);
    }

    // Title
    if (title) {
        ui_draw_str_at(x + 1, y, "─ ", borderCol);

        // Draw title (use default color or bright white?)
        int titleActualWidth = ui_draw_text_clipped(x + 3, y, w - 5, title, "\033[97m"); // Bright White

        if (3 + titleActualWidth < w - 1) {
            ui_draw_str_at(x + 3 + titleActualWidth, y, " ─", borderCol);
        }

        // ★ 오른쪽 위 모서리 보호
        ui_draw_str_at(x + w - 1, y, "┐", borderCol);
    }

    // ★ 오른쪽 아래 모서리 보호
    ui_draw_str_at(x + w - 1, y + h - 1, "┘", borderCol);
}
