#ifndef UI_H
#define UI_H

#include "../../entity/player.h"
#include "../../entity/npc.h"
#include "../../world/map.h"

#define SCREEN_W 120
#define SCREEN_H 30

void console_goto(int x , int y);
void console_clear_fast(void);

// 뷰포트 렌더링 래퍼
void ui_render_map_viewport(Map* m, Player* p);

// 상태창 및 장비창
void ui_draw_equipment(const Player* p);
void ui_draw_stats(const Player* p);

// 로그창
void ui_add_log(const char* msg);
void ui_draw_log(void);

// 전투 이펙트
void ui_show_combat_effect(void);
void ui_hide_combat_effect(void);

// 대화창
void ui_draw_dialogue(const NPC* npc);
void ui_clear_dialogue_area(void);

int display_width(const char* str);

// ============================================
// 더블 버퍼링 API
// ============================================
void ui_init_buffer(void);
void ui_clear_buffer(void);
void ui_present(void);
void ui_draw_char_at(int x, int y, const char* c, const char* color);
void ui_draw_str_at(int x, int y, const char* str, const char* color);
void ui_get_glyph_info(const char* s, int* byteLen, int* displayWidth);

#endif
