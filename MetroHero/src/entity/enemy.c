#include <stdio.h>
#include "enemy.h"
#include "../world/glyph.h"

void enemy_init(Enemy* e, const EnemyConfig* config, int x, int y) {
    if (!config) return;

    e->type = config->tile;
    e->x = x;
    e->y = y;
    e->isAlive = 1;
    e->isDead = 0; // ★ Initialize Dead State

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
    
    // ★ Random Move Init
    e->allowRandomMove = config->allowRandomMove;
    e->randomMoveInterval = config->randomMoveInterval;
    e->randomMoveTimer = 0.0f;
    
    // Copy Multi-Tile & Sprite Sheet
    e->width = config->width > 0 ? config->width : 1;
    e->height = config->height > 0 ? config->height : 1;
    e->direction = 3; // Default Down
    
    // Copy Animation Map
    e->animRight.row = config->animRight.row; e->animRight.col = config->animRight.col;
    e->animLeft.row  = config->animLeft.row;  e->animLeft.col  = config->animLeft.col;
    e->animUp.row    = config->animUp.row;    e->animUp.col    = config->animUp.col;
    e->animDown.row  = config->animDown.row;  e->animDown.col  = config->animDown.col;
    
    // Load Sprite Sheet if cols/rows specified
    if ((config->spriteRows > 1 || config->spriteCols > 1) && e->imagePath) {
        sprite_load(&e->spriteSheet, e->imagePath, config->spriteRows, config->spriteCols);
    } else {
        e->spriteSheet.texture.id = 0; // Empty
    }
    
    // Copy Event Config
    e->event = config->event;
    
    // ★ Conditional Spawn Init
    e->reqFlag = config->reqFlag;
    if (e->reqFlag != NULL) {
        e->isActive = 0; // Require flag to activate
    } else {
        e->isActive = 1; // Default active
    }
}

int enemy_is_at(const Enemy* e, int x, int y) {
    if (!e->isAlive) return 0;
    if (!e->isActive) return 0; // ★ Check Active State
    // Bounding Box Check
    return (x >= e->x && x < e->x + e->width &&
            y >= e->y && y < e->y + e->height);
}