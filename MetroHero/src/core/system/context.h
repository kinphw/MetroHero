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

} GameState;

#endif
