#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <stdlib.h>
#include <conio.h>   // _getch
#include <ctype.h>   // tolower
#include "game.h"
#include "ui.h"
#include "cinematic.h"  // ★ 시네마틱 시스템 추가
#include "../world/map.h"
#include "../entity/player.h"
#include "../world/glyph.h"  // ★ 추가
#include "combat.h"  // ★ 추가

void game_run(void) {
    Map map;
    Player player;
    int prevX, prevY;  // ★ 이전 위치 저장

    // ★ 대화 모드 관련 변수
    int inDialogue = 0;
    NPC* currentNPC = NULL;

    ui_init();

    // ★ 게임 인트로 시네마틱 재생
    cinematic_play_intro();

    map_init(&map, 1);
    player_init(&player);

    // ★ 스테이지 1 시작 시네마틱
    cinematic_play_stage_start(1);

    // ★ 스폰 포인트에 플레이어 배치
    player.x = map.spawnX;
    player.y = map.spawnY;


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

        int tx = player.x + player.dirX;
        int ty = player.y + player.dirY;

        // ★ 대화 모드일 때
        if (inDialogue && currentNPC != NULL) {
            int cmd = _getch();
            cmd = tolower(cmd);

            if (cmd == '0') {
                // 🔥 마지막 대사인지 체크
                if (currentNPC->currentDialogue == currentNPC->dialogueCount - 1) {

                    // 🔥 여기 추가
                    currentNPC->currentDialogue = 0;

                    // 🔥 대화 종료
                    inDialogue = 0;
                    currentNPC = NULL;

                    // 🔥 대화창 영역 지우기
                    ui_clear_dialogue_area(DIALOGUE_X, DIALOGUE_Y, DIALOGUE_W, DIALOGUE_H);

                    // 🔥 원래 UI 복귀
                    ui_draw_stats(&player, STATUS_X, STATUS_Y, STATUS_W, STATUS_H);
                    ui_draw_equipment(&player, EQUIP_X, EQUIP_Y, EQUIP_W, EQUIP_H);
                    ui_draw_log(0, LOG_Y, LOG_W, LOG_H);

                }
                else {
                    // 🔥 아직 마지막이 아니면 다음 대사
                    npc_next_dialogue(currentNPC);
                    ui_draw_dialogue(currentNPC, DIALOGUE_X, DIALOGUE_Y, DIALOGUE_W, DIALOGUE_H);
                }
            }
            else if (cmd == 't' && currentNPC->canTrade) {
                // 거래 모드 (향후 구현)
                ui_add_log(COLOR_YELLOW "거래 시스템은 곧 추가됩니다!" COLOR_RESET);
                ui_draw_log(LOG_X, LOG_Y, LOG_W, LOG_H);
            }
            else if (cmd == 'x' || cmd == 27) {  // X 또는 ESC

                // 🔥 여기 추가
                currentNPC->currentDialogue = 0;

                // 대화 종료
                inDialogue = 0;
                currentNPC = NULL;

                // 대화창 지우고 원래 UI 복원
                ui_clear_dialogue_area(DIALOGUE_X, DIALOGUE_Y, DIALOGUE_W, DIALOGUE_H);
                ui_draw_stats(&player, STATUS_X, STATUS_Y, STATUS_W, STATUS_H);
                ui_draw_equipment(&player, EQUIP_X, EQUIP_Y, EQUIP_W, EQUIP_H);
            }

            continue;  // 대화 모드에서는 다른 입력 무시
        }

        // ★ 플레이어 사망 체크
        if (player.hp <= 0) {
            // ★ 게임 오버 시네마틱 재생
            cinematic_play_ending(1);  // 1 = 나쁜 엔딩
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


        //// ★ 상자 열기 처리 (E 키)
        //if (cmd == 'e') {
        //    //Chest* chest = map_get_adjacent_chest(&map, player.x, player.y);
        //    Chest* chest = map_get_chest_at(&map, tx, ty);
        //    if (chest != NULL && !chest->isOpened) {
        //        chest->isOpened = 1;
        //        player_apply_item(&player, chest->itemType, chest->itemName);

        //        char msg[128];
        //        snprintf(msg, sizeof(msg),
        //            COLOR_BRIGHT_YELLOW "📦 상자를 열었다! → %s 획득!" COLOR_RESET,
        //            chest->itemName);

        //        ui_add_log(msg);

        //        // 뷰포트 갱신
        //        map_draw_viewport(&map, &player, VIEWPORT_X, VIEWPORT_Y, 40, VIEWPORT_H);

        //        // 장비창 갱신
        //        ui_draw_equipment(&player, EQUIP_X, EQUIP_Y, EQUIP_W, EQUIP_H);
        //    }
        //}

        // ★ NPC 대화 처리 (0 키) - 바로 대화창 표시
        if (cmd == '0') {
            //NPC* npc = map_get_adjacent_npc(&map, player.x, player.y);
            NPC* npc = map_get_npc_at(&map, tx, ty);
            if (npc != NULL) {
                if (npc->useDialogueBox) {
                    // ★ 전용 대화창 모드 진입
                    inDialogue = 1;
                    currentNPC = npc;

                    // ★ 먼저 기존 UI 영역 지우기
                    ui_clear_dialogue_area(DIALOGUE_X, DIALOGUE_Y, DIALOGUE_W, DIALOGUE_H);

                    // ★ 대화창 그리기
                    ui_draw_dialogue(npc, DIALOGUE_X, DIALOGUE_Y, DIALOGUE_W, DIALOGUE_H);

                    // 로그에도 기록
                    char msg[128];
                    snprintf(msg, sizeof(msg), "%s와 대화를 시작했다.", npc->name);
                    ui_add_log(msg);
                    ui_draw_log(LOG_X, LOG_Y, LOG_W, LOG_H);
                }
                else {
                    // ★ 로그창 모드 (기존 방식)
                    const char* dialogue = npc_get_dialogue(npc);

                    char msg[256];
                    snprintf(msg, sizeof(msg),
                        COLOR_BRIGHT_CYAN "💬 %s: " COLOR_RESET "「%s」",
                        npc->name, dialogue);

                    ui_add_log(msg);
                    npc_next_dialogue(npc);
                }
            }

            // Chest
            Chest* chest = map_get_chest_at(&map, tx, ty);
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

        // ★ 인접 NPC 체크 추가
        NPC* nearNpc = map_get_adjacent_npc(&map, player.x, player.y);
        if (nearNpc != NULL) {
            char msg[128];
            snprintf(msg, sizeof(msg),
                "%s이(가) 가까이 있다. [0] 키로 대화할 수 있다.",
                nearNpc->name);
            ui_add_log(msg);
        }

        // ★ 인접 상자 체크 추가 (여기!)
        Chest* nearChest = map_get_adjacent_chest(&map, player.x, player.y);
        if (nearChest != NULL && !nearChest->isOpened) {
            ui_add_log("가까운 곳에 상자가 있다. [E] 키로 열 수 있다.");
        }

        // ★ 상태창 갱신 (HP 변경 반영)
        //ui_draw_stats(&player, STATUS_X, STATUS_Y, STATUS_W, STATUS_H);

        // ★ 로그창 갱신
        //ui_draw_log(0, LOG_Y, LOG_W, LOG_H);

        // ★ 상태창/장비창/로그창은 대화 모드일 때 다시 그리면 안 됨
        if (!inDialogue) {
            ui_draw_stats(&player, STATUS_X, STATUS_Y, STATUS_W, STATUS_H);
            ui_draw_equipment(&player, EQUIP_X, EQUIP_Y, EQUIP_W, EQUIP_H);
            ui_draw_log(0, LOG_Y, LOG_W, LOG_H);
        }
    }
}
