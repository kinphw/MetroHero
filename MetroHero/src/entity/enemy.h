#ifndef ENEMY_H
#define ENEMY_H

#define MAX_ENEMIES 50  // 맵당 최대 적 수

typedef struct {
	int x , y;           // 위치
	char type;          // 'a' = 고블린, 'b' = 오크 등
	int hp;
	int maxHp;
	//int attack;

	// ★ 공격력을 범위로 변경
	int attackMin;
	int attackMax;

	int defense;
	int isAlive;        // 1 = 살아있음, 0 = 죽음
	const char* name;   // "고블린", "오크" 등
	const char** dialogues;  // ★ 대사 배열 (문자열 배열)
	int dialogueCount;       // ★ 총 몇 개?
	int dialogueIndex;       // ★ 현재 어떤 대사를 출력할지
	const char* dialogueColor;   // ★ 추가: 대사 색깔
} Enemy;

// 적 초기화 (타입에 따라 스탯 설정)
void enemy_init(Enemy* e , char type , int x , int y);

// 적이 해당 위치에 있는지 확인
int enemy_is_at(const Enemy* e , int x , int y);

#endif#pragma once
