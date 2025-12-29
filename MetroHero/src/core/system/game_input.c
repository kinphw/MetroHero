#define _CRT_SECURE_NO_WARNINGS
#include <stdio.h>
#include <ctype.h>
#include "game_internal.h"
#include "../ui/ui.h"
#include "../audio/audio.h" // Added
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
        
        // Context Action (Space / Enter / Z)
        case KEY_SPACE: case KEY_ENTER: case KEY_KP_ENTER: case KEY_Z: return ' '; 
        
        // Menu / Cancel (ESC / X)
        case KEY_ESCAPE: case KEY_X: return 'x';
        
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
// 입력 처리 및 로직 업데이트
void game_process_input(GameState* state) {
    // 1. 사망 상태 처리
    if (state->isPlayerDead) {
        int key = GetKeyPressed();
        if (key == KEY_ENTER || key == KEY_SPACE) {
            cinematic_play_ending(1);
            state->isRunning = 0;
        }
    }

    // ★ Defense Check (Real-time polling)
    // Must be checked every frame regardless of GetKeyPressed
    int wasDefending = state->player.isDefending; // Added
    
    if (IsKeyDown(KEY_KP_0) || IsKeyDown(KEY_ZERO)) {
        state->player.isDefending = 1;
    } else {
        state->player.isDefending = 0;
    }

    // Play SFX on activation
    if (!wasDefending && state->player.isDefending) {
        audio_play_sfx("shield_equip");
    }

    int key = GetRepeatingKey();
    if (key == 0) key = GetKeyPressed();
    if (key == 0) return;

    int cmd = MapKeyToCmd(key);
    if (cmd == 0) return;

    // 2. 시스템 메뉴 처리
    if (state->inSystemMenu) {
        if (cmd == 'w' || cmd == 'a') { // UP
            state->systemMenuCursor--;
            if (state->systemMenuCursor < 0) state->systemMenuCursor = 0;
        }
        else if (cmd == 's' || cmd == 'd') { // DOWN
            state->systemMenuCursor++;
            if (state->systemMenuCursor > 2) state->systemMenuCursor = 2;
        }
        else if (cmd == ' ') { // SELECT
            if (state->systemMenuCursor == 0) { // Inventory
                state->inSystemMenu = 0;
                state->inInventory = 1;
                state->inventoryCursor = 0;
            }
            else if (state->systemMenuCursor == 1) { // Save
                ui_add_log("게임을 저장했습니다. (가상)");
                state->inSystemMenu = 0;
            }
            else if (state->systemMenuCursor == 2) { // Exit
                state->isRunning = 0;
                CloseWindow();
            }
        }
        else if (cmd == 'x') { // Close Menu
            state->inSystemMenu = 0;
        }
        return;
    }

    // 3. 인벤토리 처리
    if (state->inInventory) {
        if (cmd == 'x') {
            state->inInventory = 0;
        }
        else if (cmd == 'w' || cmd == 'a') {
            state->inventoryCursor--;
            if (state->inventoryCursor < 0) state->inventoryCursor = 0;
        }
        else if (cmd == 's' || cmd == 'd') {
            state->inventoryCursor++;
            if (state->inventoryCursor >= state->player.inventory.count) 
                state->inventoryCursor = state->player.inventory.count - 1;
        }
        else if (cmd == ' ') {
             player_use_item(&state->player, state->inventoryCursor);
        }
        return;
    }

    // 4. 대화 모드 처리
    if (state->inDialogue && state->currentNPC != NULL) {
        if (cmd == ' ') { // NEXT
            if (state->currentNPC->currentDialogue == state->currentNPC->dialogueCount - 1) {
                // End Dialogue
                state->currentNPC->currentDialogue = 0;
                state->inDialogue = 0;
                state->currentNPC = NULL;
                ui_clear_dialogue_area();
            }
            else {
                // Next Line
                npc_next_dialogue(state->currentNPC);
                ui_draw_dialogue(state->currentNPC);
            }
        }
        else if (cmd == 'x') { // CANCEL
            state->currentNPC->currentDialogue = 0;
            state->inDialogue = 0;
            state->currentNPC = NULL;
            ui_clear_dialogue_area();
        }
        return;
    }

    // 5. 일반 게임 플레이
    if (cmd == 'x') {
        state->inSystemMenu = 1;
        state->systemMenuCursor = 0;
        return;
    }

    ui_hide_combat_effect(); // Clear previous effects

    // ★ Defense Block: Cannot move or attack while defending
    if (state->player.isDefending) {
        return;
    }

    // Defense Check moved to top

    // 이동
    if (cmd == 'w' || cmd == 's' || cmd == 'a' || cmd == 'd') {
        player_move(&state->player, &state->map, cmd);
    }

    // Context Action (Space)
    if (cmd == ' ') {
        int tx = state->player.x + state->player.dirX;
        int ty = state->player.y + state->player.dirY;
        int actionTaken = 0;

        // A. Interact with NPC
        NPC* interactNpc = map_get_npc_at(&state->map, tx, ty);
        if (interactNpc != NULL) {
            // Check Event Condition
            // Check Event Condition
            int canInteract = 1;
            if (interactNpc->event.reqFlag) {
                 int val = event_get_flag(&state->eventRegistry, interactNpc->event.reqFlag);
                 int req = interactNpc->event.reqVal > 0 ? interactNpc->event.reqVal : 1;
                 
                 if (val < req) {
                     canInteract = 0;
                     if (interactNpc->event.failMsg) {
                         ui_add_log(interactNpc->event.failMsg);
                     } else {
                         //ui_add_log("지금은 대화할 수 없는 것 같다."); // Default message optional
                     }
                 }
            }

            if (canInteract) {
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
                
                // Trigger Effect (On Interaction Start? Or End? Usually End, but for simple flags Start is OK)
                // For complex RPGs, set flag after dialogue ends. But here we do it on start for simplicity unless 'useDialogueBox'
                // Actually, let's do it on START for now, or maybe only if not re-triggerable?
                // `setVal` 0 means increment.
                if (interactNpc->event.setFlag) {
                    if (interactNpc->event.setVal > 0) {
                        event_set_flag(&state->eventRegistry, interactNpc->event.setFlag, interactNpc->event.setVal);
                    } else {
                        event_add_flag(&state->eventRegistry, interactNpc->event.setFlag, 1);
                    }
                }
            }
            actionTaken = 1;
        }

        // B. Open Chest
        if (!actionTaken) {
            Chest* chest = map_get_chest_at(&state->map, tx, ty);
            if (chest != NULL && !chest->isOpened) {
                
                // Check Event Condition
                int canOpen = 1;
                if (chest->event.reqFlag) {
                    int val = event_get_flag(&state->eventRegistry, chest->event.reqFlag);
                    int req = chest->event.reqVal > 0 ? chest->event.reqVal : 1;
                    if (val < req) {
                        canOpen = 0;
                        if (chest->event.failMsg) ui_add_log(chest->event.failMsg);
                        else ui_add_log("잠겨있다.");
                    }
                }
                
                if (canOpen) {
                    chest->isOpened = 1;
                    player_apply_item(&state->player, chest->itemType, chest->itemName);
                    char msg[128];
                    snprintf(msg, sizeof(msg), "📦 상자를 열었다! → %s 획득!", chest->itemName);
                    ui_add_log(msg);
                    audio_play_sfx("door_creak"); // SFX Added
                    
                    // Trigger
                    if (chest->event.setFlag) {
                        if (chest->event.setVal > 0) event_set_flag(&state->eventRegistry, chest->event.setFlag, chest->event.setVal);
                        else event_add_flag(&state->eventRegistry, chest->event.setFlag, 1);
                    }
                }
                actionTaken = 1;
            }
        }

        // C. Open Door
        if (!actionTaken) {
            Door* door = map_get_door_at(&state->map, tx, ty);
            if (door != NULL && !door->isOpen) {
                
                // Check Event Condition
                int canOpen = 1;
                if (door->event.reqFlag) {
                    int val = event_get_flag(&state->eventRegistry, door->event.reqFlag);
                    int req = door->event.reqVal > 0 ? door->event.reqVal : 1;
                    if (val < req) {
                        canOpen = 0;
                        if (door->event.failMsg) ui_add_log(door->event.failMsg);
                        else ui_add_log("문이 잠겨있다.");
                    }
                }
                
                if (canOpen) {
                    door->isOpen = 1;
                    ui_add_log("철컹! 문이 열렸다.");
                    audio_play_sfx("door_creak"); // SFX Added
                    
                    // Trigger
                    if (door->event.setFlag) {
                        if (door->event.setVal > 0) event_set_flag(&state->eventRegistry, door->event.setFlag, door->event.setVal);
                        else event_add_flag(&state->eventRegistry, door->event.setFlag, 1);
                    }
                }
                actionTaken = 1;
            }
        }

        // D. Attack (Default if no interaction)
        if (!actionTaken) {
             combat_try_attack(state);
        }
    }
}
