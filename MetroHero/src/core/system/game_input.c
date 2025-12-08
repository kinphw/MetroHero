#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <conio.h>
#include <ctype.h>
#include "game_internal.h"
#include "../ui/ui.h"
#include "../logic/combat.h"
#include "../../cinematic/cinematic.h"
#include "../../world/glyph.h"

// 입력 처리 및 로직 업데이트
void game_process_input(GameState* state) {
    // 1. 대화 모드 처리
    if (state->inDialogue && state->currentNPC != NULL) {
        int cmd = _getch();
        cmd = tolower(cmd);

        if (cmd == '0') {
            if (state->currentNPC->currentDialogue == state->currentNPC->dialogueCount - 1) {
                // 대화 끝
                state->currentNPC->currentDialogue = 0;
                state->inDialogue = 0;
                state->currentNPC = NULL;

                ui_clear_dialogue_area();
                // UI 복구는 game_render 혹은 다음 프레임에서 처리됨
                ui_draw_stats(&state->player);
                ui_draw_equipment(&state->player);
                ui_draw_log();
            }
            else {
                // 다음 대화
                npc_next_dialogue(state->currentNPC);
                ui_draw_dialogue(state->currentNPC);
            }
        }
        else if (cmd == 't' && state->currentNPC->canTrade) {
            ui_add_log(COLOR_YELLOW "거래 시스템은 곧 추가됩니다!" COLOR_RESET);
            ui_draw_log();
        }
        else if (cmd == 'x' || cmd == 27) {
            state->currentNPC->currentDialogue = 0;
            state->inDialogue = 0;
            state->currentNPC = NULL;
            
            ui_clear_dialogue_area();
            ui_draw_stats(&state->player);
            ui_draw_equipment(&state->player);
        }
        return;
    }

    // 2. 일반 게임 모드
    if (state->player.hp <= 0) {
        cinematic_play_ending(1);
        state->isRunning = 0;
        return;
    }

    int cmd = _getch();
    if (cmd == 0 || cmd == 224) {
        cmd = _getch();
        switch (cmd) {
        case 72: cmd = 'w'; break;
        case 80: cmd = 's'; break;
        case 75: cmd = 'a'; break;
        case 77: cmd = 'd'; break;
        }
    }
    cmd = tolower(cmd);

    if (cmd == 'q') {
        ui_add_log(COLOR_BRIGHT_RED "정말 종료하시겠습니까? (Y: 종료, 그 외: 취소)" COLOR_RESET);
        ui_draw_log();
        ui_present();

        int confirm = _getch();
        confirm = tolower(confirm);
        if (confirm == 'y') {
            state->isRunning = 0;
        } else {
            ui_add_log("종료를 취소했습니다.");
        }
        return;
    }

    // 행동 전 이펙트 클리어
    ui_hide_combat_effect();

    int targetX = state->player.x;
    int targetY = state->player.y;

    switch (cmd) {
    case 'w': targetY--; break;
    case 's': targetY++; break;
    case 'a': targetX--; break;
    case 'd': targetX++; break;
    }

    // 이동 처리
    player_move(&state->player, &state->map, cmd);

    // 상호작용 (0 키)
    if (cmd == '0') {
        // NPC
        NPC* npc = map_get_npc_at(&state->map, targetX, targetY);  // 내 위치 기반이 아닌 이동 방향?
        // 기존 game.c 로직: targetEnemy는 이동 예측용이었지만
        // '0'키는 map_get_npc_at(..., tx, ty) ??? 
        // 기존 코드를 자세히 보면:
        // int tx = player.x + player.dirX; int ty = ...
        // '0' 누르면 "map_get_npc_at(&map, tx, ty)" -> 바라보는 방향의 NPC와 상호작용
        // 리팩토링 시 주의: cmd가 0일 땐 이동이 아니므로 targetX/Y가 의미가 다름.
        // 바라보는 방향 계산 필요
        
        int tx = state->player.x + state->player.dirX;
        int ty = state->player.y + state->player.dirY;

        NPC* interactNpc = map_get_npc_at(&state->map, tx, ty);
        if (interactNpc != NULL) {
            if (interactNpc->useDialogueBox) {
                state->inDialogue = 1;
                state->currentNPC = interactNpc;
                ui_clear_dialogue_area();
                ui_draw_dialogue(interactNpc);
                
                char msg[128];
                snprintf(msg, sizeof(msg), "%s와 대화를 시작했다.", interactNpc->name);
                ui_add_log(msg);
                ui_draw_log();
            } else {
                const char* dialogue = npc_get_dialogue(interactNpc);
                char msg[256];
                snprintf(msg, sizeof(msg), COLOR_BRIGHT_CYAN "💬 %s: " COLOR_RESET "「%s」", interactNpc->name, dialogue);
                ui_add_log(msg);
                npc_next_dialogue(interactNpc);
            }
        }

        // Chest
        Chest* chest = map_get_chest_at(&state->map, tx, ty);
        if (chest != NULL && !chest->isOpened) {
            chest->isOpened = 1;
            player_apply_item(&state->player, chest->itemType, chest->itemName);
            char msg[128];
            snprintf(msg, sizeof(msg), COLOR_BRIGHT_YELLOW "📦 상자를 열었다! → %s 획득!" COLOR_RESET, chest->itemName);
            ui_add_log(msg);
            ui_render_map_viewport(&state->map, &state->player);
            ui_draw_equipment(&state->player);
        }
    }

    // 주변 체크
    combat_check_nearby_enemy(&state->map, &state->player);

    NPC* nearNpc = map_get_adjacent_npc(&state->map, state->player.x, state->player.y);
    if (nearNpc != NULL) {
        char msg[128];
        snprintf(msg, sizeof(msg), "%s이(가) 가까이 있다. [0] 키로 대화할 수 있다.", nearNpc->name);
        ui_add_log(msg);
    }
    Chest* nearChest = map_get_adjacent_chest(&state->map, state->player.x, state->player.y);
    if (nearChest != NULL && !nearChest->isOpened) {
        ui_add_log("가까운 곳에 상자가 있다. [0] 키로 열 수 있다.");
    }
}
