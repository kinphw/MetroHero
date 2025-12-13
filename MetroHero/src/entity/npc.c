#include <stdio.h>
#include "npc.h"
#include "../world/glyph.h"

void npc_init(NPC* npc, const NPCConfig* config, int x, int y) {
    if (!config) return;

    npc->type = config->tile;
    npc->x = x;
    npc->y = y;
    npc->currentDialogue = 0;

    npc->name = config->name;
    npc->glyph = config->glyph;
    npc->dialogues = config->dialogues;
    npc->dialogueCount = config->dialogueCount;
    npc->canTrade = config->canTrade;
    npc->shopType = config->shopType;
    npc->useDialogueBox = config->useDialogueBox;
}

int npc_is_at(const NPC* npc, int x, int y) {
    return (npc->x == x && npc->y == y);
}

const char* npc_get_dialogue(NPC* npc) {
    if (!npc->dialogues || npc->dialogueCount == 0) return "...";
    return npc->dialogues[npc->currentDialogue];
}

void npc_next_dialogue(NPC* npc) {
    if (npc->dialogueCount > 0) {
        npc->currentDialogue = (npc->currentDialogue + 1) % npc->dialogueCount;
    }
}