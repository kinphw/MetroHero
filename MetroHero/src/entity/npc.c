// src/entity/npc.c - 새 파일 생성

#include <stdio.h>
#include "npc.h"
#include "../world/glyph.h"

void npc_init(NPC* npc, char type, int x, int y) {
    npc->type = type;
    npc->x = x;
    npc->y = y;
    npc->currentDialogue = 0;

    switch (type) {
    case 'A': { // 상인
        static const char* lines[] = {
            "어서오세요! 필요한 물건이 있으신가요?",
            "좋은 장비를 구하고 싶으시다면 제게 오세요.",
            "오늘은 특별 할인 중입니다!",
            "여행자분, 안전한 여행 되세요."
        };

        npc->name = "상인";
        npc->glyph = GLYPH_NPC_MERCHANT;
        npc->dialogues = lines;
        npc->dialogueCount = sizeof(lines) / sizeof(lines[0]);
        npc->canTrade = 1;
        npc->shopType = "general";
        break;
    }

    case 'B': { // 경비원
        static const char* lines[] = {
            "이 구역은 제가 지키고 있습니다.",
            "수상한 자들이 많으니 조심하세요.",
            "질서를 유지하는 것이 제 임무입니다.",
            "평화로운 하루입니다."
        };

        npc->name = "경비원";
        npc->glyph = GLYPH_NPC_GUARD;
        npc->dialogues = lines;
        npc->dialogueCount = sizeof(lines) / sizeof(lines[0]);
        npc->canTrade = 0;
        npc->shopType = NULL;
        break;
    }

    case 'C': { // 시민
        static const char* lines[] = {
            "안녕하세요, 좋은 하루입니다.",
            "요즘 전철역이 위험하다고 하더라고요.",
            "지하에는 이상한 것들이 많다던데...",
            "조심히 다니세요!"
        };

        npc->name = "시민";
        npc->glyph = GLYPH_NPC_CITIZEN;
        npc->dialogues = lines;
        npc->dialogueCount = sizeof(lines) / sizeof(lines[0]);
        npc->canTrade = 0;
        npc->shopType = NULL;
        break;
    }

    default: { // 기본 NPC
        static const char* lines[] = {
            "안녕하세요.",
            "무슨 일이신가요?",
            "도와드릴 일이 있나요?"
        };

        npc->name = "NPC";
        npc->glyph = GLYPH_NPC_GENERIC;
        npc->dialogues = lines;
        npc->dialogueCount = sizeof(lines) / sizeof(lines[0]);
        npc->canTrade = 0;
        npc->shopType = NULL;
        break;
    }
    }
}

int npc_is_at(const NPC* npc, int x, int y) {
    return (npc->x == x && npc->y == y);
}

const char* npc_get_dialogue(NPC* npc) {
    return npc->dialogues[npc->currentDialogue];
}

void npc_next_dialogue(NPC* npc) {
    npc->currentDialogue = (npc->currentDialogue + 1) % npc->dialogueCount;
}