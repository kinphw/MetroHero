// core/combat.c
#include <stdio.h>
// #include <windows.h>  // Removed for Raylib

#include "combat.h"
#include "../ui/ui.h"
#include "../audio/audio.h" // Added
#include "../../world/map.h"
#include "../ui/ui.h"
#include "../../world/map.h"
#include "../../world/glyph.h"
#include "../ui/ui.h"
#include "../system/context.h" // ★ GameState 정의 필요
#include "raylib.h"
#include "event.h" // Added

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
        // dmg -= target->defense; // Defense removed
        if (dmg < 1) dmg = 1;

        target->hp -= dmg;
        audio_play_sfx("sword_hit"); // SFX Added

        char buf[128];
        snprintf(buf, sizeof(buf), "⚔ %s에게 %d 피해! (HP: %d)", target->name, dmg, target->hp);
        ui_add_combat_log(buf); // 우측 전투 로그에 출력

        if (target->hp <= 0) {
            target->isAlive = 0;
            // ★ 적 처치 -> 이미지 유지 (이미 표시됨)
            // 맵 타일 정리 (적 제거) - 추후 자동 clean up
             snprintf(buf, sizeof(buf), "★ %s 처치!", target->name);
             ui_add_combat_log(buf);
             audio_play_sfx("explosion"); // SFX Added
             
             // Trigger Event
             if (target->event.setFlag) {
                 if (target->event.setVal > 0) event_set_flag(&state->eventRegistry, target->event.setFlag, target->event.setVal);
                 else event_add_flag(&state->eventRegistry, target->event.setFlag, 1);
             }
        }
    } else {
        // 허공에 공격
        // ui_add_combat_log("허공을 가랐다.");
        audio_play_sfx("sword_swing"); // SFX Added
    }
}

// ★ AI 업데이트 Implementation
// ★ Helper: Check if enemy can move to target position (handling multi-tile)
static int can_enemy_move(Map* m, Player* p, Enemy* e, int tx, int ty) {
    for (int y = 0; y < e->height; y++) {
        for (int x = 0; x < e->width; x++) {
            int cx = tx + x;
            int cy = ty + y;

            // 1. Map Walkable
            if (!map_is_walkable(m, cx, cy)) return 0;
            
            // 2. Player Collision
            if (p->x == cx && p->y == cy) return 0;

            // 3. Other Enemy Collision
            Enemy* other = map_get_enemy_at(m, cx, cy);
            if (other != NULL && other != e) return 0;
        }
    }
    return 1;
}

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

        // 거리 계산 (Manhattan Distance from Closest Point)
        // For multi-tile, we want distance from bounding box?
        // Simple approximation: Center to Center or Closest Edge?
        // Current logic: p->x - e->x. This assumes e->x is top-left.
        // For correctness, we should use distance to the 'rect'.
        // But for simply chasing, moving towards player is fine.
        // We just need to check if ANY part of enemy is adjacent to player for attack.
        
        // Attack Range Check for Multi-Tile:
        // Attack if Player is adjacent to ANY tile of the enemy.
        // Or simpler: distance from (e->x, e->y) to (p->x, p->y) adjusted for size.
        // Let's keep it simple for now: standard chase logic works for Top-Left.
        // But attack check needs refinement.
        
        // Improved Distance Check:
        int dist = 999;
        // Find minimum distance between Player and any tile of Enemy
        // Optimization: Clamping
        int closestX = (p->x < e->x) ? e->x : (p->x >= e->x + e->width) ? e->x + e->width - 1 : p->x;
        int closestY = (p->y < e->y) ? e->y : (p->y >= e->y + e->height) ? e->y + e->height - 1 : p->y;
        dist = abs(p->x - closestX) + abs(p->y - closestY);

        // ★ 인식 범위 체크 및 로그 (Alert Logic)
        int hasLOS = map_check_los(m, e->x, e->y, p->x, p->y); // Approximate LOS from top-left
        
        if ((dist <= e->detectionRange && hasLOS) || e->isProvoked) {
            if (!e->isChasing) { // Not alerted yet
                e->isChasing = 1; // Mark as alerted
                show_enemy_image(state, e);
                char buf[128];
                if (e->chaseOnSight || e->isProvoked) {
                    snprintf(buf, sizeof(buf), "%s이(가) 당신을 발견했습니다, 당신에게 접근합니다!", e->name);
                } else {
                    snprintf(buf, sizeof(buf), "%s이(가) 당신을 발견했습니다!", e->name);
                }
                ui_add_combat_log(buf);
            }
        } else {
            e->isChasing = 0;
        }

        // 행동 결정
        if (e->isChasing) {
            // 1. 공격 (인접 + 선공O + 쿨타임)
            if (dist <= 1 && (e->attackOnSight || e->isProvoked)) {
                
                // Update Direction towards Player even if attacking
                int dx_p = p->x - e->x; // Vector to player
                int dy_p = p->y - e->y;
                if (abs(dx_p) > abs(dy_p)) e->direction = (dx_p > 0) ? 0 : 1;
                else e->direction = (dy_p > 0) ? 3 : 2;

                if (e->attackCooldown <= 0) {
                    e->attackCooldown = e->attackInterval; 
                    show_enemy_image(state, e);

                    int dmg = e->attackMin + rand() % (e->attackMax - e->attackMin + 1);
                    if (dmg < 1) dmg = 1;
                    if (p->isDefending) dmg = 1;

                    p->hp -= dmg;
                    audio_play_sfx("claw_hit"); 

                    char buf[128];
                    snprintf(buf, sizeof(buf), "☠ %s의 공격! %d 피해 (HP: %d)", e->name, dmg, p->hp);
                    ui_add_combat_log(buf);

                    if (p->hp <= 0) {
                        state->isPlayerDead = 1;
                        ui_add_log(COLOR_BRIGHT_RED "★ 당신은 쓰러졌습니다..." COLOR_RESET);
                        ui_add_combat_log(COLOR_BRIGHT_RED "★ 당신은 쓰러졌습니다..." COLOR_RESET);
                        ui_add_combat_log(" ");
                        ui_add_combat_log(COLOR_BRIGHT_YELLOW "[ENTER]를 눌러 계속..." COLOR_RESET);
                    }
                }
            }
            // 2. 추적
            else if (dist > 1 && (e->chaseOnSight || e->isProvoked)) {
                if (e->moveCooldown <= 0) {
                     e->moveCooldown = e->moveInterval; 
            
                    // Target: Player Position
                    // For multi-tile, just move towards player generally
                    int dx = p->x - e->x; // rough vector
                    int dy = p->y - e->y;
                    
                    // Adjust target logic: We want to get 'closestX' closer to 'p->x'
                    // Actually standard grid movement towards player works fine.
                    
                    int tryX = e->x;
                    int tryY = e->y;
                    
                    int dirX = (dx > 0) ? 1 : -1;
                    int dirY = (dy > 0) ? 1 : -1;
                    
                    int moved = 0;

                    // 1. Primary Axis
                    if (abs(dx) >= abs(dy)) {
                        // Try X
                        if (dx != 0) {
                            if (can_enemy_move(m, p, e, e->x + dirX, e->y)) {
                                e->x += dirX;
                                e->direction = (dirX > 0) ? 0 : 1; // 0: Right, 1: Left
                                moved = 1;
                            }
                        }
                        // Fallback Y
                        if (!moved && dy != 0) {
                            if (can_enemy_move(m, p, e, e->x, e->y + dirY)) {
                                e->y += dirY;
                                e->direction = (dirY > 0) ? 3 : 2; // 3: Down, 2: Up
                                moved = 1;
                            }
                        }
                    } else {
                        // Try Y
                        if (dy != 0) {
                            if (can_enemy_move(m, p, e, e->x, e->y + dirY)) {
                                e->y += dirY;
                                e->direction = (dirY > 0) ? 3 : 2;
                                moved = 1;
                            }
                        }
                        // Fallback X
                        if (!moved && dx != 0) {
                             if (can_enemy_move(m, p, e, e->x + dirX, e->y)) {
                                e->x += dirX;
                                e->direction = (dirX > 0) ? 0 : 1;
                                moved = 1;
                            }
                        }
                    }
                }
            }
        }
    }
}