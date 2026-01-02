#include "stage_01.h"
#include <stdlib.h>
#include "../../world/glyph.h"

// ============================================
// Stage 1 Map Data
// ============================================
static const char* MAP_LINES[] = {
"#######################",
"#...B................0#",
"#.@..............A...1#",
"#.......C............2#",
"###########.###########",
"          #$#          ", // Replaced % with $ for Event Door
"          #.#          ",
"          #.#          ",
"          #.#          ",
"###########.###########",
"#.....................#",
"#..a...............a..#",
"#.......a.............#",
"#######################"
};

// ============================================
// Stage 1 Chests
// ============================================
static const ChestConfig CHESTS[] = {
    // receive_mission Flag checking added
    {'0', "weapon", "초보자 검", NULL, { .reqFlag="receive_mission", .reqVal=1, .failMsg="역무원에게 말을 먼저 걸어야 할 것 같다." } },
    {'1', "armor",  "가죽 갑옷", NULL, { .reqFlag="receive_mission", .reqVal=1, .failMsg="역무원에게 말을 먼저 걸어야 할 것 같다." } },
    {'2', "item",   "HP 포션",   NULL, { .reqFlag="receive_mission", .reqVal=1, .failMsg="역무원에게 말을 먼저 걸어야 할 것 같다." } },
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

// Type 'b': 스켈레톤병사
static const char* DIALOGUES_SKELETON[] = {
    "달그락... 달그락...",
    "생명체의.. 냄새가.. 난다.."
};

static const EnemyConfig ENEMIES[] = {
    {
        .tile = 'a', 
        .name = "맹혹한고양이", 
        .glyph = "🐈",
        .imagePath = "assets/enemy/texture_sprite/cat_tile.png",
        .portraitPath = "assets/enemy/portrait/1a.png",
        .width = 1, .height = 1,
        .spriteRows = 1, .spriteCols = 4, 
        // 1x4 Directional Map: {Row, Col}
        // User Spec: 1:Down, 2:Up, 3:Left, 4:Right
        .animDown = {0, 0}, .animUp = {0, 1}, .animLeft = {0, 2}, .animRight = {0, 3},
        .maxHp = 5,
        .attackMin = 1, .attackMax = 3,
        .chaseOnSight = 1, .attackOnSight = 1, 
        .detectionRange = 8, 
        .moveInterval = 1.0f, .attackInterval = 1.0f,
        .dialogues = DIALOGUES_A,
        .dialogueCount = sizeof(DIALOGUES_A) / sizeof(DIALOGUES_A[0]),
        .dialogueColor = COLOR_BRIGHT_CYAN,
        .expReward = 50 // 2 kills = Level Up
    },
    {
        .tile = 'b', 
        .name = "스켈레톤병사", 
        .glyph = COLOR_WHITE "💀" COLOR_RESET,
        .imagePath = "assets/enemy/texture_sprite/skel_tile.png", // To be set by user
        .portraitPath = "assets/enemy/portrait/skel_port.png", // To be set by user
        .width = 1, .height = 1,
        
        .spriteRows = 1, .spriteCols = 4,
        .animDown = {0, 0}, .animUp = {0, 1}, .animLeft = {0, 2}, .animRight = {0, 3},
        
        .maxHp = 15,
        .attackMin = 3, .attackMax = 5,
        .chaseOnSight = 1, .attackOnSight = 1, 
        .detectionRange = 6, 
        .moveInterval = 1.2f, .attackInterval = 1.2f,
        .dialogues = DIALOGUES_SKELETON,
        .dialogueCount = sizeof(DIALOGUES_SKELETON) / sizeof(DIALOGUES_SKELETON[0]),
        .dialogueColor = COLOR_WHITE,
        .expReward = 80
    },
    {
        .tile = 'y', 
        .name = "최악의로보트 아카카", 
        .glyph = COLOR_RED "🤖" COLOR_RESET,
        .imagePath = "assets/enemy/texture/security_robot.png",
        .portraitPath = "assets/enemy/portrait/1b.png",
        .width = 1, .height = 1,
        .maxHp = 10,
        .attackMin = 100, .attackMax = 100, // Insta-kill logic (Guard)
        .chaseOnSight = 0, .attackOnSight = 0, // Guard mode
        .detectionRange = 5, 
        .moveInterval = 2.0f, .attackInterval = 2.0f,
        .dialogues = DIALOGUES_B,
        .dialogueCount = sizeof(DIALOGUES_B) / sizeof(DIALOGUES_B[0]),
        .dialogueColor = COLOR_RED,
        .expReward = 200
    },
    {
        // ★ Boss C (Multi-Tile)
        .tile = 'c',
        .name = "SCP-682",
        .glyph = "👹",
        .imagePath = "assets/enemy/texture_sprite/1c.png", // Use Sprite Sheet
        .portraitPath = "assets/enemy/portrait/1c.png", 
        .width = 2, .height = 2,
        .spriteRows = 2, .spriteCols = 2, // 2x2 Sprite Sheet
        // Legacy 2x2 Map
        .animRight={0,0}, .animLeft={0,1}, .animUp={1,0}, .animDown={1,1},
        
        .maxHp = 50,
        .attackMin = 5, .attackMax = 10,
        .chaseOnSight = 1, .attackOnSight = 1,
        .detectionRange = 10,
        .moveInterval = 0.8f, .attackInterval = 1.5f,
        .dialogues = NULL, 
        .dialogueCount = 0,
        .dialogueColor = COLOR_BRIGHT_RED,
        .expReward = 1000 // Boss Reward
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
    "제 뒤에 있는 장비를 챙겨가세요. 행운을 빕니다!" // Added confirmation line
};

static const char* NPC_DIALOGUES_B[] = {
    "으아아아악!!!",
    "집에 가고싶어!!!!",
    "사람살려!!! 괴물이야!!!",    
};

static const char* NPC_DIALOGUES_C[] = {
    "적의 공격이 너무나 강력한가요? 0 키를 눌러서 방패를 사용하세요.",
    "적의 공격을 먼저 막아낸 이후에 방어를 풀고 반격하세요!",
    "그리고 다시 적이 공격해올때까지 방어태세를 취하세요.",    
    "반복하면 어떤 적이든 이길 수 있습니다.",        
};

static const char* NPC_DIALOGUES_A_ALT[] = {
    "야수를 물리치셨나요?\n야수를 물리쳐야만 이곳에서 나갈 수 있습니다...",
    "야수를 물리치신 후에 돌아오세요..."    
};

static const NPCConfig NPCS[] = {
    {
        .tile = 'A',
        .name = "역무원",
        .glyph = "👮", // Police/Guard
        .imagePath = "assets/npc/texture/staff1.png",
        .faceImagePath = "assets/npc/portrait/1A_face.png",
        .dialogues = NPC_DIALOGUES_A,
        .dialogueCount = sizeof(NPC_DIALOGUES_A)/sizeof(NPC_DIALOGUES_A[0]),
        .useDialogueBox = 1,
        // Event: Give Key
        .event = { .giveItem="철문열쇠1", .setFlag="receive_mission", .setVal=1 }
    },
    {
        .tile = 'B',
        .name = "시민",
        .glyph = COLOR_GREEN "웃" COLOR_RESET,
        .imagePath = "assets/npc/texture/staff2.png", // Using same sprite for now
        .faceImagePath = NULL,
        .dialogues = NPC_DIALOGUES_B,
        .dialogueCount = sizeof(NPC_DIALOGUES_B)/sizeof(NPC_DIALOGUES_B[0]),
        .useDialogueBox = 0,
        .canTrade = 0
    },
    {
        .tile = 'C',
        .name = "안내원",
        .glyph = COLOR_YELLOW "윽" COLOR_RESET,
        .imagePath = "assets/npc/texture/staff2.png", // Using same sprite for now
        .faceImagePath = NULL,
        .dialogues = NPC_DIALOGUES_C,
        .dialogueCount = sizeof(NPC_DIALOGUES_C)/sizeof(NPC_DIALOGUES_C[0]),
        .useDialogueBox = 0,
        .canTrade = 0
    }
};

static const EventConfig EVENT_A_ALT = { NULL, 0, NULL, 0, NULL, 0, NULL, NULL }; // No Reward

static const DialogueOverride DIALOGUE_OVERRIDES[] = {
    // 1. 역무원 (A) - 미션 수락 후
    { 
        .npcTile = 'A', 
        .reqFlag = "receive_mission", 
        .reqVal = 1, 
        .newDialogues = NPC_DIALOGUES_A_ALT, 
        .newDialogueCount = sizeof(NPC_DIALOGUES_A_ALT)/sizeof(NPC_DIALOGUES_A_ALT[0]),
        .newEvent = &EVENT_A_ALT // ★ Stop Giving Keys
    }
};

// ============================================
// Stage 1 Doors (New)
// ============================================
static const DoorConfig DOORS[] = {
    // Symbol $, Req '철문열쇠1', Consume=1
    { '$', { NULL, 0, "철문열쇠1", 1, NULL, 0, NULL, "열쇠가 필요하다." } }
};

// ============================================
// Stage 1 Quests
// ============================================
static const QuestConfig QUESTS[] = {
    // 퀘스트 1: 역무원 대화
    // 초기 상태는 game.c에서 설정 or "start_quest" 플래그 사용
    { "receive_mission", 1, "목표: 맹혹한 고양이를 처치하고 길을 뚫어라.", 50 }
};

// ============================================
// Stage 1 Story (Cinematics)
// ============================================

// --- Intro ---
static const CinematicLine INTRO_LINES[] = {
    { "", STYLE_NORMAL, 300, NULL }, 
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

// --- Outro ---
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
    .doors = DOORS,
    .doorCount = sizeof(DOORS) / sizeof(DOORS[0]),
    .quests = QUESTS,
    .questCount = sizeof(QUESTS) / sizeof(QUESTS[0]),
    .overrides = DIALOGUE_OVERRIDES,
    .overrideCount = sizeof(DIALOGUE_OVERRIDES) / sizeof(DIALOGUE_OVERRIDES[0]),
    .intro = &INTRO_CINEMATIC,
    .outro = &CLEAR_CINEMATIC
};
