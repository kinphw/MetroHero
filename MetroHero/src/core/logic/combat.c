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
#include "raylib.h"

// ... (rest of code logic is same, commented out Sleep calls are fine to remain commented or be removed)
#include "../ui/ui.h"
#include "../../world/map.h"
#include "../../world/glyph.h"  // ★ 추가

// ★ 적 이미지 표시 헬퍼 함수 (5초 타이머)
static void show_enemy_image(GameState* state, const Enemy* enemy) {
    if (enemy && enemy->portraitPath) {
        state->enemyImagePath = enemy->portraitPath; // ★ 초상화 이미지 사용
        state->enemyImageTimer = GetTime() + 5.0;    // 현재 시간 + 5초
    }
}


// ★ 인접 적 체크 및 메시지 (체력 + 순환 대사 포함)
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
        // ★ 플레이어가 적 공격 -> 적 이미지 표시
        show_enemy_image(state, target);

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
            // ★ 적 처치 -> 이미지 유지 (이미 표시됨)
            // 맵 타일 정리 (적 제거) - 추후 자동 clean up
             snprintf(buf, sizeof(buf), "★ %s 처치!", target->name);
             ui_add_combat_log(buf);
        }
    } else {
        // 허공에 공격
        // ui_add_combat_log("허공을 가랐다.");
    }
}

// ★ AI 업데이트 Implementation
// ★ AI 업데이트 Implementation
void combat_update_ai(GameState* state, float dt) {
    Map* m = &state->map;
    Player* p = &state->player;

    for (int i = 0; i < m->enemyCount; i++) {
        Enemy* e = &m->enemies[i];
        if (!e->isAlive) continue;

        // 쿨타임 감소 (개별)
        if (e->moveCooldown > 0)    e->moveCooldown -= dt;
        if (e->attackCooldown > 0)  e->attackCooldown -= dt;

        // 거리 계산 (Manhattan Distance)
        int dx = p->x - e->x;
        int dy = p->y - e->y;
        int dist = abs(dx) + abs(dy);
        
        // ★ 인식 범위 체크 및 로그 (Alert Logic)
        if (dist <= e->detectionRange) {
            if (!e->isChasing) { // Not alerted yet
                e->isChasing = 1; // Mark as alerted

                // ★ 적 발견 -> 이미지 표시
                show_enemy_image(state, e);

                // 인식 로그 출력 (좌측 일반 로그)
                char buf[128];
                if (e->chaseOnSight) {
                    snprintf(buf, sizeof(buf), "%s이(가) 당신을 발견했습니다, 당신에게 접근합니다!", e->name);
                } else {
                    snprintf(buf, sizeof(buf), "%s이(가) 당신을 발견했습니다!", e->name);
                }
                ui_add_log(buf);
            }
        } else {
            // 범위 밖으로 나가면 인식(Alert) 해제
            e->isChasing = 0;
        }

        // 행동 결정 (Alerted 상태일 때만 or 항상? 보통 인식해야 행동)
        if (e->isChasing) {
            // 1. 공격 (인접 + 선공O + 쿨타임)
            if (dist <= 1 && e->attackOnSight) {
                if (e->attackCooldown <= 0) {
                    e->attackCooldown = e->attackInterval; // 쿨타임 리셋

                    // ★ 적이 플레이어 공격 -> 이미지 표시
                    show_enemy_image(state, e);

                    // 데미지 계산
                    int dmg = e->attackMin + rand() % (e->attackMax - e->attackMin + 1);
                    dmg -= p->defense;
                    if (dmg < 1) dmg = 1;

                    p->hp -= dmg;

                    char buf[128];
                    snprintf(buf, sizeof(buf), "☠ %s의 공격! %d 피해 (HP: %d)", e->name, dmg, p->hp);
                    ui_add_combat_log(buf);

                    if (p->hp <= 0) {
                        ui_add_log("당신은 쓰러졌습니다...");
                    }
                }
            }
            // 2. 추적 (거리>1 + 선추격O + 쿨타임)
            else if (dist > 1 && e->chaseOnSight) {
                if (e->moveCooldown <= 0) {
                     e->moveCooldown = e->moveInterval; // 쿨타임 리셋
            
                    int nextX = e->x;
                    int nextY = e->y;

                    // X축, Y축 중 더 먼 쪽을 좁힘 (단순 추적)
                    if (abs(dx) > abs(dy)) {
                        nextX += (dx > 0) ? 1 : -1;
                    } else {
                        nextY += (dy > 0) ? 1 : -1;
                    }

                    // 이동 가능 여부 확인
                    if (map_is_walkable(m, nextX, nextY) && !map_get_enemy_at(m, nextX, nextY)) {
                        e->x = nextX;
                        e->y = nextY;
                    }
                }
            }
        }
    }
}