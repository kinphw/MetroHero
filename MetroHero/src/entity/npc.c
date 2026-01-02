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
    
    // Copy Branches
    npc->branches = config->branches;
    npc->branchCount = config->branchCount;
    
    // Copy Event Config
    npc->event = config->event; // Default
    npc->activeEvent = config->event; // Initial Active
    
    // Set Default Active
    npc->activeDialogues = npc->dialogues;
    npc->activeDialogueCount = npc->dialogueCount;
}

int npc_is_at(const NPC* npc, int x, int y) {
    return (npc->x == x && npc->y == y);
}

void npc_update_dialogue_state(NPC* npc, const EventRegistry* events, const StageData* stageData) {
    if (!npc || !events) return;

    // Iterate Branches
    if (npc->branches && npc->branchCount > 0) {
        for (int i = 0; i < npc->branchCount; i++) {
            const DialogueBranch* b = &npc->branches[i];
            
            // Check Condition
            int match = 0;
            if (b->reqFlag == NULL) {
                match = 1; // Always match (Default)
            } else {
                int val = event_get_flag(events, b->reqFlag);
                int req = b->reqVal > 0 ? b->reqVal : 1;
                if (val >= req) match = 1;
            }
            
            if (match) {
                // Apply Branch
                // Safety Check
                if (b->startIndex >= 0 && (b->startIndex + b->count) <= npc->dialogueCount) {
                    npc->activeDialogues = &npc->dialogues[b->startIndex];
                    npc->activeDialogueCount = b->count;
                } else {
                    // Fallback to full list if range invalid
                    npc->activeDialogues = npc->dialogues;
                    npc->activeDialogueCount = npc->dialogueCount;
                }
                
                // Set Active Event
                if (b->event) {
                    npc->activeEvent = *b->event;
                } else {
                    // Clear Active Event (or keep Fallback? usually clear if branch has no event)
                    // But maybe we want the default event?
                    // Let's assume branch defines everything for that state.
                    memset(&npc->activeEvent, 0, sizeof(EventConfig));
                }
                
                return; // Priority Found, Stop.
            }
        }
    }
    
    // Legacy / Fallback (No Branches or All Checks Failed)
    // Keep internal default
    npc->activeDialogues = npc->dialogues;
    npc->activeDialogueCount = npc->dialogueCount;
    npc->activeEvent = npc->event; // Reset to default config event
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