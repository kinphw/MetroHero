#include "stage_03.h"
#include <stdlib.h>
#include "../../world/glyph.h"

// ============================================
// Stage 3 Map Data
// ============================================
static const char* MAP_LINES[] = {
    "####################",
    "#..................#",
    "#....####..........#",
    "#....#..#..........#",
    "#....####..........#",
    "#..................#",
    "#..................#",
    "#..................#",
    "#..................#",
    "####################"
};

// ============================================
// Stage 3 Chests
// ============================================
static const ChestConfig CHESTS[] = {
    {'0', "weapon", "마력의 검"},
    {'1', "armor",  "마나 갑옷"},
    {'2', "item",   "최대 HP +5"},
    {'3', "item",   "공격력 +2"},
    {'4', "item",   "방어력 +2"},
};

// ============================================
// Stage 3 Enemies
// ============================================

// Generic Monster (Default case from original)
static const char* DIALOGUES_M[] = {
    "키에에에엑!",
    "으르르르르…",
    "끼아아악!"
};

static const EnemyConfig ENEMIES[] = {
    {
        'm', "몬스터", COLOR_YELLOW "몹" COLOR_RESET,
        "assets/monster.png",      // 맵 타일 이미지
        "assets/enemy/1m.png",     // ★ 전투 박스 초상화 이미지
        3,              // maxHp
        1, 2,           // attackMin, Max
        0,              // defense
        // AI: 선추격O, 선공O, 감지6, 이속1.5, 공속1.5
        1, 1, 6, 1.5f, 1.5f,

        DIALOGUES_M,
        sizeof(DIALOGUES_M) / sizeof(DIALOGUES_M[0]),
        COLOR_YELLOW
    }
};

// ============================================
// Stage 3 Story
// ============================================

static const CinematicLine INTRO_LINES[] = {
    { "", STYLE_NORMAL, 300 },
    { "[ STAGE 3 ]", STYLE_TITLE, 500 },
    { "", STYLE_NORMAL, 200 },
    { "서울역 지하 심층부", STYLE_SUBTITLE, 800 },
    { "", STYLE_NORMAL, 500 },
    { "모든 것의 시작점...", STYLE_TYPEWRITER, 600 },
    { "최종 결전이 기다린다.", STYLE_TYPEWRITER, 0 },
};

static const Cinematic INTRO_CINEMATIC = {
    NULL,
    INTRO_LINES,
    sizeof(INTRO_LINES) / sizeof(INTRO_LINES[0]),
    25,
    1, // showSkipHint
    COLOR_BRIGHT_RED, // Border
    COLOR_WHITE
};

static const CinematicLine CLEAR_LINES[] = {
    { "", STYLE_NORMAL, 300 },
    { "★ STAGE CLEAR ★", STYLE_TITLE, 800 },
    { "", STYLE_NORMAL, 500 },
    { "모든 어둠을 물리쳤다!", STYLE_TYPEWRITER, 600 },
    { "", STYLE_NORMAL, 300 },
    { "서울 지하철에 다시 평화가 찾아왔다.", STYLE_TYPEWRITER, 0 },
};

static const Cinematic CLEAR_CINEMATIC = {
    NULL,
    CLEAR_LINES,
    sizeof(CLEAR_LINES) / sizeof(CLEAR_LINES[0]),
    30,
    1,
    COLOR_BRIGHT_YELLOW,
    COLOR_WHITE
};

// ============================================
// Stage Package
// ============================================
const StageData STAGE_03_DATA = {
    3,
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
