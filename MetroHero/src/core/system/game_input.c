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

// Check Quests based on current flags
static void check_quest_updates(GameState* state) {
    if (!state->currentStageData || !state->currentStageData->quests) return;

    for (int i = 0; i < state->currentStageData->questCount; i++) {
        const QuestConfig* q = &state->currentStageData->quests[i];
        if (q->reqFlag && q->msg) {
            int val = event_get_flag(&state->eventRegistry, q->reqFlag);
            int req = q->reqVal > 0 ? q->reqVal : 1;
            
            if (val >= req) {
                // Check if this is a NEW quest update
                // We compare against pendingQuestMsg too to avoid re-triggering during animation
                if (strcmp(state->activeQuestMsg, q->msg) != 0 && strcmp(state->pendingQuestMsg, q->msg) != 0) {
                    
                    // Start Transition
                    snprintf(state->pendingQuestMsg, sizeof(state->pendingQuestMsg), "%s", q->msg);
                    state->questState = 1; // 1: Complete Animation (Sparkle)
                    state->questTimer = 2.0f; // 2 seconds sparkle
                    
                    // Log single line with Yellow Color
                    char logBuf[512];
                    snprintf(logBuf, sizeof(logBuf), "%s📘 퀘스트 갱신! %s", COLOR_BRIGHT_YELLOW, q->msg);
                    ui_add_log(logBuf);
                    
                    // ★ Award Quest EXP
                    if (q->expReward > 0) {
                        player_add_exp(&state->player, q->expReward);
                    }
                    
                    audio_play_sfx("cinematic_blip"); 
                }
            }
        }
    }
}

// Update Quest Animation State (Call each frame)
void game_update_quest(GameState* state) {
    if (state->questState == 0) return; // Idle/Active

    // Decrease Timer
    state->questTimer -= GetFrameTime(); // Uses Raylib GetFrameTime

    if (state->questState == 1) { // Sparkle Phase
        if (state->questTimer <= 0) {
            state->questState = 2; // Disappear Phase
            state->questTimer = 0.5f; // 0.5 sec hidden
        }
    }
    else if (state->questState == 2) { // Hidden Phase
        if (state->questTimer <= 0) {
            // Apply New Quest
            snprintf(state->activeQuestMsg, sizeof(state->activeQuestMsg), "%s", state->pendingQuestMsg);
            state->questState = 0; // Back to Active
            
            // Optional: Sound for new quest appear?
        }
    }
}

// Wrapper to set flag and check quests
static void trigger_event_flag(GameState* state, const char* flag, int val) {
    if (val > 0) event_set_flag(&state->eventRegistry, flag, val);
    else event_add_flag(&state->eventRegistry, flag, 1);
    
    check_quest_updates(state);
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
        return; // ★ Stop processing other inputs if dead
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
            // ★ Use activeDialogueCount for dynamic dialogue support
            if (state->currentNPC->currentDialogue == state->currentNPC->activeDialogueCount - 1) {
                // End Dialogue
                // ★ Trigger Event Flag on Completion
                if (state->currentNPC->event.setFlag) {
                    trigger_event_flag(state, state->currentNPC->event.setFlag, state->currentNPC->event.setVal);
                }
                
                // ★ Item Reward (Dialogue End)
                if (state->currentNPC->event.giveItem) {
                    const Item* it = item_get(state->currentNPC->event.giveItem);
                    if (it && !inventory_has_item(&state->player.inventory, it->name)) { // Unique give
                         if (inventory_add(&state->player.inventory, it)) {
                             char msg[128];
                             snprintf(msg, sizeof(msg), "%s을(를) 받았다!", it->name);
                             ui_add_log(msg);
                         } else {
                             ui_add_log("가방이 가득 차서 받을 수 없다!");
                         }
                    }
                }

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
            // ★ Update Dialogue State based on flags
            npc_update_dialogue_state(interactNpc, &state->eventRegistry, state->currentStageData);

            // Check Event Condition
            int canInteract = 1;
            
            // 1. Flag Check
            if (interactNpc->event.reqFlag) {
                 int val = event_get_flag(&state->eventRegistry, interactNpc->event.reqFlag);
                 int req = interactNpc->event.reqVal > 0 ? interactNpc->event.reqVal : 1;
                 if (val < req) canInteract = 0;
            }
            
            // 2. Item Check
            if (canInteract && interactNpc->event.reqItem) {
                 if (!inventory_has_item(&state->player.inventory, interactNpc->event.reqItem)) {
                     canInteract = 0;
                 }
            }

            if (!canInteract) {
                 if (interactNpc->event.failMsg) {
                     ui_add_log(interactNpc->event.failMsg);
                 } else {
                     //ui_add_log("조건이 부족하다.");
                 }
            } else {
                // Success: Consume Item if needed
                if (interactNpc->event.reqItem && interactNpc->event.consumeItem) {
                    inventory_remove_item_by_name(&state->player.inventory, interactNpc->event.reqItem);
                    char msg[128];
                    snprintf(msg, sizeof(msg), "%s을(를) 사용했다.", interactNpc->event.reqItem);
                    ui_add_log(msg);
                }

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
                
                // Trigger Effect (Simple Flag / Item Reward)
                int triggerNow = !interactNpc->useDialogueBox;
                
                // ★ Only trigger here if NOT using Dialogue Box (Simple Float Text)
                if (!interactNpc->useDialogueBox && interactNpc->event.setFlag) {
                    trigger_event_flag(state, interactNpc->event.setFlag, interactNpc->event.setVal);
                }    // Item Reward
                    if (interactNpc->event.giveItem) {
                        const Item* it = item_get(interactNpc->event.giveItem);
                        if (it && !inventory_has_item(&state->player.inventory, it->name)) { // Unique give
                             if (inventory_add(&state->player.inventory, it)) {
                                 char msg[128];
                                 snprintf(msg, sizeof(msg), "%s을(를) 받았다!", it->name);
                                 ui_add_log(msg);
                             } else {
                                 ui_add_log("가방이 가득 차서 받을 수 없다!");
                             }
                        }
                    }
                }
                actionTaken = 1;
            }

        // B. Open Chest
        if (!actionTaken) {
            Chest* chest = map_get_chest_at(&state->map, tx, ty);
            if (chest != NULL && !chest->isOpened) {
                
                int canOpen = 1;
                // 1. Flag Check
                if (chest->event.reqFlag) {
                    int val = event_get_flag(&state->eventRegistry, chest->event.reqFlag);
                    int req = chest->event.reqVal > 0 ? chest->event.reqVal : 1;
                    if (val < req) canOpen = 0;
                }
                // 2. Item Check
                if (canOpen && chest->event.reqItem) {
                    if (!inventory_has_item(&state->player.inventory, chest->event.reqItem)) {
                        canOpen = 0;
                    }
                }

                if (!canOpen) {
                     if (chest->event.failMsg) ui_add_log(chest->event.failMsg);
                     else ui_add_log("잠겨있다.");
                } else {
                    // Success
                    if (chest->event.reqItem && chest->event.consumeItem) {
                        inventory_remove_item_by_name(&state->player.inventory, chest->event.reqItem);
                        char msg[128];
                        snprintf(msg, sizeof(msg), "%s을(를) 사용했다.", chest->event.reqItem);
                        ui_add_log(msg);
                    }

                    chest->isOpened = 1;
                    player_apply_item(&state->player, chest->itemType, chest->itemName);
                    char msg[128];
                    snprintf(msg, sizeof(msg), "📦 상자를 열었다! → %s 획득!", chest->itemName);
                    ui_add_log(msg);
                    audio_play_sfx("door_creak"); 
                    
                    // Trigger
                    if (chest->event.setFlag) {
                        trigger_event_flag(state, chest->event.setFlag, chest->event.setVal);
                    }
                    if (chest->event.giveItem) {
                         const Item* it = item_get(chest->event.giveItem);
                         if (it) inventory_add(&state->player.inventory, it);
                    }
                }
                actionTaken = 1;
            }
        }

        // C. Open Door
        if (!actionTaken) {
            Door* door = map_get_door_at(&state->map, tx, ty);
            if (door != NULL && !door->isOpen) {
                
                int canOpen = 1;
                // 1. Flag Check
                if (door->event.reqFlag) {
                    int val = event_get_flag(&state->eventRegistry, door->event.reqFlag);
                    int req = door->event.reqVal > 0 ? door->event.reqVal : 1;
                    if (val < req) canOpen = 0;
                }
                // 2. Item Check
                if (canOpen && door->event.reqItem) {
                    if (!inventory_has_item(&state->player.inventory, door->event.reqItem)) {
                        canOpen = 0;
                    }
                }
                
                if (!canOpen) {
                     if (door->event.failMsg) ui_add_log(door->event.failMsg);
                     else ui_add_log("문이 잠겨있다.");
                } else {
                    // Success
                    if (door->event.reqItem && door->event.consumeItem) {
                        inventory_remove_item_by_name(&state->player.inventory, door->event.reqItem);
                        char msg[128];
                        snprintf(msg, sizeof(msg), "%s을(를) 사용했다.", door->event.reqItem);
                        ui_add_log(msg);
                    }

                    door->isOpen = 1;
                    ui_add_log("철컹! 문이 열렸다.");
                    audio_play_sfx("door_creak"); 
                    
                    if (door->event.setFlag) {
                        trigger_event_flag(state, door->event.setFlag, door->event.setVal);
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
