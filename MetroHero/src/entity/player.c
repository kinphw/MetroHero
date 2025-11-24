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

    p->dirX = 0;
    p->dirY = 1;   // 아래 방향을 기본값

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
    case 'w':
        p->dirX = 0; p->dirY = -1;
        ny--;
        break;

    case 's':
        p->dirX = 0; p->dirY = 1;
        ny++;
        break;

    case 'a':
        p->dirX = -1; p->dirY = 0;
        nx--;
        break;

    case 'd':
        p->dirX = 1;  p->dirY = 0;
        nx++;
        break;
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

void player_apply_item(Player* p, const char* itemType, const char* itemName) {

    // ======== 무기 적용 ========
    if (strcmp(itemType, "weapon") == 0) {
        p->weaponName = itemName;

        // 무기 종류별 능력치 설정
        if (strcmp(itemName, "초보자 검") == 0) {
            p->attackMin = 1000;
            p->attackMax = 10000;
        }
        else if (strcmp(itemName, "강철 검") == 0) {
            p->attackMin = 20;
            p->attackMax = 150;
        }
        else if (strcmp(itemName, "마력의 검") == 0) {
            p->attackMin = 50;
            p->attackMax = 200;
        }

        ui_add_log("무기를 장착했다!");
        return;
    }

    // ======== 방어구 적용 ========
    if (strcmp(itemType, "armor") == 0) {
        p->armorName = itemName;

        if (strcmp(itemName, "가죽 갑옷") == 0)
            p->defense = 2;
        else if (strcmp(itemName, "철 갑옷") == 0)
            p->defense = 5;
        else if (strcmp(itemName, "마나 갑옷") == 0)
            p->defense = 10;

        ui_add_log("방어구를 착용했다!");
        return;
    }

    // ======== 아이템 적용 (영구 버프 버전) ========
    if (strcmp(itemType, "item") == 0) {
        if (strcmp(itemName, "HP 포션") == 0) {
            p->hp += 10;
            if (p->hp > p->maxHp) p->hp = p->maxHp;
        }
        else if (strcmp(itemName, "힘의 물약") == 0) {
            p->attackMin += 2;
            p->attackMax += 2;
        }
        else if (strcmp(itemName, "민첩의 물약") == 0) {
            p->defense += 1;
        }

        ui_add_log("아이템 효과가 적용되었다!");
        return;
    }
}
