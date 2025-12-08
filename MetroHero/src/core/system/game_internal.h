#ifndef GAME_INTERNAL_H
#define GAME_INTERNAL_H

#include "context.h"

// 입력 처리
void game_process_input(GameState* state);

// 화면 렌더링
void game_render(GameState* state);

// 메인 루프
void game_loop(GameState* state);

#endif
