#include <stdio.h>
#include "enemy.h"
#include "../world/glyph.h"

void enemy_init(Enemy* e, const EnemyConfig* config, int x, int y) {
    if (!config) return;

    e->type = config->tile;
    e->x = x;
    e->y = y;
    e->isAlive = 1;

    e->name = config->name;
    e->glyph = config->glyph;
    e->imagePath = config->imagePath;       // 맵 타일 이미지
    e->portraitPath = config->portraitPath; // ★ 초상화 이미지
    e->maxHp = config->maxHp;
    e->hp = config->maxHp;
    
    e->attackMin = config->attackMin;
    e->attackMax = config->attackMax;
    e->defense = config->defense;

    e->dialogues = config->dialogues;
    e->dialogueCount = config->dialogueCount;
    e->dialogueIndex = 0;
    e->dialogueColor = config->dialogueColor;

    // AI Init (From Config)
    e->chaseOnSight = config->chaseOnSight;
    e->attackOnSight = config->attackOnSight;
    e->detectionRange = config->detectionRange;
    e->moveInterval = config->moveInterval;
    e->attackInterval = config->attackInterval;
    
    e->moveCooldown = 0.0f;
    e->attackCooldown = 0.0f;
    e->isChasing = 0;
}

int enemy_is_at(const Enemy* e, int x, int y) {
    return (e->isAlive && e->x == x && e->y == y);
}