#ifndef ENEMY_H
#define ENEMY_H

#include "../stages/common.h"

#define MAX_ENEMIES 50  // 맵당 최대 적 수

typedef struct {
	char type;      // 종류 ('a': 고블린, 'b': 슬라임, ...)
	int x, y;       // 위치
	int isAlive;

	// 스탯
	const char* name;
    const char* glyph;
    const char* imagePath; // ★ Add Raylib Image Path
	int maxHp;
	int hp;
	//int attack;
	int attackMin;
	int attackMax;
	int defense;

    // ★ AI 관련
    float actionCooldown;   // 행동 쿨타임 (0이 되면 행동 가능)
    float actionInterval;   // 행동 간격 (초 단위, 예: 1.0f)
    int detectionRange;     // 감지 범위 (칸 단위)
    int isChasing;          // 추격 중 여부 (로그 출력용)

	// 대사
	const char** dialogues; // 문자열 배열 포인터
	int dialogueCount;
	int dialogueIndex;      // 현재 대사 인덱스
	
    // 렌더링
    const char* dialogueColor; // 색상 코드 추가

} Enemy;

// 초기화
void enemy_init(Enemy* e, const EnemyConfig* config, int x, int y);

int enemy_is_at(const Enemy* e, int x, int y);

#endif
#pragma once
