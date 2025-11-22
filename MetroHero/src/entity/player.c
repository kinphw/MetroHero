#include <stdio.h>

#include "../world/map.h"
#include "player.h"
#include "../core/ui.h"  // ★ ui_add_log 사용을 위해 추가

void player_init(Player* p) {
    // ★ 맵 크기를 알 수 없으므로 기본값 설정
    p->x = 20;  // 또는 map_init 후에 설정
    p->y = 10;

    p->weaponName = "목검";
    p->armorName = "천 갑옷";
    p->item1 = "HP 포션";
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

    if (map_is_walkable(m, nx, ny)) {
        p->x = nx;
        p->y = ny;

        // 이동 성공 로그 찍기
        //char logMsg[128];
        //snprintf(logMsg, sizeof(logMsg), "이동: (%d, %d)", p->x, p->y);
		//ui_add_log(logMsg);
    }
}
