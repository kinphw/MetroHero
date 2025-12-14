#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include "game_internal.h"
#include "../ui/ui.h"
#include "../logic/combat.h"
#include "../../cinematic/cinematic.h"
#include "../../world/glyph.h"
#include "raylib.h"

// Helper to map Raylib keys to char 'commands'
static int MapKeyToCmd(int key) {
    switch (key) {
        case KEY_W: case KEY_UP:    return 'w';
        case KEY_S: case KEY_DOWN:  return 's';
        case KEY_A: case KEY_LEFT:  return 'a';
        case KEY_D: case KEY_RIGHT: return 'd';
        case KEY_ZERO: case KEY_KP_0: case KEY_ENTER: case KEY_SPACE: return '0';
        case KEY_Q: return 'q';
        case KEY_X: case KEY_ESCAPE: return 'x';
        case KEY_T: return 't';
        default: return 0;
    }
}

// 입력 처리 및 로직 업데이트
void game_process_input(GameState* state) {
    if (state->player.hp <= 0) {
        cinematic_play_ending(1);
        state->isRunning = 0;
        return;
    }

    int key = GetKeyPressed();
    if (key == 0) return; // No input this frame

    int cmd = MapKeyToCmd(key);
    if (cmd == 0) return; // Unmapped key

    // 1. 대화 모드 처리
    if (state->inDialogue && state->currentNPC != NULL) {
        if (cmd == '0') {
            if (state->currentNPC->currentDialogue == state->currentNPC->dialogueCount - 1) {
                // 대화 끝
                state->currentNPC->currentDialogue = 0;
                state->inDialogue = 0;
                state->currentNPC = NULL;
                ui_clear_dialogue_area();
            }
            else {
                // 다음 대화
                npc_next_dialogue(state->currentNPC);
                ui_draw_dialogue(state->currentNPC);
            }
        }
        else if (cmd == 't' && state->currentNPC->canTrade) {
            ui_add_log("거래 시스템은 곧 추가됩니다!");
        }
        else if (cmd == 'x') {
            state->currentNPC->currentDialogue = 0;
            state->inDialogue = 0;
            state->currentNPC = NULL;
            ui_clear_dialogue_area();
        }
        return;
    }

    // 2. 일반 게임 모드
    if (cmd == 'q') {
        state->isRunning = 0;
        CloseWindow();
        return;
    }

    // 행동 전 이펙트 클리어 (매 프레임 호출하긴 비효율적이나 input이 있을 때만 하므로 OK)
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
            } else {
                const char* dialogue = npc_get_dialogue(interactNpc);
                char msg[256];
                snprintf(msg, sizeof(msg), "💬 %s: 「%s」", interactNpc->name, dialogue);
                ui_add_log(msg);
                npc_next_dialogue(interactNpc);
            }
        }

        Chest* chest = map_get_chest_at(&state->map, tx, ty);
        if (chest != NULL && !chest->isOpened) {
            chest->isOpened = 1;
            player_apply_item(&state->player, chest->itemType, chest->itemName);
            char msg[128];
            snprintf(msg, sizeof(msg), "📦 상자를 열었다! → %s 획득!", chest->itemName);
            ui_add_log(msg);
        }
    }

    // 주변 체크
    combat_check_nearby_enemy(&state->map, &state->player);

    NPC* nearNpc = map_get_adjacent_npc(&state->map, state->player.x, state->player.y);
    if (nearNpc != NULL) {
        // Log removed to reduce spam
    }
}
