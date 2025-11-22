#ifndef UI_H
#define UI_H

#include "../entity/player.h"

#define SCREEN_W 120
#define SCREEN_H 30

// 상단 전체 높이
#define TOP_H 20

// 맵 설정
//#define MAP_W 40                // 논리적 가로 칸 수
//#define MAP_H 20                // 맵 세로 수
//#define MAP_RENDER_W (MAP_W*2)  // 실제 출력되는 문자 폭 80

// ★ 맵 렌더링 최대 너비 (동적 크기 지원)
#define MAX_MAP_RENDER_W 100  // 최대값만 정의

// 장비창(오른쪽) - 고정 위치
#define EQ_X 80       // ★ 고정값으로 변경 (MAX_MAP_W * 2)
#define EQ_W (SCREEN_W - EQ_X)
#define EQ_H TOP_H

// 로그창(하단)
#define LOG_Y TOP_H
#define LOG_W SCREEN_W
#define LOG_H (SCREEN_H - TOP_H -1)


void console_goto(int x , int y);

void ui_draw_equipment(const Player* p , int x , int y , int w , int h);  // ★ 이 줄 확인
void ui_draw_stats(const Player* p , int x , int y , int w , int h);  // ★ 추가


void ui_add_log(const char* msg);
void ui_draw_log(int x , int y , int w , int h);

void console_clear_fast(void);

#endif
