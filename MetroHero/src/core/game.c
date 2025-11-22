#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>   // _getch
#include <ctype.h>   // tolower
#include "game.h"
#include "ui.h"
#include "../world/map.h"
#include "../entity/player.h"
#include "../world/glyph.h"  // ★ 추가

void game_run(void) {
    Map map;
    Player player;
    int prevX, prevY;  // ★ 이전 위치 저장

    map_init(&map, 1);
    player_init(&player);

    // ★ 맵 로드 후 플레이어를 맵 중앙에 배치
    player.x = map.width / 2;
    player.y = map.height / 2;

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

        if (prevX != player.x || prevY != player.y) {
            // 이전 위치를 원래 타일로 복원
            console_goto(prevX * 2, prevY);

            // ★ map.c의 tile_to_glyph를 public으로 만들고 재사용
            extern const char* tile_to_glyph(char t);  // map.h에 선언 추가
            printf("%s", tile_to_glyph(map.tiles[prevY][prevX]));

            // 새 위치 그리기
            console_goto(player.x * 2, player.y);
            printf(GLYPH_PLAYER);

            // ★ 로그창 갱신 추가
            ui_draw_log(0, LOG_Y, LOG_W, LOG_H);

            // ★ 입력 안내 다시 출력 (로그가 덮어쓸 수 있으므로)
            console_goto(0, SCREEN_H - 1);
            printf("[w,a,s,d] 이동 | [q] 종료");

            prevX = player.x;
            prevY = player.y;
        }
    }
}
