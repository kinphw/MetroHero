#pragma once
// src/entity/npc.h - 새 파일 생성

#ifndef NPC_H
#define NPC_H

#include "../stages/common.h"

#define MAX_NPCS 20
#define MAX_NPC_DIALOGUES 10

#include "../stages/common.h" // For EventConfig and NPCConfig

typedef struct {
	int x, y;
	char tile;              // 'A', 'B'...
	const char* name;       // "상인", "경비원" 등
	const char* glyph;      // 렌더링용 문자
    const char* imagePath;  // ★ Add Raylib Image Path
    const char* faceImagePath; // ★ Added

	// 대화 시스템
	const char** dialogues;
	int dialogueCount;
	int currentDialogue;    // 현재 대화 인덱스
    
	// ★ 대화 모드 추가
	int useDialogueBox;    // 1 = 전용 대화창 사용, 0 = 로그창만

	// 향후 확장용
	int canTrade;          // 거래 가능 여부
	const char* shopType;  // "weapon", "armor", "item" 등
    
    EventConfig event; // Added
    
    // ★ Active Dialogue (Updated dynamically)
    const char** activeDialogues;
    int activeDialogueCount;
} NPC;
#include "../core/logic/event.h"

// 초기화
void npc_init(NPC* npc, const NPCConfig* config, int x, int y);
int npc_is_at(const NPC* npc , int x , int y);

// Updates activeDialogues based on flags via Stage Overrides
void npc_update_dialogue_state(NPC* npc, const EventRegistry* events, const StageData* stageData); // Updated

const char* npc_get_dialogue(const NPC* npc); // Restored
void npc_next_dialogue(NPC* npc);

#endif