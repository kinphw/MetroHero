#ifndef PLAYER_H
#define PLAYER_H

struct Map;
#include "../core/graphics/sprite_sheet.h" // Added
#include "inventory.h"

typedef struct {
    SpriteSheet spriteSheet; // Added
	int x , y;

	int dirX;
	int dirY;

	int hp;
	int maxHp;
    int baseMaxHp; // 기본 체력
    
    // ★ Level System
    int level;
    int exp;
    int expNext; // XP required for next level

	// 공격력 (Base + Weapon)
	int attackMin;
	int attackMax;
    int baseAttackMin;
    int baseAttackMax;

    // 인벤토리 & 장비
    Inventory inventory;
    const Item* equippedWeapon;
    const Item* equippedArmor;

	const char* weaponName; // Legacy (Simple Display)
	const char* armorName;  // Legacy
	const char* item1;      // Legacy

    // ★ 전투 관련 (실시간)
    float attackCooldown; // 0보다 크면 공격 불가
    
    // ★ 방어 관련
    int isDefending;
    SpriteSheet shieldSprite;

} Player;

void player_init(Player* p);
void player_update_stats(Player* p);
void player_add_exp(Player* p, int amount); // ★ Added
void player_use_item(Player* p, int index);
void player_move(Player* p , struct Map* m , int cmd);  // ★ const 제거

#endif