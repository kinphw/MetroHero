#include <stdio.h>
#include "npc.h"
#include "../world/glyph.h"

void npc_init(NPC* npc, const NPCConfig* config, int x, int y) {
    if (!config) return;

    npc->tile = config->tile;
    npc->x = x;
    npc->y = y;
    npc->currentDialogue = 0;

    npc->name = config->name;
    npc->glyph = config->glyph;
    npc->imagePath = config->imagePath; // ★ Add
    npc->faceImagePath = config->faceImagePath; // ★ Added
    npc->dialogues = config->dialogues;
    npc->dialogueCount = config->dialogueCount;
    npc->canTrade = config->canTrade;
    npc->shopType = config->shopType;
    npc->useDialogueBox = config->useDialogueBox;
    
    // Copy Event Config
    npc->event = config->event;
    
    // Set Default Active
    npc->activeDialogues = npc->dialogues;
    npc->activeDialogueCount = npc->dialogueCount;
}

int npc_is_at(const NPC* npc, int x, int y) {
    return (npc->x == x && npc->y == y);
}

void npc_update_dialogue_state(NPC* npc, const EventRegistry* events, const StageData* stageData) {
    if (!stageData || !events) return;

    // Reset to Default First
    npc->activeDialogues = npc->dialogues;
    npc->activeDialogueCount = npc->dialogueCount;
    
    // Iterate Overrides (In Order)
    // Later overrides overwrite earlier ones if conditions are met
    for (int i = 0; i < stageData->overrideCount; i++) {
        const DialogueOverride* ov = &stageData->overrides[i];
        
        // Check Target NPC
        if (ov->npcTile == npc->tile) {
            // Check Flag Condition
            if (ov->reqFlag) {
                int val = event_get_flag(events, ov->reqFlag);
                int req = ov->reqVal > 0 ? ov->reqVal : 1;
                
                if (val >= req) {
                     // Condition Met: Apply Override
                     npc->activeDialogues = ov->newDialogues;
                     npc->activeDialogueCount = ov->newDialogueCount;
                }
            }
        }
    }
}

const char* npc_get_dialogue(const NPC* npc) {
    if (!npc->activeDialogues || npc->activeDialogueCount == 0) return "...";
    return npc->activeDialogues[npc->currentDialogue % npc->activeDialogueCount];
}

void npc_next_dialogue(NPC* npc) {
    if (npc->activeDialogueCount > 0) {
        npc->currentDialogue = (npc->currentDialogue + 1) % npc->activeDialogueCount;
    }
}