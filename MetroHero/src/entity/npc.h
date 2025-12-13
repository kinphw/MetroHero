#pragma once
// src/entity/npc.h - 새 파일 생성

#ifndef NPC_H
#define NPC_H

#include "../stages/common.h"

#define MAX_NPCS 20
#define MAX_NPC_DIALOGUES 10

typedef struct {
	int x , y;
	char type;              // 'A', 'B', 'C' 등
	const char* name;       // "상인", "경비원" 등
	const char* glyph;      // 렌더링용 문자

	// 대화 시스템
	const char** dialogues;
	int dialogueCount;
	int currentDialogue;    // 현재 대화 인덱스

	// ★ 대화 모드 추가
	int useDialogueBox;    // 1 = 전용 대화창 사용, 0 = 로그창만

	// 향후 확장용
	int canTrade;          // 거래 가능 여부
	const char* shopType;  // "weapon", "armor", "item" 등
} NPC;
// 초기화
void npc_init(NPC* npc, const NPCConfig* config, int x, int y);
int npc_is_at(const NPC* npc , int x , int y);
const char* npc_get_dialogue(NPC* npc);
void npc_next_dialogue(NPC* npc);

#endif