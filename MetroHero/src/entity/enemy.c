#include <stdio.h>
#include "enemy.h"
#include "../world/glyph.h"


void enemy_init(Enemy* e, char type, int x, int y) {
    e->type = type;
    e->x = x;
    e->y = y;
    e->isAlive = 1;

    // 타입별 스탯 설정
    switch (type) {
    case 'a': { // 고블린

        // ★ enemy_init 내부에 대사 정의
        static const char* lines[] = {
            "나는 잔인한 냥코다옹",
            "냥코냥코 대전쟁"
        };

        e->name = "맹혹한고양이";
        e->maxHp = 5;
        e->hp = 5;
        //e->attack = 2;

        e->attackMin = 1;  // ★ 1~3 데미지
        e->attackMax = 3;

        e->defense = 0;
        e->dialogues = lines;   // ★ 배열 연결
        e->dialogueCount = sizeof(lines) / sizeof(lines[0]);
        e->dialogueIndex = 0;   // ★ 최초 index 설정
        e->dialogueColor = COLOR_BRIGHT_CYAN;
        break;
    }
    case 'b': { // 검은괴물

        static const char* lines[] = {
            "전철역 내 침입자를 인식했습니다...",
            "경고: 비인가 인원입니다.",
            "돌아가라 날 건들면 후회할것이다"
        };

        e->name = "경비로봇";
        e->maxHp = 10;
        e->hp = 10;
        //e->attack = 3;

        e->attackMin = 100;  // ★ 2~4 데미지
        e->attackMax = 100;

        e->defense = 0;

        e->dialogues = lines;
        e->dialogueCount = sizeof(lines) / sizeof(lines[0]);
        e->dialogueIndex = 0;
        e->dialogueColor = COLOR_RED;
        break;

    }

    default: {  // 알 수 없는 타입

        static const char* lines[] = {
            "키에에에엑!",
            "으르르르르…",
            "끼아아악!"
        };

        e->name = "몬스터";
        e->maxHp = 3;
        e->hp = 3;
        //e->attack = 1;

        e->attackMin = 1;
        e->attackMax = 2;

        e->defense = 0;

        e->dialogues = lines;
        e->dialogueCount = sizeof(lines) / sizeof(lines[0]);
        e->dialogueIndex = 0;
        e->dialogueColor = COLOR_YELLOW;
        break;
    }
    }
}

int enemy_is_at(const Enemy* e, int x, int y) {
    return (e->isAlive && e->x == x && e->y == y);
}