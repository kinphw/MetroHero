#include "stage_02.h"
#include <stdlib.h>
#include "../../world/glyph.h"

// ============================================
// Stage 2 Map Data
// ============================================
static const char* MAP_LINES[] = {
    "########################################",
    "#====================================..#",
    "#====================================..#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "#......................................#",
    "########################################"
};

// ============================================
// Stage 2 Chests
// ============================================
static const ChestConfig CHESTS[] = {
    {'0', "weapon", "강철 검"},
    {'1', "armor",  "철 갑옷"},
    {'2', "item",   "대형 HP 포션"},
    {'3', "item",   "스태미너 물약"},
    {'4', "item",   "신비한 원석"},
};

// ============================================
// Stage 2 Enemies
// ============================================

// Type 'b': 경비로봇 (Available in Stage 2 context)
static const char* DIALOGUES_B[] = {
    "전철역 내 침입자를 인식했습니다...",
    "경고: 비인가 인원입니다.",
    "돌아가라 날 건들면 후회할것이다"
};

static const EnemyConfig ENEMIES[] = {
    {
        'b', "경비로봇",
        10,
        100, 100,
        0,
        DIALOGUES_B,
        sizeof(DIALOGUES_B) / sizeof(DIALOGUES_B[0]),
        COLOR_RED
    }
};

// ============================================
// Stage 2 Story
// ============================================

static const CinematicLine INTRO_LINES[] = {
    { "", STYLE_NORMAL, 300 },
    { "[ STAGE 2 ]", STYLE_TITLE, 500 },
    { "", STYLE_NORMAL, 200 },
    { "화서역 환승 통로", STYLE_SUBTITLE, 800 },
    { "", STYLE_NORMAL, 500 },
    { "미로처럼 얽힌 통로...", STYLE_TYPEWRITER, 600 },
    { "더 강력한 적들이 기다리고 있다.", STYLE_TYPEWRITER, 0 },
};

static const Cinematic INTRO_CINEMATIC = {
    NULL,
    INTRO_LINES,
    sizeof(INTRO_LINES) / sizeof(INTRO_LINES[0]),
    25,
    1,
    COLOR_BRIGHT_YELLOW,
    COLOR_WHITE
};

static const CinematicLine CLEAR_LINES[] = {
    { "", STYLE_NORMAL, 300 },
    { "★ STAGE CLEAR ★", STYLE_TITLE, 800 },
    { "", STYLE_NORMAL, 500 },
    { "환승 통로를 돌파했다!", STYLE_TYPEWRITER, 600 },
    { "", STYLE_NORMAL, 300 },
    { "최종 목적지가 눈앞에 있다...", STYLE_TYPEWRITER, 0 },
};

static const Cinematic CLEAR_CINEMATIC = {
    NULL,
    CLEAR_LINES,
    sizeof(CLEAR_LINES) / sizeof(CLEAR_LINES[0]),
    30,
    1,
    COLOR_BRIGHT_GREEN,
    COLOR_WHITE
};

// ============================================
// Stage Package
// ============================================
const StageData STAGE_02_DATA = {
    2,
    MAP_LINES,
    sizeof(MAP_LINES) / sizeof(MAP_LINES[0]),
    CHESTS,
    sizeof(CHESTS) / sizeof(CHESTS[0]),
    ENEMIES,
    sizeof(ENEMIES) / sizeof(ENEMIES[0]),
    NULL, 0, // NPCS
    &INTRO_CINEMATIC,
    &CLEAR_CINEMATIC
};
