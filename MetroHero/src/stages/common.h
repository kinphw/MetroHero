#ifndef STAGE_COMMON_H
#define STAGE_COMMON_H

#include "../cinematic/cinematic.h"

// ============================================
// 공통 데이터 구조체 정의
// ============================================

// --- Chest Definition ---
typedef struct {
    char tile;              // '0', '1', '2' ... (맵상의 숫자)
    const char* itemType;   // "weapon", "armor", "item"
    const char* itemName;   // 아이템 이름
} ChestConfig;

// --- Enemy Definition ---
typedef struct {
    char tile;              // 'a', 'b'... (맵상의 문자)
    const char* name;       // 몬스터 이름
    int maxHp;
    int attackMin;
    int attackMax;
    int defense;
    
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
