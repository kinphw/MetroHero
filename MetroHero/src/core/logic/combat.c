// core/combat.c
#include <stdio.h>
// #include <windows.h>  // Removed for Raylib

#include "combat.h"
#include "../ui/ui.h"
#include "../../world/map.h"
#include "../ui/ui.h"
#include "../../world/map.h"
#include "../../world/glyph.h" 
#include "../system/context.h" // ★ GameState 정의 필요 

// ... (rest of code logic is same, commented out Sleep calls are fine to remain commented or be removed)
#include "../ui/ui.h"
#include "../../world/map.h"
#include "../../world/glyph.h"  // ★ 추가


// ★ 인접 적 체크 및 메시지 (체력 + 순환 대사 포함)
void combat_check_nearby_enemy(Map* m, Player* p) {
    Enemy* e = map_get_adjacent_enemy(m, p->x, p->y);
    if (e == NULL) return;

    const char* direction = map_get_enemy_direction(m, p->x, p->y, e);

    // ★ 현재 대사를 꺼낸다
    const char* line = e->dialogues[e->dialogueIndex];

    // ★ 다음 접근 시 다음 대사 출력되도록 index 증가 (순환)
    e->dialogueIndex = (e->dialogueIndex + 1) % e->dialogueCount;

    char logMsg[512];
    snprintf(logMsg, sizeof(logMsg),
        "%s%s에 %s이(가) 서있다! (HP: %d/%d) %s「%s」%s",
        COMBAT_DETECT,
        direction,
        e->name,
        e->hp, e->maxHp,

        e->dialogueColor,   // ★ 대사 색깔
        line,                // ★ 대사 내용
        COLOR_RESET          // ★ 리셋을 맨 뒤에 강제 배치
    );

    ui_add_log(logMsg);
}

// ★ 전투 시스템
void combat_attack_enemy(Player* p, Enemy* e, Map* m) {
    char logMsg[128];

    // ★ 전투 이펙트 표시 (상태창 내부 오른쪽)
    ui_show_combat_effect();

    // ★★★ 플레이어 공격 - 랜덤 데미지 ★★★
    int playerAttack = p->attackMin + rand() % (p->attackMax - p->attackMin + 1);
    int playerDamage = playerAttack - e->defense;
    if (playerDamage < 1) playerDamage = 1;

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
        ui_hide_combat_effect();
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

    // ★★★ 적 반격 - 랜덤 데미지 ★★★
    int enemyAttack = e->attackMin + rand() % (e->attackMax - e->attackMin + 1);
    int enemyDamage = enemyAttack - p->defense;
    if (enemyDamage < 1) enemyDamage = 1;

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

// ★ 실시간 공격 시도
void combat_try_attack(GameState* state) {
    Player* p = &state->player;
    Map* m = &state->map;

    if (p->attackCooldown > 0) return; // 쿨타임 중

    p->attackCooldown = 0.5f; // 0.5초 쿨타임

    // 바라보는 방향 계산
    int tx = p->x + p->dirX;
    int ty = p->y + p->dirY;

    // ★ 이펙트 표시 (0.2초)
    state->effectX = tx;
    state->effectY = ty;
    state->effectTimer = 0.2f;

    Enemy* target = map_get_enemy_at(m, tx, ty);
    if (target) {
        // 데미지 계산
        int dmg = p->attackMin + rand() % (p->attackMax - p->attackMin + 1);
        dmg -= target->defense;
        if (dmg < 1) dmg = 1;

        target->hp -= dmg;

        char buf[128];
        snprintf(buf, sizeof(buf), "⚔ %s에게 %d 피해! (HP: %d)", target->name, dmg, target->hp);
        ui_add_combat_log(buf); // 우측 전투 로그에 출력

        if (target->hp <= 0) {
            target->isAlive = 0;
            // 맵 타일 정리 (적 제거) - 추후 자동 clean up
             snprintf(buf, sizeof(buf), "★ %s 처치!", target->name);
             ui_add_combat_log(buf);
        }
    } else {
        // 허공에 공격
        // ui_add_combat_log("허공을 가랐다.");
    }
}