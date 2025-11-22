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
#include "combat.h"  // ★ 추가

void game_run(void) {
    Map map;
    Player player;
    int prevX, prevY;  // ★ 이전 위치 저장

    map_init(&map, 1);
    player_init(&player);

    // ★ 스폰 포인트에 플레이어 배치
    player.x = map.spawnX;
    player.y = map.spawnY;

    ui_init();


    // ★ 초기 화면은 한 번만 그리기
    console_clear_fast();
    map_draw_at(&map, &player, 0, 0);

    // ★ 상태창 (상단)
    ui_draw_stats(&player, EQ_X, 0, EQ_W, 10);

    // ★ 장비창 (중간)
    ui_draw_equipment(&player, EQ_X, 10, EQ_W, 6);
    ui_draw_log(0, LOG_Y, LOG_W, LOG_H);

    // ★ 입력 안내는 초기에 한 번만 출력 (로그창 밖)
    console_goto(0, SCREEN_H - 1);
    printf("[화살표/WASD] 이동 | [Q] 종료");

    prevX = player.x;
    prevY = player.y;

    while (1) {

        // ★ 플레이어 사망 체크
        if (player.hp <= 0) {
            ui_add_log("게임 오버!");
            ui_draw_log(0, LOG_Y, LOG_W, LOG_H);
            console_goto(0, SCREEN_H - 1);
            printf("아무 키나 누르면 종료...");
            _getch();
            break;
        }

        int cmd = _getch();

        // ★ 화살표 키 처리 (2바이트 입력)
        if (cmd == 0 || cmd == 224) {  // 특수 키 감지
            cmd = _getch();  // 실제 키 코드 읽기

            switch (cmd) {
            case 72: cmd = 'w'; break;  // 위쪽 화살표
            case 80: cmd = 's'; break;  // 아래쪽 화살표
            case 75: cmd = 'a'; break;  // 왼쪽 화살표
            case 77: cmd = 'd'; break;  // 오른쪽 화살표
            }
        }

        cmd = tolower(cmd);

        if (cmd == 'q') break;

        // ★ 행동 전에 이펙트 지우기
        ui_clear_combat_effect(107, 2);

        // ★ 이동 전에 목표 위치의 적 확인
        int targetX = player.x;
        int targetY = player.y;

        switch (cmd) {
        case 'w': targetY--; break;
        case 's': targetY++; break;
        case 'a': targetX--; break;
        case 'd': targetX++; break;
        }

        Enemy* targetEnemy = map_get_enemy_at(&map, targetX, targetY);

        player_move(&player, &map, cmd);

        // ★ 전투가 발생했고 적이 죽었으면 화면에서 지우기
        if (targetEnemy != NULL && !targetEnemy->isAlive) {
            console_goto(targetEnemy->x * 2, targetEnemy->y);
            printf("%s", tile_to_glyph(map.tiles[targetEnemy->y][targetEnemy->x]));
        }

        if (prevX != player.x || prevY != player.y) {
            // ★ 이전 위치에 적이 있었는지 확인
            Enemy* prevEnemy = map_get_enemy_at(&map, prevX, prevY);

            // 이전 위치 복원
            console_goto(prevX * 2, prevY);
            if (prevEnemy != NULL) {
                // 적이 있으면 적을 다시 그림
                printf("%s", enemy_to_glyph(prevEnemy->type));
            }
            else {
                // 바닥 타일 복원
                printf("%s", tile_to_glyph(map.tiles[prevY][prevX]));
            }

            // 새 위치 그리기
            console_goto(player.x * 2, player.y);
            printf(GLYPH_PLAYER);

            prevX = player.x;
            prevY = player.y;
        }

        // ★ 인접 적 체크
        combat_check_nearby_enemy(&map, &player);

        // ★ 상태창 갱신 (HP 변경 반영)
        ui_draw_stats(&player, EQ_X, 0, EQ_W, 10);

        // ★ 로그창 갱신
        ui_draw_log(0, LOG_Y, LOG_W, LOG_H);
    }
}
