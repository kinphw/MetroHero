#define _CRT_SECURE_NO_WARNINGS
#include "equipment.h"
#include "../text/render.h"
#include "../widgets/box.h"
#include <stdio.h>

#include "../layout.h"

void ui_draw_equipment(const Player* p) {
    int x = EQUIP_X;
    int y = EQUIP_Y;
    int w = EQUIP_W;
    int h = EQUIP_H;

    ui_draw_box(x, y, w, h, "장비");

    char buf[128];
    snprintf(buf, sizeof(buf), " 무기:    %s", p->weaponName);
    ui_draw_text_clipped(x + 2, y + 2, w - 4, buf, NULL);

    snprintf(buf, sizeof(buf), " 방어구:  %s", p->armorName);
    ui_draw_text_clipped(x + 2, y + 3, w - 4, buf, NULL);

    snprintf(buf, sizeof(buf), " 아이템:  %s", p->item1);
    ui_draw_text_clipped(x + 2, y + 4, w - 4, buf, NULL);

    // ★ 테두리 모서리 보호
    const char* borderCol = "\033[0m";
    ui_draw_str_at(x, y, "┌", borderCol);
    ui_draw_str_at(x + w - 1, y, "┐", borderCol);
    ui_draw_str_at(x, y + h - 1, "└", borderCol);
    ui_draw_str_at(x + w - 1, y + h - 1, "┘", borderCol);
}
