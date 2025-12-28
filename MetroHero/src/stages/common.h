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

// --- Chest Definition ---
typedef struct {
    char tile;              // '0', '1', '2' ... (맵상의 숫자)
    const char* itemType;   // "weapon", "armor", "item"
    const char* itemName;   // 아이템 이름
    const char* imagePath;  // ★ 추가 (필요하다면)
} ChestConfig;

// --- Enemy Definition ---
typedef struct {
    char tile;              // 'a', 'b'... (맵상의 문자)
    const char* name;       // 몬스터 이름
    const char* glyph;      // e.g. "🐈"
    const char* imagePath;  // ★ 추가
    int maxHp;
    int attackMin;
    int attackMax;
    int defense;

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
} NPCConfig;

// --- Stage Definition ---
typedef struct {
    int stageId;
    
    // 맵 데이터
    const char** mapLines;
    int mapHeight;
    // width는 strlen(mapLines[0])으로 계산 가능

    // 상자 데이터
    const ChestConfig* chests;
    int chestCount;

    // 적 데이터
    const EnemyConfig* enemies;
    int enemyCount;

    // NPC 데이터
    const NPCConfig* npcs;
    int npcCount;

    // 시네마틱 (스토리)
    const Cinematic* intro;       // 스테이지 시작 시 재생
    const Cinematic* outro;       // 스테이지 클리어 시 재생
} StageData;

#endif
