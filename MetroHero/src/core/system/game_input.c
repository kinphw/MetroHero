#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include "game_internal.h"
#include "../ui/ui.h"
#include "../logic/combat.h"
#include "../../cinematic/cinematic.h"
#include "../../world/glyph.h"
#include "raylib.h"

// ★ 키 반복 입력을 위한 타이머 구조체
typedef struct {
    int lastKey;              // 마지막으로 눌린 키
    double keyDownTime;       // 키를 누르기 시작한 시간
    double lastRepeatTime;    // 마지막 반복 입력 시간
    double initialDelay;      // 초기 딜레이 (초)
    double repeatInterval;    // 반복 간격 (초, 점점 짧아짐)
    double minInterval;       // 최소 반복 간격
} KeyRepeatState;

static KeyRepeatState g_keyRepeat = { 0, 0.0, 0.0, 0.3, 0.15, 0.05 };

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

// ★ 이동 키인지 확인
static int IsMovementKey(int key) {
    return (key == KEY_W || key == KEY_S || key == KEY_A || key == KEY_D ||
            key == KEY_UP || key == KEY_DOWN || key == KEY_LEFT || key == KEY_RIGHT);
}

// ★ 키 반복 입력 처리 (누르고 있으면 점점 빨라짐)
static int GetRepeatingKey() {
    double currentTime = GetTime();

    // 이동 키들 체크
    int pressedKeys[] = { KEY_W, KEY_S, KEY_A, KEY_D, KEY_UP, KEY_DOWN, KEY_LEFT, KEY_RIGHT };
    int currentKey = 0;

    for (int i = 0; i < 8; i++) {
        if (IsKeyDown(pressedKeys[i])) {
            currentKey = pressedKeys[i];
            break;
        }
    }

    // 키가 안 눌려있으면 초기화
    if (currentKey == 0) {
        g_keyRepeat.lastKey = 0;
        g_keyRepeat.keyDownTime = 0.0;
        g_keyRepeat.lastRepeatTime = 0.0;
        g_keyRepeat.repeatInterval = 0.15;  // 반복 간격 초기화
        return 0;
    }

    // 새로운 키가 눌렸을 때
    if (currentKey != g_keyRepeat.lastKey) {
        g_keyRepeat.lastKey = currentKey;
        g_keyRepeat.keyDownTime = currentTime;
        g_keyRepeat.lastRepeatTime = currentTime;
        g_keyRepeat.repeatInterval = 0.15;  // 반복 간격 초기화
        return currentKey;  // 첫 입력은 즉시 반환
    }

    // 같은 키를 계속 누르고 있을 때
    double heldTime = currentTime - g_keyRepeat.keyDownTime;

    // 초기 딜레이 대기
    if (heldTime < g_keyRepeat.initialDelay) {
        return 0;
    }

    // 반복 간격 체크
    double timeSinceLastRepeat = currentTime - g_keyRepeat.lastRepeatTime;
    if (timeSinceLastRepeat >= g_keyRepeat.repeatInterval) {
        g_keyRepeat.lastRepeatTime = currentTime;

        // 점점 빨라지게 (최소값까지)
        g_keyRepeat.repeatInterval *= 0.85;
        if (g_keyRepeat.repeatInterval < g_keyRepeat.minInterval) {
            g_keyRepeat.repeatInterval = g_keyRepeat.minInterval;
        }

        return currentKey;
    }

    return 0;
}

// 입력 처리 및 로직 업데이트
void game_process_input(GameState* state) {
    if (state->player.hp <= 0) {
        cinematic_play_ending(1);
        state->isRunning = 0;
        return;
    }

    // ★ 이동 키는 반복 입력 처리, 다른 키는 한 번만
    int key = GetRepeatingKey();  // 이동 키 반복 처리
    if (key == 0) {
        // 이동 키가 아닌 다른 키 체크 (한 번만)
        key = GetKeyPressed();
    }

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
