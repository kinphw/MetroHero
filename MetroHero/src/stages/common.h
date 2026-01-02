#ifndef STAGE_COMMON_H_V2
#define STAGE_COMMON_H_V2

#include "../cinematic/cinematic.h"

#define BRANCH_SYSTEM_V1 // Diagnostics


// ============================================
// 공통 데이터 구조체 정의
// ============================================

// --- Tile Definition (Global Palette) ---
typedef struct {
    char symbol;          // 맵 문자 (예: '#')
    // const char* glyph; // Removed (Legacy)
    const char* imagePath;// 이미지 경로 (예: "assets/wall.png") - Raylib용
    int walkable;         // 1 = 이동 가능, 0 = 이동 불가
    const char* desc;     // 설명 (디버깅용)
} TileDef;

// --- Event Configuration ---
typedef struct {
    const char* reqFlag;      // 요구 플래그 (NULL이면 조건 없음)
    int reqVal;               // 요구 값 (보통 1)
    
    // ★ Key Item Config
    const char* reqItem;      // 요구 아이템 이름
    int consumeItem;          // 1: 사용 시 아이템 소모, 0: 유지
    
    const char* setFlag;      // 완료 후 설정/증가할 플래그 (NULL이면 없음)
    int setVal;               // 설정 값 (0이면 +1 증가)
    
    // ★ Reward Config
    const char* giveItem;     // 완료 후 지급할 아이템 이름

    const char* failMsg;      // 조건 불충족 시 메시지
} EventConfig;

// --- Chest Definition ---
typedef struct {
    char tile;              // '0', '1', '2' ... (맵상의 숫자)
    const char* itemType;   // "weapon", "armor", "item"
    const char* itemName;   // 아이템 이름

    const char* imagePath;  // ★ 추가 (필요하다면)
    
    EventConfig event;      // ★ 이벤트 설정 (요구 조건 등)
} ChestConfig;

// ★ Flexible Animation Mapping
typedef struct { int row; int col; } SpriteCoord;

// --- Enemy Definition ---
typedef struct {
    char tile;              // 'a', 'b'... (맵상의 문자)
    const char* name;       // 몬스터 이름
    const char* glyph;      // e.g. "🐈"
    const char* imagePath;  // 맵 타일 이미지
    const char* portraitPath; // ★ 전투 박스 초상화 이미지
    
    // ★ Multi-Tile & Sprite Sheet
    int width, height; // Size in tiles (default 1x1)
    
    // Animation/SpriteSheet Config
    int spriteRows; // if > 1, treat imagePath as sheet
    int spriteCols; // if > 1, treat imagePath as sheet
    
    // ★ Flexible Animation Mapping (직관적인 좌표 지정)
    // 예: .animDown = {0, 0}, .animUp = {0, 1} ...
    SpriteCoord animRight; // 0: Right
    SpriteCoord animLeft;  // 1: Left
    SpriteCoord animUp;    // 2: Up
    SpriteCoord animDown;  // 3: Down
    
    int maxHp;
    int attackMin;
    int attackMax;
    int expReward; // ★ Added


    // ★ AI 설정
    int chaseOnSight;    // 1: 감지 시 즉시 추격
    int attackOnSight;   // 1: 인접 시 선제 공격
    int detectionRange;  // 감지 범위
    float moveInterval;  // 이동 속도 (초 단위) (추격 시)
    float attackInterval;// 공격 속도 (초 단위)
    
    // ★ Random Idle Movement
    int allowRandomMove;       // 1: 비전투/비추격 시 랜덤 이동
    float randomMoveInterval;  // 랜덤 이동 주기
    
    // 대사 관련
    const char** dialogues;
    int dialogueCount;

    const char* dialogueColor; // 예: COLOR_RED (ANSI Code)
    
    // ★ Spawn Condition
    const char* reqFlag;       // 이 플래그가 있어야 등장 (없으면 처음에 숨김)
    
    EventConfig event;         // ★ 이벤트 설정 (사망 시 플래그 설정 등)
} EnemyConfig;

// ★ Dialogue Branch System
typedef struct {
    const char* reqFlag;      // 조건 플래그
    int reqVal;               // 조건 값
    
    int startIndex;           // 전체 대화 배열에서의 시작 인덱스
    int count;                // 사용할 대화 개수
    
    const EventConfig* event; // 이 브랜치 종료 시 실행할 이벤트 (Optional)
} DialogueBranch;

// --- NPC Definition ---
typedef struct {
    char tile;              // 'A', 'B', 'C'...
    const char* name;
    const char* glyph;      // e.g. GLYPH_NPC_MERCHANT
    const char* imagePath;  // ★ 추가
    const char* faceImagePath; // ★ 얼굴 이미지 경로 추가 (128x128)
    
    const char** dialogues; // ★ All Dialogues Combined
    int dialogueCount;      // Total Count
    
    // ★ Branches (Logic)
    const DialogueBranch* branches;
    int branchCount;
    
    int canTrade;
    const char* shopType;

    int useDialogueBox;
    
    EventConfig event;         // ★ Default Event (Deprecated/Legacy Fallback)
} NPCConfig;

// --- Door Configuration (New) ---
typedef struct {
    char symbol;            // 문 문자 (예: '$', 'D') - '%'는 기본값
    EventConfig event;      // 잠금 조건 등
} DoorConfig;

// ★ Quest Config
typedef struct {
    const char* reqFlag;    // 발동 조건 플래그
    int reqVal;             // 발동 값 (이상일 때)
    const char* msg;        // 출력할 메시지 (파란색 자동 적용)
    
    // ★ Counter Support
    const char* counterFlag; // 카운팅할 플래그 (없으면 NULL)
    int counterMax;          // 목표치 (0이면 카운터 없음)
    
    // ★ Completion Condition (To hide from list)
    const char* endFlag;     // 이 플래그가 설정되면 퀘스트 목록에서 사라짐
    int endVal;              // 이 값 이상이면 종료 (기본 1)
    
    int expReward;          // ★ Added
} QuestConfig;

// ★ Dialogue Override Config
typedef struct {
    char npcTile;
    const char* reqFlag;
    int reqVal;
    
    const char** newDialogues;
    int newDialogueCount;
    const EventConfig* newEvent; 
} DialogueOverride;

// ★ Warp Configuration (Teleport)
typedef struct {
    char triggerSymbol;      // 현재 맵에서 밟는 타일 (예: '1')
    int targetFloorIdx;      // 이동할 층 인덱스 (0-based)
    char targetSymbol;       // 이동 후 도착할 타일 (예: 'A')
} WarpConfig;

// ★ Sub-Map Configuration (Floor)
typedef struct {
    const char** mapLines;
    int mapHeight;
    
    // Warps for this floor
    const WarpConfig* warps;
    int warpCount;
} SubMapConfig;

// ★ Coordinate-Based Map Event (For Signposts etc.)
typedef struct {
    int floorIndex; // 0-based
    int x;
    int y;
    const char* msg;
} MapEvent;

// ★ Coordinate-Based Door Event
typedef struct {
    int floorIndex;
    int x; 
    int y;
    
    const char* reqKey;   // Item Name required
    int consumeKey;       // 1: Consume, 0: Keep
    const char* reqFlag;  // Flag required
    
    const char* failMsg;
    const char* successMsg; // If NULL, use default "Opened"
} DoorEvent;

// ★ Area Trigger (Cinematic/Event on Step)
typedef struct {
    int floorIndex;
    int x;
    int y;
    
    const char* reqFlag;    // Active Condition (e.g. "mission_complete")
    const char* setFlag;    // Set this flag on trigger (e.g. "boss_spawned")
    const Cinematic* cinematic; // Optional Cinematic to play
    int oneShot;            // 1: Run once (uses setFlag to check completion)
    
    // Optional: Update Quest Message
    const char* updateQuestMsg;
} AreaTrigger;

// --- Stage Definition ---
// --- Stage Definition ---
typedef struct {
    int stageId;
    
    // ★ Area Triggers (Zone Events) - MOVED TO TOP for Visibility Check
    const AreaTrigger* triggers;
    int triggerCount;
    
    // ★ Multi-Floor Support
    const SubMapConfig* floors;
    int floor_count;

    // 상자 데이터 (Stage Global)
    const ChestConfig* chests;
    int chestCount;

    // 적 데이터 (Stage Global)
    const EnemyConfig* enemies;
    int enemyCount;
    
    // NPC 데이터 (Stage Global)
    const NPCConfig* npcs;
    int npcCount;
    
    // 문 데이터 (Stage Global) -- DEPRECATED
    const DoorConfig* doors;
    int doorCount;
    
    // ★ Door Events (Coordinate Based)
    const DoorEvent* doorEvents;
    int doorEventCount;
    
    // ★ Map Events (Signposts)
    const MapEvent* events;
    int eventCount;
    
    // 퀘스트 데이터
    const QuestConfig* quests;
    int questCount;
    
    // ★ 대화 오버라이드 데이터
    const DialogueOverride* overrides;
    int overrideCount;

    // 시네마틱 (스토리)
    const Cinematic* intro;       // 스테이지 시작 시 재생
    const Cinematic* outro;       // 스테이지 클리어 시 재생
} StageData;

#endif
