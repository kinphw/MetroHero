#pragma once
// core/combat.h
#ifndef COMBAT_H
#define COMBAT_H

#include "../entity/player.h"
#include "../entity/enemy.h"
#include "../world/map.h"

// 인접 적 체크 및 메시지 생성
void combat_check_nearby_enemy(Map* m , Player* p);

// 전투 시작 (나중에 구현)
void combat_attack_enemy(Player* p , Enemy* e);

#endif