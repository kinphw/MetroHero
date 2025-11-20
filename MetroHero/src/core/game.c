#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include "game.h"
#include "../world/map.h"
#include "../entity/player.h"

void game_run(void) {
    Map map;
    Player player;

    map_init(&map);
    player_init(&player);

    while (1) {
        system("cls");  // 콘솔 화면 초기화
        map_draw(&map, &player);

        printf("\n");
        printf("[w,a,s,d] 이동 | [q] 종료\n");

        int cmd = getchar();
        getchar(); // 개행 제거

        if (cmd == 'q') break;

        player_move(&player, &map, cmd);
    }
}
