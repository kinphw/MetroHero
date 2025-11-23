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

    // ★ 뷰포트 (왼쪽) - 40칸 너비로 제한
    map_draw_viewport(&map, &player, VIEWPORT_X, VIEWPORT_Y, 40, VIEWPORT_H);

    // ★ 상태창 (오른쪽 상단)
    ui_draw_stats(&player, STATUS_X, STATUS_Y, STATUS_W, STATUS_H);

    // ★ 장비창 (오른쪽 중단)
    ui_draw_equipment(&player, EQUIP_X, EQUIP_Y, EQUIP_W, EQUIP_H);

    // ★ 로그창 (하단)
    ui_draw_log(LOG_X, LOG_Y, LOG_W, LOG_H);

    // ★ 입력 안내 (최하단)
    console_goto(0, SCREEN_H - 1);
    printf("[화살표/WASD] 이동 | [E] 상자 열기 | [Q] 종료");

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
            // 뷰포트 갱신으로 처리됨
        }

        // ★ 이동했으면 뷰포트만 다시 그리기
        if (prevX != player.x || prevY != player.y) {
            map_draw_viewport(&map, &player, VIEWPORT_X, VIEWPORT_Y, 40, VIEWPORT_H);
            prevX = player.x;
            prevY = player.y;
        }


        // ★ 상자 열기 처리 (E 키)
        if (cmd == 'e') {
            Chest* chest = map_get_adjacent_chest(&map, player.x, player.y);
            if (chest != NULL && !chest->isOpened) {
                chest->isOpened = 1;
                player_apply_item(&player, chest->itemType, chest->itemName);

                char msg[128];
                snprintf(msg, sizeof(msg),
                    COLOR_BRIGHT_YELLOW "📦 상자를 열었다! → %s 획득!" COLOR_RESET,
                    chest->itemName);

                ui_add_log(msg);

                // 뷰포트 갱신
                map_draw_viewport(&map, &player, VIEWPORT_X, VIEWPORT_Y, 40, VIEWPORT_H);

                // 장비창 갱신
                ui_draw_equipment(&player, EQUIP_X, EQUIP_Y, EQUIP_W, EQUIP_H);
            }
        }

        // ★ 인접 적 체크
        combat_check_nearby_enemy(&map, &player);

        // ★ 인접 상자 체크 추가 (여기!)
        Chest* nearChest = map_get_adjacent_chest(&map, player.x, player.y);
        if (nearChest != NULL && !nearChest->isOpened) {
            ui_add_log("가까운 곳에 상자가 있다. [E] 키로 열 수 있다.");
        }

        // ★ 상태창 갱신 (HP 변경 반영)
        ui_draw_stats(&player, STATUS_X, STATUS_Y, STATUS_W, STATUS_H);

        // ★ 로그창 갱신
        ui_draw_log(0, LOG_Y, LOG_W, LOG_H);
    }
}
