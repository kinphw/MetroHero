#ifndef STAGE_COMMON_H
#define STAGE_COMMON_H

#include "../cinematic/cinematic.h"

// ============================================
// 공통 데이터 구조체 정의
// ============================================

// --- Tile Definition (Global Palette) ---
typedef struct {
    char symbol;          // 맵 문자 (예: '#')
    const char* glyph;    // 글리프 문자열 (예: GLYPH_WALL_GRAY) - Debug/Legacy용
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
    
    int maxHp;
    int attackMin;
    int attackMax;


    // ★ AI 설정
    int chaseOnSight;    // 1: 감지 시 즉시 추격
    int attackOnSight;   // 1: 인접 시 선제 공격
    int detectionRange;  // 감지 범위
    float moveInterval;  // 이동 속도 (초 단위)
    float attackInterval;// 공격 속도 (초 단위)
    
    // 대사 관련
    const char** dialogues;
    int dialogueCount;

    const char* dialogueColor; // 예: COLOR_RED (ANSI Code)
    
    EventConfig event;         // ★ 이벤트 설정 (사망 시 플래그 설정 등)
} EnemyConfig;

// --- NPC Definition ---
typedef struct {
    char tile;              // 'A', 'B', 'C'...
    const char* name;
    const char* glyph;      // e.g. GLYPH_NPC_MERCHANT
    const char* imagePath;  // ★ 추가
    const char* faceImagePath; // ★ 얼굴 이미지 경로 추가 (128x128)
    
    const char** dialogues;
    int dialogueCount;
    
    int canTrade;
    const char* shopType;

    int useDialogueBox;
    
    EventConfig event;         // ★ 이벤트 설정 (대화 조건/결과)
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
} QuestConfig;

// ★ Dialogue Override Config (Decoupled from NPCConfig)
typedef struct {
    char npcTile;           // 대상 NPC 타일 문자 (예: 'A')
    const char* reqFlag;    // 발동 조건 플래그
    int reqVal;             // 발동 값
    
    const char** newDialogues; // 교체할 대화 목록
    int newDialogueCount;
    
    // Optional: New Event? (If talk reward also changes?)
    // For now, only Dialogue changes. Reward is usually one-off via event.
} DialogueOverride;

// --- Stage Definition ---
typedef struct {
    int stageId;
    
    // 맵 데이터
    const char** mapLines;
    int mapHeight;

    // 상자 데이터
    const ChestConfig* chests;
    int chestCount;

    // 적 데이터
    const EnemyConfig* enemies;
    int enemyCount;

    // NPC 데이터
    const NPCConfig* npcs;
    int npcCount;

    // 문 데이터 (이벤트 문)
    const DoorConfig* doors;
    int doorCount;

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
