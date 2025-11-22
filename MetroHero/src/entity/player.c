#include <stdio.h>
#include <stdlib.h>  // ★ rand() 사용
#include <time.h>    // ★ time() 사용

#include "../world/map.h"
#include "player.h"
#include "../core/ui.h"  // ★ ui_add_log 사용을 위해 추가
#include "../core/combat.h"  // ★ 추가

void player_init(Player* p) {
    // ★ 맵 크기를 알 수 없으므로 기본값 설정
    p->x = 20;  // 또는 map_init 후에 설정
    p->y = 10;

    // ★ 초기 스탯
    p->maxHp = 10;
    p->hp = 10;
    //p->attack = 3;

    // ★ 공격력 범위 설정
    p->attackMin = 2;
    p->attackMax = 5;

    p->defense = 0;

    p->weaponName = "";
    p->armorName = "";
    p->item1 = "";
}


void player_move(Player* p, const Map* m, int cmd) {
    int nx = p->x;
    int ny = p->y;

    switch (cmd) {
    case 'w': ny--; break;
    case 's': ny++; break;
    case 'a': nx--; break;
    case 'd': nx++; break;
    }

    // ★ 목표 위치에 적이 있는지 확인
    Enemy* targetEnemy = map_get_enemy_at((Map*)m, nx, ny);

    if (targetEnemy != NULL) {
        // ★ 적이 있으면 전투!
        combat_attack_enemy(p, targetEnemy, (Map*)m);
        return;  // 이동하지 않음
    }

    // 일반 이동
    if (map_is_walkable(m, nx, ny)) {
        p->x = nx;
        p->y = ny;
    }
}
