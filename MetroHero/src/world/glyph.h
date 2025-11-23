// src/world/glyph.h - 타일 정의 추가

#ifndef GLYPH_H
#define GLYPH_H

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
#define COMBAT_ATTACK    COLOR_BRIGHT_RED
#define COMBAT_DAMAGE    COLOR_BRIGHT_YELLOW
#define COMBAT_DEATH     COLOR_BRIGHT_MAGENTA
#define COMBAT_ENEMY     COLOR_RED
#define COMBAT_DETECT    COLOR_CYAN

// 맵 타일
#define GLYPH_FLOOR    "  "
#define GLYPH_WALL     "\033[48;5;220m  \033[0m"
#define GLYPH_RAIL     "=="
#define GLYPH_EMPTY    "  "
#define GLYPH_DOOR     "++"
#define GLYPH_STAIRS   "▼▼"

// 엔티티
#define GLYPH_PLAYER   COLOR_BRIGHT_GREEN "옷" COLOR_RESET

// ★ 몹 (소문자 a-z)
#define GLYPH_MOB_CAT      "🐈"
#define GLYPH_MOB_ROBOT    COLOR_RED "🤖" COLOR_RESET
#define GLYPH_MOB_GENERIC  COLOR_YELLOW "몹" COLOR_RESET

// ★ NPC (대문자 A-Z)
#define GLYPH_NPC_MERCHANT COLOR_BRIGHT_CYAN "상" COLOR_RESET
#define GLYPH_NPC_GUARD    COLOR_BRIGHT_BLUE "경" COLOR_RESET
#define GLYPH_NPC_CITIZEN  COLOR_BRIGHT_WHITE "민" COLOR_RESET
#define GLYPH_NPC_GENERIC  COLOR_CYAN "N" COLOR_RESET

// ★ 상자 (숫자 0-9)
#define GLYPH_CHEST        "📦"

// 기타
#define GLYPH_TREE     "♣♣"
#define GLYPH_WATER    "≈≈"

// ========================
// 타일 문자 (맵 데이터용)
// ========================
#define TILE_FLOOR     '.'
#define TILE_WALL      '#'
#define TILE_RAIL      '='
#define TILE_DOOR      '+'
#define TILE_SPAWN     '@'

// ★ Entity 타일 분류
// 몹: 소문자 a-z
#define TILE_MOB_CAT    'a'
#define TILE_MOB_ROBOT  'b'

// NPC: 대문자 A-Z
#define TILE_NPC_MERCHANT 'A'
#define TILE_NPC_GUARD    'B'
#define TILE_NPC_CITIZEN  'C'

// 상자: 숫자 0-9
#define TILE_CHEST_0    '0'
#define TILE_CHEST_1    '1'
#define TILE_CHEST_2    '2'
#define TILE_CHEST_3    '3'
#define TILE_CHEST_4    '4'
#define TILE_CHEST_5    '5'
#define TILE_CHEST_6    '6'
#define TILE_CHEST_7    '7'
#define TILE_CHEST_8    '8'
#define TILE_CHEST_9    '9'

#endif