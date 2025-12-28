// core/combat.h
#ifndef COMBAT_H
#define COMBAT_H

#include "../../entity/player.h"
#include "../../entity/enemy.h"
#include "../../world/map.h"

struct GameState; // Forward decl

// 실시간 공격 시도 (Space 키)
void combat_try_attack(struct GameState* state);

// AI 업데이트 (매 프레임)
void combat_update_ai(struct GameState* state, float dt);

#endif