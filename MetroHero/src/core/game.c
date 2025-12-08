#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "system/game_internal.h"
#include "ui/ui.h"
#include "../cinematic/cinematic.h"
#include "../world/map.h"
#include "../entity/player.h"

// 게임 실행 (진입점)
void game_run(void) {
    // 1. 상태 초기화
    GameState state = { 0 };  // 모든 필드 0/NULL 초기화

    ui_init();

    // 인트로 시네마틱
    cinematic_play_intro();

    map_init(&state.map, 1);
    player_init(&state.player);

    // 스테이지 1 시작
    cinematic_play_stage_start(1);

    // 플레이어 배치
    state.player.x = state.map.spawnX;
    state.player.y = state.map.spawnY;
    
    // 초기 뷰포트 상태 설정 (강제 렌더링을 위해 다르게 설정)
    state.prevX = -1;
    state.prevY = -1;
    state.isRunning = 1;
    state.inDialogue = 0;
    state.currentNPC = NULL;

    // 초기 화면 그리기 (렌더링 모듈에서 일부 수행하지만, 전체 클리어는 여기서 하는게 깔끔할 수 있음)
    ui_clear_buffer();
    
    // 뷰포트, UI 초기화 그리기
    game_render(&state);

    // 입력 안내
    ui_draw_str_at(0, SCREEN_H - 1, "[화살표/WASD] 이동 | [0] 상호작용 | [Q] 종료", NULL);
    ui_present();

    // 2. 메인 루프 실행
    game_loop(&state);
}
