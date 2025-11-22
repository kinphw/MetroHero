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
    int prevX, prevY;  // ★ 이전 위치 저장

    map_init(&map);
    player_init(&player);
    ui_init();


    // ★ 초기 화면은 한 번만 그리기
    console_clear_fast();
    map_draw_at(&map, &player, 0, 0);
    ui_draw_equipment(&player, EQ_X, 0, EQ_W, TOP_H);
    ui_draw_log(0, LOG_Y, LOG_W, LOG_H);
    console_goto(0, SCREEN_H - 1);
    printf("[w,a,s,d] 이동 | [q] 종료");

    prevX = player.x;
    prevY = player.y;

    while (1) {
        int cmd = _getch();
        cmd = tolower(cmd);

        if (cmd == 'q') break;

        player_move(&player, &map, cmd);

        // ★ 플레이어가 실제로 움직였을 때만 화면 갱신
        if (prevX != player.x || prevY != player.y) {
            // 이전 위치 지우기
            console_goto(prevX * 2, prevY);
            printf("..");

            // 새 위치 그리기
            console_goto(player.x * 2, player.y);
            printf(">>");

            prevX = player.x;
            prevY = player.y;
        }
    }
}
