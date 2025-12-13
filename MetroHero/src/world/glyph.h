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
#define COLOR_GRAY    "\033[90m"
#define COLOR_DARK_GRAY "\033[90m"

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

// Bright Backgrounds
#define BG_BRIGHT_BLACK   "\033[100m"
#define BG_BRIGHT_RED     "\033[101m"
#define BG_BRIGHT_GREEN   "\033[102m"
#define BG_BRIGHT_YELLOW  "\033[103m"
#define BG_BRIGHT_BLUE    "\033[104m"
#define BG_BRIGHT_MAGENTA "\033[105m"
#define BG_BRIGHT_CYAN    "\033[106m"
#define BG_BRIGHT_WHITE   "\033[107m"

// 전투 메시지용 색상
#define COMBAT_ATTACK    COLOR_BRIGHT_RED
#define COMBAT_DAMAGE    COLOR_BRIGHT_YELLOW
#define COMBAT_DEATH     COLOR_BRIGHT_MAGENTA
#define COMBAT_ENEMY     COLOR_RED
#define COMBAT_DETECT    COLOR_CYAN

// 맵 타일
// ========================
// 맵 타일 (배경/구조물) -> map_data.c 로 이동됨 global palette 사용

// 5. 상자 (Chests - map.c 로직에서 사용)
#define GLYPH_CHEST_CLOSED   "📦"
#define GLYPH_CHEST_OPEN     "📭"

// 6. 엔티티 (Entities)
#define GLYPH_PLAYER   COLOR_BRIGHT_GREEN "옷" COLOR_RESET

// ========================
// 호환성 (Compatibility)
// ========================
// (Moved to map_data.c)
#define GLYPH_EMPTY    "  "  // Still used as default fallback in code

// ========================
// 타일 문자 (맵 데이터용)
// ========================
#define TILE_FLOOR     '.'
// ... (Keep existing TILE_ definitions if useful, or remove if unused)

#endif