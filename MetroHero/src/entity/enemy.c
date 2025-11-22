#include "enemy.h"
#include <stdio.h>

void enemy_init(Enemy* e, char type, int x, int y) {
    e->type = type;
    e->x = x;
    e->y = y;
    e->isAlive = 1;

    // 타입별 스탯 설정
    switch (type) {
    case 'a':  // 고블린
        e->name = "맹혹한고양이";
        e->maxHp = 5;
        e->hp = 5;
        //e->attack = 2;

        e->attackMin = 1;  // ★ 1~3 데미지
        e->attackMax = 3;

        e->defense = 0;
        break;

    case 'b':  // 검은괴물
        e->name = "B-370";
        e->maxHp = 10;
        e->hp = 10;
        //e->attack = 3;

        e->attackMin = 2;  // ★ 2~4 데미지
        e->attackMax = 4;

        e->defense = 1;
        break;

    default:  // 알 수 없는 타입
        e->name = "몬스터";
        e->maxHp = 3;
        e->hp = 3;
        //e->attack = 1;

        e->attackMin = 1;
        e->attackMax = 2;

        e->defense = 0;
        break;
    }
}

int enemy_is_at(const Enemy* e, int x, int y) {
    return (e->isAlive && e->x == x && e->y == y);
}