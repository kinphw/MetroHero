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
    e->expReward = config->expReward; // ★ Added


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
    e->isProvoked = 0; // ★ 초기화
    
    // Copy Multi-Tile & Sprite Sheet
    e->width = config->width > 0 ? config->width : 1;
    e->height = config->height > 0 ? config->height : 1;
    e->direction = 3; // Default Down
    
    // Load Sprite Sheet if cols/rows specified
    if ((config->spriteRows > 1 || config->spriteCols > 1) && e->imagePath) {
        sprite_load(&e->spriteSheet, e->imagePath, config->spriteRows, config->spriteCols);
    } else {
        e->spriteSheet.texture.id = 0; // Empty
    }
    
    // Copy Event Config
    e->event = config->event;
}

int enemy_is_at(const Enemy* e, int x, int y) {
    if (!e->isAlive) return 0;
    // Bounding Box Check
    return (x >= e->x && x < e->x + e->width &&
            y >= e->y && y < e->y + e->height);
}