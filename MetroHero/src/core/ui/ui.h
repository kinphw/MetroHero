#ifndef UI_H
#define UI_H

#include "../../entity/player.h"
#include "../../entity/npc.h"  // ★ 추가

#define SCREEN_W 120
#define SCREEN_H 30

// 상단 전체 높이
#define TOP_H 20

// ★ 뷰포트 영역 (왼쪽)
#define VIEWPORT_X 0
#define VIEWPORT_Y 0
#define VIEWPORT_W 80    // 40칸 × 2 (전각)
#define VIEWPORT_H 20

// ★ 상태창 (오른쪽 상단)
#define STATUS_X 82
#define STATUS_Y 0
#define STATUS_W 38
#define STATUS_H 10

// ★ 장비창 (오른쪽 중단)
#define EQUIP_X 82
#define EQUIP_Y 10
#define EQUIP_W 38
#define EQUIP_H 6

// ★ 대화창 (상태창/장비창 위에 오버레이)
#define DIALOGUE_X 82
#define DIALOGUE_Y 0
#define DIALOGUE_W 38
#define DIALOGUE_H 16

// ★ 로그창 (하단 전체)
#define LOG_X 0
#define LOG_Y 20
#define LOG_W 120
#define LOG_H 9

void console_goto(int x , int y);
void console_clear_fast(void);

void ui_draw_equipment(const Player* p , int x , int y , int w , int h);  // ★ 이 줄 확인
void ui_draw_stats(const Player* p , int x , int y , int w , int h);  // ★ 추가

void ui_add_log(const char* msg);
void ui_draw_log(int x , int y , int w , int h);

void ui_draw_combat_effect(int x , int y);  // 전투 이펙트
void ui_clear_combat_effect(int x , int y); // 이펙트 제거

// ★ 대화창 관련 함수
void ui_draw_dialogue(const NPC* npc , int x , int y , int w , int h);
void ui_clear_dialogue_area(int x , int y , int w , int h);

int display_width(const char* str);

#endif
