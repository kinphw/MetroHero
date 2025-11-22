// core/combat.c
#include "combat.h"
#include "ui.h"
#include "../world/map.h"
#include <stdio.h>

// ★ 인접 적 체크 및 메시지 자동 생성 (체력 포함)
void combat_check_nearby_enemy(Map* m, Player* p) {
    Enemy* nearEnemy = map_get_adjacent_enemy(m, p->x, p->y);

    if (nearEnemy != NULL) {
        const char* direction = map_get_enemy_direction(m, p->x, p->y, nearEnemy);
        char logMsg[128];
        snprintf(logMsg, sizeof(logMsg), "%s에 %s이(가) 서있다! (HP: %d/%d)",
            direction, nearEnemy->name, nearEnemy->hp, nearEnemy->maxHp);
        ui_add_log(logMsg);
    }
}

// ★ 전투 시스템
void combat_attack_enemy(Player* p, Enemy* e, Map* m) {
    char logMsg[128];

    // 플레이어 공격
    int playerDamage = p->attack - e->defense;
    if (playerDamage < 1) playerDamage = 1;  // 최소 1 데미지

    e->hp -= playerDamage;

    snprintf(logMsg, sizeof(logMsg), "%s를 공격! %d 데미지! (HP: %d/%d)",
        e->name, playerDamage, e->hp > 0 ? e->hp : 0, e->maxHp);
    ui_add_log(logMsg);

    // 적 사망 체크
    if (e->hp <= 0) {
        e->isAlive = 0;
        snprintf(logMsg, sizeof(logMsg), "%s를 처치했다!", e->name);
        ui_add_log(logMsg);
        return;  // 적이 죽으면 반격 없음
    }

    // 적 반격
    int enemyDamage = e->attack - p->defense;
    if (enemyDamage < 1) enemyDamage = 1;  // 최소 1 데미지

    p->hp -= enemyDamage;

    snprintf(logMsg, sizeof(logMsg), "%s의 반격! %d 데미지! (플레이어 HP: %d/%d)",
        e->name, enemyDamage, p->hp > 0 ? p->hp : 0, p->maxHp);
    ui_add_log(logMsg);

    // 플레이어 사망 체크
    if (p->hp <= 0) {
        ui_add_log("당신은 쓰러졌습니다...");
    }
}