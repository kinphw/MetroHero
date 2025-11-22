// core/combat.c
#include <stdio.h>
#include <windows.h>  // Sleep 사용

#include "combat.h"
#include "ui.h"
#include "../world/map.h"
#include "../world/glyph.h"  // ★ 추가


// ★ 인접 적 체크 및 메시지 자동 생성 (체력 포함)
void combat_check_nearby_enemy(Map* m, Player* p) {
    Enemy* nearEnemy = map_get_adjacent_enemy(m, p->x, p->y);

    if (nearEnemy != NULL) {
        const char* direction = map_get_enemy_direction(m, p->x, p->y, nearEnemy);
        char logMsg[256];
        // ★ 청록색으로 강조
        snprintf(logMsg, sizeof(logMsg),
            COMBAT_DETECT "%s에 %s이(가) 서있다! (HP: %d/%d)" COLOR_RESET,
            direction, nearEnemy->name, nearEnemy->hp, nearEnemy->maxHp);
        ui_add_log(logMsg);
    }
}

// ★ 전투 시스템
void combat_attack_enemy(Player* p, Enemy* e, Map* m) {
    char logMsg[128];

    // ★ 전투 이펙트 표시
    ui_draw_combat_effect(107, 2);  // 상태창 오른쪽

    // 플레이어 공격
    int playerDamage = p->attack - e->defense;
    if (playerDamage < 1) playerDamage = 1;  // 최소 1 데미지

    e->hp -= playerDamage;

    // ★ 밝은 빨간색으로 강조
    snprintf(logMsg, sizeof(logMsg),
        COMBAT_ATTACK "⚔ %s를 공격! " COMBAT_DAMAGE "%d 데미지!" COLOR_RESET " (HP: %d/%d)",
        e->name, playerDamage, e->hp > 0 ? e->hp : 0, e->maxHp);
    ui_add_log(logMsg);

    // ★ 잠깐 대기 (타격감)
    //Sleep(100);

    // 적 사망 체크
    if (e->hp <= 0) {
        e->isAlive = 0;
        // ★ 보라색으로 강조
        snprintf(logMsg, sizeof(logMsg),
            COMBAT_DEATH "★ %s를 처치했다! ★" COLOR_RESET, e->name);
        ui_add_log(logMsg);

        // ★ 이펙트 제거
        ui_clear_combat_effect(107, 2);
        return;
    }

    // ★ 적 반격 전 이펙트 변경
    //ui_clear_combat_effect(107, 2);

    // 적 반격 이펙트 (방패)
    //console_goto(107, 2);
    //printf(COLOR_BRIGHT_BLUE "  🛡" COLOR_RESET);
    //console_goto(107, 3);
    //printf(COLOR_BRIGHT_BLUE " 🛡🛡🛡" COLOR_RESET);
    //console_goto(107, 4);
    //printf(COLOR_BRIGHT_BLUE "🛡🛡🛡🛡🛡" COLOR_RESET);

    // 적 반격
    int enemyDamage = e->attack - p->defense;
    if (enemyDamage < 1) enemyDamage = 1;  // 최소 1 데미지

    p->hp -= enemyDamage;

    // ★ 빨간색으로 강조
    snprintf(logMsg, sizeof(logMsg),
        COMBAT_ENEMY "☠ %s의 반격! " COMBAT_DAMAGE "%d 데미지!" COLOR_RESET " (플레이어 HP: %d/%d)",
        e->name, enemyDamage, p->hp > 0 ? p->hp : 0, p->maxHp);
    ui_add_log(logMsg);

    //Sleep(100);

    // ★ 이펙트 제거
    //ui_clear_combat_effect(107, 2);

    // 플레이어 사망 체크
    if (p->hp <= 0) {
        ui_add_log("당신은 쓰러졌습니다...");
    }
}