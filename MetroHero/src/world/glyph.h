#ifndef GLYPH_H
#define GLYPH_H

// ========================
// 렌더링 문자 상수 정의
// ========================

// ========================
// ANSI 색상 코드
// ========================
#define COLOR_RESET   "\033[0m"
#define COLOR_BLACK   "\033[30m"
#define COLOR_RED     "\033[31m"
#define COLOR_GREEN   "\033[32m"
#define COLOR_YELLOW  "\033[33m"
#define COLOR_BLUE    "\033[34m"
#define COLOR_MAGENTA "\033[35m"
#define COLOR_CYAN    "\033[36m"
#define COLOR_WHITE   "\033[37m"

// 밝은 색상
#define COLOR_BRIGHT_BLACK   "\033[90m"
#define COLOR_BRIGHT_RED     "\033[91m"
#define COLOR_BRIGHT_GREEN   "\033[92m"
#define COLOR_BRIGHT_YELLOW  "\033[93m"
#define COLOR_BRIGHT_BLUE    "\033[94m"
#define COLOR_BRIGHT_MAGENTA "\033[95m"
#define COLOR_BRIGHT_CYAN    "\033[96m"
#define COLOR_BRIGHT_WHITE   "\033[97m"

// 배경색
#define BG_BLACK   "\033[40m"
#define BG_RED     "\033[41m"
#define BG_GREEN   "\033[42m"
#define BG_YELLOW  "\033[43m"
#define BG_BLUE    "\033[44m"
#define BG_MAGENTA "\033[45m"
#define BG_CYAN    "\033[46m"
#define BG_WHITE   "\033[47m"

// 전투 메시지용 색상
#define COMBAT_ATTACK    COLOR_BRIGHT_RED      // 공격 (밝은 빨강)
#define COMBAT_DAMAGE    COLOR_BRIGHT_YELLOW   // 데미지 (밝은 노랑)
#define COMBAT_DEATH     COLOR_BRIGHT_MAGENTA  // 처치 (밝은 보라)
#define COMBAT_ENEMY     COLOR_RED             // 적 반격 (빨강)
#define COMBAT_DETECT    COLOR_CYAN            // 적 발견 (청록)

// 맵 타일
#define GLYPH_FLOOR    "  "    // 바닥
//#define GLYPH_WALL     COLOR_YELLOW "██" COLOR_RESET    // ★ 노란색 벽
// #define GLYPH_WALL     "\033[48;5;220m  \033[0m"  // 노란 배경만
#define GLYPH_WALL     "██"    // 단일 패턴 벽
//#define GLYPH_WALL     "\033[48;5;220m\033[38;5;234m˅ \033[0m"
#define GLYPH_RAIL     "=="    // 철도 레일
#define GLYPH_EMPTY    "  "    // 빈 공간
#define GLYPH_DOOR     "++"    // 문
#define GLYPH_STAIRS   "▼▼"    // 계단

// 엔티티
#define GLYPH_PLAYER   COLOR_BRIGHT_GREEN "옷" COLOR_RESET    // 밝은 녹색
#define GLYPH_DOG      "🐈‍"    // ★ 고블린 추가
#define GLYPH_ORC      "옷"    // 나중에 사용
#define GLYPH_NPC      "민"    // NPC
#define GLYPH_ITEM     "물"    // 아이템

#define GLYPH_CHEST  "📦"

// 기타
#define GLYPH_TREE     "♣♣"    // 나무
#define GLYPH_WATER    "≈≈"    // 물

// ========================
// 타일 문자 (맵 데이터용)
// ========================
#define TILE_FLOOR     '.'
#define TILE_WALL      '#'
#define TILE_RAIL      '='
#define TILE_DOOR      '+'
#define TILE_SPAWN     '@'     // ★ 스폰 포인트

// ★ 적 타일
#define TILE_GOBLIN    'a'
#define TILE_ORC       'b'

#endif