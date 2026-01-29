#ifndef ENEMY_H
#define ENEMY_H

#include "../stages/common.h"
#include "../core/graphics/sprite_sheet.h" // Added

#define MAX_ENEMIES 50  // 맵당 최대 적 수

typedef struct {
	char type;      // 종류 ('a': 고블린, 'b': 슬라임, ...)
	int x, y;       // 위치
	int isAlive; // Pool Active State
    int isDead;  // Gameplay Dead State (HP <= 0)

	// 스탯
	const char* name;
    const char* glyph;
    const char* imagePath;     // 맵 타일 이미지 (예: assets/cat.png)
    const char* portraitPath;  // ★ 전투 박스 초상화 이미지 (예: assets/enemy/1a.png)
    
    // ★ Multi-Tile & Sprite Sheet Support
    int width, height; 
    SpriteSheet spriteSheet; // Runtime Sprite Sheet
    int direction; // 0: Right, 1: Left, 2: Up, 3: Down
    
    // ★ Animation Coords (Copied from Config)
    struct { int row; int col; } animRight, animLeft, animUp, animDown;
    
	int maxHp;
	int hp;
	//int attack;
    int attackMin;
	int attackMax;
    int expReward; // ★ Added


    // ★ AI 관련
    int chaseOnSight;
    int attackOnSight;
    int detectionRange;
    int isChasing;
    int isProvoked; // ★ 공격받음 상태


    float moveInterval;
    float attackInterval;
    float moveCooldown;
    float attackCooldown;
    
    // ★ Random Idle Move
    int allowRandomMove;
    float randomMoveInterval;
    float randomMoveTimer;

	// 대사
	const char** dialogues; // 문자열 배열 포인터
	int dialogueCount;
	int dialogueIndex;      // 현재 대사 인덱스
	
    // 렌더링
    const char* dialogueColor; // 색상 코드 추가
    
    // ★ Conditional Spawn
    const char* reqFlag; 
    int isActive;
    
    EventConfig event; // Added

} Enemy;

// 초기화
void enemy_init(Enemy* e, const EnemyConfig* config, int x, int y);

int enemy_is_at(const Enemy* e, int x, int y);

#endif
#pragma once
