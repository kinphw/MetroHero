#ifndef PLAYER_H
#define PLAYER_H

struct Map;

#include "inventory.h"

typedef struct {
	int x , y;

	int dirX;
	int dirY;

	int hp;
	int maxHp;
    int baseMaxHp; // 기본 체력

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

} Player;

void player_init(Player* p);
void player_update_stats(Player* p);
void player_use_item(Player* p, int index);
void player_move(Player* p , struct Map* m , int cmd);  // ★ const 제거

#endif