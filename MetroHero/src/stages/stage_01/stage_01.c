#include "stage_01.h"
#include <stdlib.h>
#include "../../world/glyph.h"

// ============================================
// Stage 1 Map Data
// ============================================
static const char* MAP_LINES[] = {
"#######################",
"#012..................#",
"#...@.......A.........#",
"#.............B.......#",
"########.##############",
"       #%#             ",
"       #.#             ",
"       #.#             ",
"       #.#             ",
"########.##############",
"#.....................#",
"#......a.....a.....a..#",
"#.....................#",
"#######################"
};

// ============================================
// Stage 1 Chests
// ============================================
static const ChestConfig CHESTS[] = {
    {'0', "weapon", "초보자 검"},
    {'1', "armor",  "가죽 갑옷"},
    {'2', "item",   "HP 포션"},
    {'3', "item",   "민첩의 물약"},
    {'4', "item",   "힘의 물약"},
};

// ============================================
// Stage 1 Enemies
// ============================================

// Type 'a': 맹혹한고양이
static const char* DIALOGUES_A[] = {
    "나는 잔인한 냥코다옹",
    "냥코냥코 대전쟁"
};

// Type 'b': 경비로봇
static const char* DIALOGUES_B[] = {
    "전철역 내 침입자를 인식했습니다...",
    "경고: 비인가 인원입니다.",
    "돌아가라 날 건들면 후회할것이다"
};

static const EnemyConfig ENEMIES[] = {
    {
        'a', "맹혹한고양이", "🐈",
        "assets/cat.png",           // 맵 타일 이미지
        "assets/enemy/1a.png",      // ★ 전투 박스 초상화 이미지
        5,              // maxHp
        1, 3,           // attackMin, Max
        // Defense removed
        // AI: 선추격O, 선공O, 감지8, 이속1.0, 공속1.0
        1, 1, 8, 1.0f, 1.0f,

        DIALOGUES_A,
        sizeof(DIALOGUES_A) / sizeof(DIALOGUES_A[0]),
        COLOR_BRIGHT_CYAN
    },
    {
        'b', "최악의로보트 아카카", COLOR_RED "🤖" COLOR_RESET,
        "assets/security_robot.png", // 맵 타일 이미지
        "assets/enemy/1b.png",       // ★ 전투 박스 초상화 이미지
        10,
        100, 100,
        // Defense removed
        // AI: 선추격X(지키기), 선공O(접근하면), 감지5, 이속2.0(느림), 공속2.0(느림)
        0, 0, 5, 2.0f, 2.0f,

        DIALOGUES_B,
        sizeof(DIALOGUES_B) / sizeof(DIALOGUES_B[0]),
        COLOR_RED
    }
};

// ============================================
// Stage 1 NPCs
// ============================================

// A: 역무원
static const char* NPC_DIALOGUES_A[] = {
    "으으윽... 도와주세요...\n누구신지 모르겠지만 제발 도와주세요...\n저는 성균관대역의 역무원입니다...",
    "당신이 나타나기 직전에 갑자기 거대한 굉음과 함께 역이 혼란에 빠졌습니다...",    
    "저 아래의 문 밖에 무서운 야수가 위협하고 있어요.. 제발 야수를 물리쳐 주십시오..",
    // "일단 이 역의 역장을 물리치고 의왕역으로 넘어가야 할 것입니다...",
    // "행운을 빕니다... 후후후..."
};

// C: Citizen
static const char* NPC_DIALOGUES_B[] = {
    "으아아아악!!!",
    "집에 가고싶어!!!!",
    // "지하에는 이상한 것들이 많다던데...",
    // "조심히 다니세요!"
};

static const NPCConfig NPCS[] = {
    {
        'A', "우건박", COLOR_BRIGHT_BLUE "읏" COLOR_RESET,
        "assets/old_man.png",
        "assets/npc/1A_face.png", // ★ Added Face
        NPC_DIALOGUES_A, sizeof(NPC_DIALOGUES_A)/sizeof(NPC_DIALOGUES_A[0]),
        0, "general", 1 // canTrade=0 currently but shopType set? Original code had canTrade=0.
    },
    {
        'B', "시민", COLOR_GREEN "웃" COLOR_RESET,
        "assets/citizen_black.png",
        NULL,
        NPC_DIALOGUES_B, sizeof(NPC_DIALOGUES_B)/sizeof(NPC_DIALOGUES_B[0]),
        0, "general", 0
    }
};

// ============================================
// Stage 1 Story (Cinematics)
// ============================================

// --- Intro ---
static const CinematicLine INTRO_LINES[] = {
    { "", STYLE_NORMAL, 300, NULL }, // Reverted to NULL
    { "[ STAGE 1 ]", STYLE_TITLE, 500, NULL },
    { "", STYLE_NORMAL, 200, NULL },
    { "성균관대역", STYLE_SUBTITLE, 800, NULL },
};

static const Cinematic INTRO_CINEMATIC = {
    NULL,
    INTRO_LINES,
    sizeof(INTRO_LINES) / sizeof(INTRO_LINES[0]),
    25,
    1,
    COLOR_YELLOW,
    COLOR_WHITE
};

// --- Outro (Clear) ---
static const CinematicLine CLEAR_LINES[] = {
    { "", STYLE_NORMAL, 300, NULL },
    { "★ STAGE CLEAR ★", STYLE_TITLE, 800, NULL },
    { "", STYLE_NORMAL, 500, NULL },
    { "성균관대역을 정화했다!", STYLE_TYPEWRITER, 600, NULL },
    { "", STYLE_NORMAL, 300, NULL },
    { "하지만 어둠은 더 깊은 곳에서 기다리고 있다...", STYLE_TYPEWRITER, 0, NULL },
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
const StageData STAGE_01_DATA = {
    .stageId = 1,
    .mapLines = MAP_LINES,
    .mapHeight = sizeof(MAP_LINES) / sizeof(MAP_LINES[0]),
    .chests = CHESTS,
    .chestCount = sizeof(CHESTS) / sizeof(CHESTS[0]),
    .enemies = ENEMIES,
    .enemyCount = sizeof(ENEMIES) / sizeof(ENEMIES[0]),
    .npcs = NPCS,
    .npcCount = sizeof(NPCS) / sizeof(NPCS[0]),
    .doors = NULL,
    .doorCount = 0,
    .intro = &INTRO_CINEMATIC,
    .outro = &CLEAR_CINEMATIC
};
