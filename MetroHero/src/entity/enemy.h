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
	int maxHp;
	int hp;
	//int attack;
	int attackMin;
	int attackMax;
	int defense;

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
