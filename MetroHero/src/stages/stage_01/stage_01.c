#include "stage_01.h"
#include <stdlib.h>
#include "../../world/glyph.h"

// ============================================
// Stage 1 Map Data
// ============================================
// ============================================
// Stage 1 Map Data (Floor 1: Station)
// ============================================
static const char* MAP_FLOOR1_LINES[] = {
    "##$$##$$##$$##$$##$$###                           ",
    "#!..B................0#                           ",
    "#.@..............A...1#                           ",
    "#.......C............2#                           ",
    "##$$##$$###.##$$##$$###                           ",
    "          #+#                                     ",
    "          #.#                              &&&&&&&",
    "          #.#                              &....c&",
    "          #.#                              &.....&",
    "###########.###########                    &.....&",
    "#..a..a........a.....!##########           &.....&",
    "#==================a..+........#           &.....&",
    "#.......a.............########.#           &&..&&&",
    "#######################      #.#            #!.#  ",
    "                             #a#            #..#  ",
    "                             #.#            #..#  ",
    "                             #.#            #..#  ",
    "                             #.#            #..#  ",
    "                             #.#            #..#  ",
    "                         #####.#####        #..#  ",
    "                         #.........#        #..#  ",
    "                         #.b.....b.#        #..#  ",
    "                         #.........#        #..#  ",
    "                         #.........#        #..#  ",
    "                         #.........#        #a.#  ",
    "                         #.........#        #..#  ",
    "                         #.........##########..#  ",
    "                         #.b.....b.!...........#  ",
    "                         #.....................#  ",
    "                         #######################  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
    "                                                  ",
};

// Warp Config for Floor 1
static const WarpConfig WARPS_FLOOR1[] = {
    { '>', 1, '<' } // '>' tile goes to Floor 1 (index 1), at '<' tile
};

// ============================================
// Stage 1 Map Data (Floor 2: Underground)
// ============================================
static const char* MAP_FLOOR2_LINES[] = {
"#######################",
"#<....................#", // '<' Arrival from Floor 1
"#.....................#",
"#.........b...........#", // Skeleton
"#.....................#",
"#.....................#",
"#.....................#",
"#.....................#",
"#.........z...........#", // Zombie Added
"#.........b...........#",
"#.....................#",
"#.....................#",
"#######################"
};

// Warp Config for Floor 2
static const WarpConfig WARPS_FLOOR2[] = {
    { '<', 0, '>' } // '<' tile goes to Floor 0 (index 0), at '>' tile
};

// ============================================
// Floor Configuration Package
// ============================================
static const SubMapConfig FLOORS[] = {
    // Floor 0
    { 
        MAP_FLOOR1_LINES, 
        sizeof(MAP_FLOOR1_LINES)/sizeof(MAP_FLOOR1_LINES[0]),
        WARPS_FLOOR1,
        sizeof(WARPS_FLOOR1)/sizeof(WARPS_FLOOR1[0])
    },
    // Floor 1
    { 
        MAP_FLOOR2_LINES, 
        sizeof(MAP_FLOOR2_LINES)/sizeof(MAP_FLOOR2_LINES[0]),
        WARPS_FLOOR2,
        sizeof(WARPS_FLOOR2)/sizeof(WARPS_FLOOR2[0])
    }
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

// Type 'z': 좀비 역무원
static const char* DIALOGUES_ZOMBIE[] = {
    "표... 표를... 보여줘...",
    "이번 역은... 지옥... 지옥행입니다...",
    "크아아아..."
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
        .expReward = 50, // 2 kills = Level Up
        
        // ★ Random Move
        .allowRandomMove = 1,
        .randomMoveInterval = 0.5f,
        
        // ★ Event: Kill Count
        .event = { .setFlag="kill_cat_count", .setVal=0 } // 0 means Add 1
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
        .tile = 'c', 
        .name = "좀비 역무원", 
        .glyph = COLOR_GREEN "🧟" COLOR_RESET,
        .imagePath = "assets/enemy/texture_sprite/zomerk_tile.png", // User will set
        .portraitPath = "assets/enemy/portrait/zomerk_port.png", // User will set
        .width = 1, .height = 1,
        
        .spriteRows = 1, .spriteCols = 4,
        .animDown = {0, 0}, .animUp = {0, 1}, .animLeft = {0, 2}, .animRight = {0, 3},
        
        .maxHp = 25, // Stronger than Skeleton (15)
        .attackMin = 5, .attackMax = 8, // Stronger than Skeleton (3-5)
        .chaseOnSight = 1, .attackOnSight = 1, 
        .detectionRange = 7, 
        .moveInterval = 1.1f, .attackInterval = 1.1f,
        .dialogues = DIALOGUES_ZOMBIE,
        .dialogueCount = sizeof(DIALOGUES_ZOMBIE) / sizeof(DIALOGUES_ZOMBIE[0]),
        .dialogueColor = COLOR_GREEN,
        .expReward = 120
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

// 1. 역무원 (A) - All Dialogues Merged (Original Text + Return)
static const char* ALL_DIALOGUES_A[] = {
    // 0~3: Default (Original Long Text)
    "으으윽... 도와주세요...\n누구신지 모르겠지만 제발 도와주세요...\n저는 성균관대역의 역무원입니다...",
    "당신이 나타나기 직전에 갑자기 거대한 굉음과 함께 역이 혼란에 빠졌습니다...",
    "저 아래의 문 밖에 무서운 야수가 위협하고 있어요.. 제발 야수를 물리쳐 주십시오..",
    "제 뒤에 있는 장비를 챙겨가세요. 행운을 빕니다!",
    
    // 4~5: Mission Active (Original Alt Text)
    "야수를 물리치셨나요?\n야수를 물리쳐야만 이곳에서 나갈 수 있습니다...",
    "야수를 물리치신 후에 돌아오세요...",
    
    // 6~9: Mission Complete (New Return Text)
    "맹혹한 고양이들을 모두 쓰러뜨리셨군요... 느낄 수 있습니다.",
    "이제 주위가 안전해진 것 같습니다.",
    "이 열쇠는 역장실로 가는 철문열쇠입니다. 이 열쇠를 드리겠습니다.",
    "역장님께서는 아마 왜 이런 사태가 벌어졌는지 알고 계실 것 같군요....",

    "(이제 당신에게 할 말은 없는 모양이다)",
};

static const EventConfig EVENT_A_START = { 
    .giveItem="철문열쇠1", .setFlag="receive_mission", .setVal=1 
};

static const EventConfig EVENT_A_COMPLETE = { 
    .setFlag = "mission_complete", .setVal = 1, 
    .giveItem = "철문열쇠2" // Reward changed to Key 2
};

// Dialogue Branches for NPC A
static const DialogueBranch BRANCHES_A[] = {
    // Priority 1: Post-Mission (Already completed)
    { "mission_complete", 1, 10, 1, NULL },

    // Priority 2: Mission Complete Reward (Flag: kill_cat_count >= 5)
    { "kill_cat_count", 5, 6, 4, &EVENT_A_COMPLETE },
    
    // Priority 3: Mission Active (Flag: receive_mission >= 1)
    { "receive_mission", 1, 4, 2, NULL }, 
    
    // Priority 4: Default (No Flag)
    { NULL, 0, 0, 4, &EVENT_A_START } // Give Key
};



// 2. 다른 NPC들...
static const char* NPC_DIALOGUES_B[] = {
    "으아아아악!!!",
    "집에 가고싶어!!!!",
    "사람살려!!! 괴물이야!!!",    
};
static const DialogueBranch BRANCHES_B[] = {
    { NULL, 0, 0, 3, NULL } // Count 3
};

static const char* NPC_DIALOGUES_C[] = {
    "적의 공격이 너무나 강력한가요? 0 키를 눌러서 방패를 사용하세요.",
    "적의 공격을 먼저 막아낸 이후에 방어를 풀고 반격하세요!",
    "그리고 다시 적이 공격해올때까지 방어태세를 취하세요.",    
    "반복하면 어떤 적이든 이길 수 있습니다.",        
};
static const DialogueBranch BRANCHES_C[] = {
    { NULL, 0, 0, 4, NULL }
};

static const NPCConfig NPCS[] = {
    {
        .tile = 'A',
        .name = "역무원",
        .glyph = "👮", // Police/Guard
        .imagePath = "assets/npc/texture/staff1.png",
        .faceImagePath = "assets/npc/portrait/1A_face.png",
        
        .dialogues = ALL_DIALOGUES_A,
        .dialogueCount = sizeof(ALL_DIALOGUES_A)/sizeof(ALL_DIALOGUES_A[0]),
        
        .branches = BRANCHES_A,
        .branchCount = sizeof(BRANCHES_A)/sizeof(BRANCHES_A[0]),
        
        .useDialogueBox = 1,
        // Default Event (Give Key on first interaction naturally handled by Default Branch if we want? 
        // OR we trigger it via the Default Branch logic in npc.c?)
        // Wait, npc.c applies branch->event. 
        // The default branch has NULL event in my code above.
        // Original code had: .event = { .giveItem="철문열쇠1", .setFlag="receive_mission", .setVal=1 }
        // So the DEFAULT branch must have this event!
        // Otherwise the player never receives the key/mission.
        .canTrade = 0,
        .event = { .giveItem="철문열쇠1", .setFlag="receive_mission", .setVal=1 } // Fallback/Init
    },
    {
        .tile = 'B',
        .name = "시민",
        .glyph = COLOR_GREEN "웃" COLOR_RESET,
        .imagePath = "assets/npc/texture/staff2.png", 
        .faceImagePath = NULL,
        
        .dialogues = NPC_DIALOGUES_B,
        .dialogueCount = sizeof(NPC_DIALOGUES_B)/sizeof(NPC_DIALOGUES_B[0]),
        .branches = BRANCHES_B,
        .branchCount = sizeof(BRANCHES_B)/sizeof(BRANCHES_B[0]),
        
        .useDialogueBox = 0, // Originally 0
        .canTrade = 0
    },
    {
        .tile = 'C',
        .name = "안내원",
        .glyph = COLOR_YELLOW "윽" COLOR_RESET,
        .imagePath = "assets/npc/texture/staff2.png",
        .faceImagePath = NULL,
        
        .dialogues = NPC_DIALOGUES_C,
        .dialogueCount = sizeof(NPC_DIALOGUES_C)/sizeof(NPC_DIALOGUES_C[0]),
        .branches = BRANCHES_C,
        .branchCount = sizeof(BRANCHES_C)/sizeof(BRANCHES_C[0]),
        
        .useDialogueBox = 0, // Originally 0
        .canTrade = 0
    }
};

// ============================================
// Stage 1 Doors (New)
// ============================================
static const DoorConfig DOORS[] = {
    // 1. Unlocked Door (+) - Just a door.
    { '+', { NULL, 0, NULL, 0, NULL, 0, NULL, "문이 열렸다." } },
    
    // 2. First Locked Door (*) - Requires Key 1
    { '*', { NULL, 0, "철문열쇠1", 1, NULL, 0, NULL, "굳게 잠긴 문이다. (철문열쇠1 필요)" } },
    
    // 3. Second Locked Door (^) - Requires Key 2
    { '^', { NULL, 0, "철문열쇠2", 1, NULL, 0, NULL, "더 깊은 곳으로 가는 문이다. (철문열쇠2 필요)" } }
};

// ============================================
// Stage 1 Quests
// ============================================
static const QuestConfig QUESTS[] = {
    // 퀘스트 0: 시작 (조건 없음) -> "receive_mission" 1이 되면 종료
    { NULL, 0, "목표: 역무원을 찾아 대화하라.", NULL, 0, "receive_mission", 1, 0 },
    
    // 퀘스트 1: 고양이 처치 (x/5) -> "kill_cat_count" 5가 되면 종료 (즉시 Q2로 넘어감)
    { "receive_mission", 1, "목표: 맹혹한 고양이를 처치하고 길을 뚫어라.", "kill_cat_count", 5, "kill_cat_count", 5, 50 },
    
    // 퀘스트 2: 복귀 -> "mission_complete"가 되면 종료
    { "kill_cat_count", 5, "목표: 역무원에게 돌아가라.", NULL, 0, "mission_complete", 1, 0 },
    
    // 퀘스트 3: 역장실 찾기 (New)
    { "mission_complete", 1, "목표: 역장실을 찾아라.", NULL, 0, "found_office", 1, 0 }
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
// Stage 1 Map Events (Signposts)
// ============================================
static const MapEvent MAP_EVENTS[] = {
    // Floor 0 (Station)
    { 0, 1, 1, "성균관대역 - 안전지대" },
    { 0, 21, 10, "만남의 광장으로 가는 길" }, // Adjusted by user request previously? User edited it to 10,10.
    { 0, 35, 27, "역장실로 가는 방향" }, // Adjusted by user request previously? User edited it to 10,10.    
    { 0, 45, 13, "역장실" },
    
    // Floor 1 (Underground)
    { 1, 1, 1, "지하시설 - 관계자 외 출입금지" }
};

// ============================================
// Stage 1 Door Events (Coordinate Based)
// ============================================
static const DoorEvent DOOR_EVENTS[] = {
    // 1. Locked Door 1 (Tunnel Entrance)
    { 0, 11, 5, "철문열쇠1", 1, NULL, "굳게 잠겨있다. (철문열쇠1 필요)", "철커덩! 문이 열렸다." },
    
    // 2. Locked Door 2 (Exit)
    { 0, 22, 11, "철문열쇠2", 1, NULL, "더 깊은 곳으로 가는 문이다. (철문열쇠2 필요)", "문이 열렸다. 이제 더 깊은 곳으로..." }
};

// --------------------------------------------
// Boss Intro Cinematic
// --------------------------------------------
static const CinematicLine BOSS_INTRO_LINES[] = {
    { "", STYLE_NORMAL, 500, "assets/cinematic/1_boss.png" }, // ★ Image Start (Show immediately)
    { "역장실에 도착했다.", STYLE_TYPEWRITER, 1000, NULL },
    { "하지만 역장의 상태가 이상하다...", STYLE_TYPEWRITER, 1000, NULL },
    { "", STYLE_NORMAL, 500, NULL },
    { "크아아아아...!!!", STYLE_SHAKE, 200, NULL }, // Removed sound string to prevent texture error
    { "역장은 이미 좀비가 되어버렸다!", STYLE_FLASH, 500, NULL },
    { "쓰러뜨려서 안식을 줘야 한다!", STYLE_TYPEWRITER, 0, NULL }
};

static const Cinematic BOSS_INTRO_CINEMATIC = {
    NULL,
    BOSS_INTRO_LINES,
    sizeof(BOSS_INTRO_LINES) / sizeof(BOSS_INTRO_LINES[0]),
    30, 0, COLOR_RED, COLOR_WHITE
};

// ============================================
// Stage 1 Area Triggers
// ============================================
static const AreaTrigger AREA_TRIGGERS[] = {
    { 0, 46, 13, "mission_complete", "boss_spawned", &BOSS_INTRO_CINEMATIC, 1, "좀비가 된 역장을 쓰러뜨려라!" }
};

// ============================================
// Stage Package
// ============================================
const StageData STAGE_01_DATA = {
    .stageId = 1,
    .floors = FLOORS,
    .floor_count = sizeof(FLOORS) / sizeof(FLOORS[0]),
    // Shared Data
    .chests = CHESTS,
    .chestCount = sizeof(CHESTS) / sizeof(CHESTS[0]),
    .enemies = ENEMIES,
    .enemyCount = sizeof(ENEMIES) / sizeof(ENEMIES[0]),
    .npcs = NPCS,
    .npcCount = sizeof(NPCS) / sizeof(NPCS[0]),
    .doors = DOORS,
    .doorCount = sizeof(DOORS) / sizeof(DOORS[0]),
    
    // ★ Door Events
    .doorEvents = DOOR_EVENTS,
    .doorEventCount = sizeof(DOOR_EVENTS) / sizeof(DOOR_EVENTS[0]),
    
    // ★ Map Events
    .events = MAP_EVENTS,
    .eventCount = sizeof(MAP_EVENTS) / sizeof(MAP_EVENTS[0]),
    
    // ★ Area Triggers
    .triggers = AREA_TRIGGERS,
    .triggerCount = sizeof(AREA_TRIGGERS) / sizeof(AREA_TRIGGERS[0]),
    
    .quests = QUESTS,
    .questCount = sizeof(QUESTS) / sizeof(QUESTS[0]),
    
    .intro = &INTRO_CINEMATIC,
    .outro = &CLEAR_CINEMATIC,
    
    // Deprecated
    .overrides = NULL,
    .overrideCount = 0
};
