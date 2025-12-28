// core/combat.h
#ifndef COMBAT_H
#define COMBAT_H

#include "../../entity/player.h"
#include "../../entity/enemy.h"
#include "../../world/map.h"

struct GameState; // Forward decl

// 인접 적 체크 및 메시지 생성
void combat_check_nearby_enemy(Map* m , Player* p);

// 전투 시작
void combat_attack_enemy(Player* p , Enemy* e , Map* m);

// 실시간 공격 시도 (Space 키)
void combat_try_attack(struct GameState* state);

// AI 업데이트 (매 프레임)
void combat_update_ai(struct GameState* state, float dt);

#endif