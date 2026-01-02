#define _CRT_SECURE_NO_WARNINGS
#include "viewport.h"
#include "../backend/buffer.h" // For globalColor reset
#include <string.h>

#include "../layout.h"

void ui_render_map_viewport(Map* m, Player* p, 
                            int effectX, int effectY, float effectTimer, const char* effectPath,
                            int effect2X, int effect2Y, float effect2Timer, const char* effect2Path) {
    // Reset color before map render to avoid bleed
    strcpy(globalColor, "\033[0m");
    map_draw_viewport(m, p, VIEWPORT_X, VIEWPORT_Y, VIEWPORT_MAP_W, VIEWPORT_MAP_H, 
                      effectX, effectY, effectTimer, effectPath,
                      effect2X, effect2Y, effect2Timer, effect2Path);
}
