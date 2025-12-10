#define _CRT_SECURE_NO_WARNINGS
#include "viewport.h"
#include "../backend/buffer.h" // For globalColor reset
#include <string.h>

#include "../layout.h"

void ui_render_map_viewport(Map* m, Player* p) {
    // Reset color before map render to avoid bleed
    strcpy(globalColor, "\033[0m");
    map_draw_viewport(m, p, VIEWPORT_X, VIEWPORT_Y, 60, VIEWPORT_H);
}
