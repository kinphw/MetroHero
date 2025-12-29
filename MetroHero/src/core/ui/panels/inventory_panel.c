#include "inventory_panel.h"
#include "../layout.h"
#include "../text/render.h"
#include "../widgets/box.h"
#include "raylib.h" // Added for DrawRectangle
#include <stdio.h>

// Draw Inventory covering the Map Viewport
void ui_draw_inventory_viewport(const Player* p, int cursor) {
    int x = VIEWPORT_X;
    int y = VIEWPORT_Y;
    int w = VIEWPORT_W;
    int h = VIEWPORT_H;

    // 1. Draw Opaque Background (Cover Map)
    // Convert grid to pixels
    int px = x * GRID_W;
    int py = y * GRID_H;
    int pw = w * GRID_W;
    int ph = h * GRID_H;

    DrawRectangle(px, py, pw, ph, BLACK);

    // 2. Draw Box with Title
    ui_draw_box(x, y, w, h, "INVENTORY");

    // 2. List Items
    int listX = x + 2;
    int listY = y + 2;
    int itemsPerPage = h - 10; // Reserve space for description
    
    // Ensure cursor is within view (simple scrolling or just clamp if items > page)
    // For now, assume simple list < perPage or implement simple scroll logic later.
    // Given MAX_INVENTORY_SLOTS is 20, and height is 30, it fits on one page easy.
    
    for (int i = 0; i < p->inventory.count; i++) {
        const Item* item = p->inventory.items[i];
        if (!item) continue;

        char buf[128];
        char marker = (i == cursor) ? '>' : ' ';
        
        // Equip Status
        const char* equipStatus = "";
        if (p->equippedWeapon == item || p->equippedArmor == item) {
            equipStatus = "[E] ";
        }

        snprintf(buf, sizeof(buf), "%c %s%s", marker, equipStatus, item->name);
        
        // Highlight cursor line
        const char* color = (i == cursor) ? "\033[1;33m" : NULL; // Yellow for cursor
        if (p->equippedWeapon == item || p->equippedArmor == item) {
             if (i != cursor) color = "\033[1;32m"; // Green for equipped
        }

        ui_draw_str_at(listX, listY + i, buf, color);
    }

    if (p->inventory.count == 0) {
        ui_draw_str_at(listX, listY, "비어있음", "\033[1;30m");
    }

    // 3. Draw Description (Bottom Area)
    int descY = y + h - 8;
    
    // Separator line
    for (int i = x + 1; i < x + w - 1; i++) {
        ui_draw_str_at(i, descY - 1, "-", NULL);
    }

    if (cursor >= 0 && cursor < p->inventory.count) {
        const Item* sel = p->inventory.items[cursor];
        if (sel) {
            // Name
            ui_draw_str_at(listX, descY, sel->name, "\033[1;37m");
            
            // Stats
            char statBuf[64];
            if (sel->type == ITEM_WEAPON) {
                snprintf(statBuf, sizeof(statBuf), "공격력 +%d", sel->attackBonus);
            } else if (sel->type == ITEM_ARMOR) {
                snprintf(statBuf, sizeof(statBuf), "최대체력 +%d", sel->hpBonus);
            } else if (sel->type == ITEM_CONSUMABLE) {
                if (sel->attackBonus > 0) snprintf(statBuf, sizeof(statBuf), "공격력 영구 +%d", sel->attackBonus);
                else if (sel->hpBonus > 0) snprintf(statBuf, sizeof(statBuf), "최대체력 영구 +%d", sel->hpBonus);
                else snprintf(statBuf, sizeof(statBuf), "소모품");
            } else {
                snprintf(statBuf, sizeof(statBuf), "기타");
            }
            ui_draw_str_at(listX + 20, descY, statBuf, "\033[1;36m"); // Cyan

            // Description
            ui_draw_str_at(listX, descY + 2, sel->desc, NULL);
            
            // Usage Hint
            ui_draw_str_at(listX, descY + 4, "[ENTER] 장착/사용", "\033[0;33m");
        }
    }
}
