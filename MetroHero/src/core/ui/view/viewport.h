#ifndef UI_VIEWPORT_H
#define UI_VIEWPORT_H

#include "../../../world/map.h"
#include "../../../entity/player.h"

void ui_render_map_viewport(Map* m, Player* p, 
                            int effectX, int effectY, float effectTimer, const char* effectPath,
                            int effect2X, int effect2Y, float effect2Timer, const char* effect2Path);

#endif
