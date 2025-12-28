#ifndef UI_DIALOGUE_H
#define UI_DIALOGUE_H

#include "../../../entity/npc.h"

void ui_draw_dialogue(const NPC* npc);
void ui_clear_dialogue_area(void);
void ui_add_log(const char* msg);
void ui_draw_log(void);
void ui_add_combat_log(const char* msg);
void ui_draw_combat_log(void);
void ui_draw_enemy_image(const char* imagePath); // ★ 적 이미지 표시

#endif
