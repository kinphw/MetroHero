/* ■ UTF-8 FORCE ■ */
#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>   // _getch
#include <ctype.h>   // tolower
#include "game.h"
#include "ui.h"
#include "../world/map.h"
#include "../entity/player.h"

void game_run(void) {
    Map map;
    Player player;

    map_init(&map);
    player_init(&player);
    ui_init();

    while (1) {
        console_clear_fast();      // ★ system("cls") 대신 이거
        // system("cls");
        map_draw(&map, &player);

        printf("\n");
        printf("[w,a,s,d] 이동 | [q] 종료\n");

        int cmd = _getch();        // 엔터 없이 즉시 입력
        cmd = tolower(cmd);        // 대문자도 허용

        if (cmd == 'q') break;

        player_move(&player, &map, cmd);
    }
}
