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

        target->isProvoked = 1; // ★ 공격받으면 적대적 상태 전환
        
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
        // 시야 체크 추가: 거리가 가까워도 벽으로 막혀있으면 인식 불가 (단, 이미 Provoke 된 경우 무조건 인식)
        int hasLOS = map_check_los(m, e->x, e->y, p->x, p->y);
        
        if ((dist <= e->detectionRange && hasLOS) || e->isProvoked) {
            if (!e->isChasing) { // Not alerted yet
                e->isChasing = 1; // Mark as alerted

                // ★ 적 발견 -> 이미지 표시
                show_enemy_image(state, e);

                // 인식 로그 출력 (좌측 일반 로그)
                char buf[128];
                if (e->chaseOnSight || e->isProvoked) {
                    snprintf(buf, sizeof(buf), "%s이(가) 당신을 발견했습니다, 당신에게 접근합니다!", e->name);
                } else {
                    snprintf(buf, sizeof(buf), "%s이(가) 당신을 발견했습니다!", e->name);
                }
                ui_add_combat_log(buf);
            }
        } else {
            // 범위 밖으로 나가면 인식(Alert) 해제 (단, Provoked 상태면 계속 추격할 수도 있음 - 여기서는 해제)
            // User requirement: "All enemies must follow". So maybe don't lose aggro if provoked?
            // Let's keep it simple: if provoked, condition (dist <= detectionRange || isProvoked) is TRUE.
            // isProvoked is persistent until death.
            // So if provoked, we never enter this else block unless... wait.
            // If provoked, the IF condition is always TRUE. So we never clear isChasing.
            // Correct.
            e->isChasing = 0;
        }

        // 행동 결정 (Alerted 상태일 때만 or 항상? 보통 인식해야 행동)
        if (e->isChasing) {
            // 1. 공격 (인접 + 선공O + 쿨타임)
            if (dist <= 1 && (e->attackOnSight || e->isProvoked)) {
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

                    // ★ 사망 처리 (즉시 종료하지 않고 상태만 변경)
                    if (p->hp <= 0) {
                        state->isPlayerDead = 1;
                        ui_add_log(COLOR_BRIGHT_RED "★ 당신은 쓰러졌습니다..." COLOR_RESET);
                        ui_add_combat_log(COLOR_BRIGHT_RED "★ 당신은 쓰러졌습니다..." COLOR_RESET);
                        ui_add_combat_log(" ");
                        ui_add_combat_log(COLOR_BRIGHT_YELLOW "[ENTER]를 눌러 계속..." COLOR_RESET);
                    }
                }
            }
            // 2. 추적 (거리>1 + 선추격O + 쿨타임)
            else if (dist > 1 && (e->chaseOnSight || e->isProvoked)) {
                if (e->moveCooldown <= 0) {
                     e->moveCooldown = e->moveInterval; // 쿨타임 리셋
            
                    int tryX = e->x;
                    int tryY = e->y;
                    
                    int dirX = (dx > 0) ? 1 : -1;
                    int dirY = (dy > 0) ? 1 : -1;
                    
                    int moved = 0;

                    // 1. 주축(Primary Axis) 결정 및 이동 시도
                    if (abs(dx) >= abs(dy)) {
                        // X축 우선
                        if (dx != 0) {
                            int nx = e->x + dirX;
                            int ny = e->y;
                            if (map_is_walkable(m, nx, ny) && !map_get_enemy_at(m, nx, ny)) {
                                e->x = nx;
                                e->y = ny;
                                moved = 1;
                            }
                        }
                        // X축 실패 or 이동 안함 -> Y축 시도 (Fallback)
                        if (!moved && dy != 0) {
                            int nx = e->x;
                            int ny = e->y + dirY;
                            if (map_is_walkable(m, nx, ny) && !map_get_enemy_at(m, nx, ny)) {
                                e->x = nx;
                                e->y = ny;
                                moved = 1;
                            }
                        }
                    } else {
                        // Y축 우선
                        if (dy != 0) {
                            int nx = e->x;
                            int ny = e->y + dirY;
                            if (map_is_walkable(m, nx, ny) && !map_get_enemy_at(m, nx, ny)) {
                                e->x = nx;
                                e->y = ny;
                                moved = 1;
                            }
                        }
                        // Y축 실패 -> X축 시도 (Fallback)
                        if (!moved && dx != 0) {
                            int nx = e->x + dirX;
                            int ny = e->y;
                            if (map_is_walkable(m, nx, ny) && !map_get_enemy_at(m, nx, ny)) {
                                e->x = nx;
                                e->y = ny;
                                moved = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}