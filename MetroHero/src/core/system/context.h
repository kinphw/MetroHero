#ifndef CONTEXT_H
#define CONTEXT_H

#include "../../world/map.h"
#include "../../entity/player.h"

// 게임의 전체 상태를 담는 구조체
typedef struct {
    Map map;
    Player player;
    
    int isRunning;      // 게임 루프 실행 여부
    
    // 이동 관련
    int prevX;
    int prevY;

    // 대화 모드 관련
    int inDialogue;
    NPC* currentNPC;

    // ★ 이펙트 관련
    int effectX, effectY;
    float effectTimer;

    // ★ 적 이미지 표시 관련
    const char* enemyImagePath;  // 현재 표시할 적 이미지 경로
    double enemyImageTimer;      // 이미지 표시 시간 (5초)

    // ★ 사망 처리 관련
    int isPlayerDead;            // 플레이어 사망 상태 (엔터 대기 중)

} GameState;

#endif
