// core/combat.c
#include "combat.h"
#include "ui.h"
#include "../world/map.h"
#include <stdio.h>

// ★ 인접 적 체크 및 메시지 자동 생성
void combat_check_nearby_enemy(Map* m, Player* p) {
    Enemy* nearEnemy = map_get_adjacent_enemy(m, p->x, p->y);

    if (nearEnemy != NULL) {
        const char* direction = map_get_enemy_direction(m, p->x, p->y, nearEnemy);
        char logMsg[128];
        snprintf(logMsg, sizeof(logMsg), "%s에 %s이(가) 서있다!", direction, nearEnemy->name);
        ui_add_log(logMsg);
    }
}

// ★ 나중에 전투 시스템 추가
void combat_attack_enemy(Player* p, Enemy* e) {
    // 데미지 계산
    int damage = p->attack - e->defense;
    if (damage < 0) damage = 0;

    e->hp -= damage;

    // 전투 로그
    char logMsg[128];
    snprintf(logMsg, sizeof(logMsg), "%s를 공격! %d 데미지!", e->name, damage);
    ui_add_log(logMsg);

    if (e->hp <= 0) {
        e->isAlive = 0;
        snprintf(logMsg, sizeof(logMsg), "%s를 처치했다!", e->name);
        ui_add_log(logMsg);
    }
    else {
        // 적 반격
        int enemyDamage = e->attack - p->defense;
        if (enemyDamage < 0) enemyDamage = 0;

        p->hp -= enemyDamage;
        snprintf(logMsg, sizeof(logMsg), "%s의 반격! %d 데미지!", e->name, enemyDamage);
        ui_add_log(logMsg);
    }
}