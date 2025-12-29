#include <stdio.h>
#include <stdlib.h>  // ★ rand() 사용
#include <time.h>    // ★ time() 사용

#include "../world/map.h"
#include "player.h"
#include "../core/ui/ui.h"  // ★ ui_add_log 사용을 위해 추가
#include "player.h"
#include "../core/ui/ui.h"
#include "../core/logic/combat.h"
#include "item.h" // Added

void player_update_stats(Player* p) {
    // 1. Base Logic
    p->maxHp = p->baseMaxHp;
    p->attackMin = p->baseAttackMin;
    p->attackMax = p->baseAttackMax;

    // 2. Equipment Bonus
    if (p->equippedWeapon) {
        p->attackMin += p->equippedWeapon->attackBonus;
        p->attackMax += p->equippedWeapon->attackBonus; // Min/Max shift together? Or spread? Plan says bonus adds to both.
        p->weaponName = p->equippedWeapon->name;
    } else {
        p->weaponName = "맨주먹";
    }

    if (p->equippedArmor) {
        p->maxHp += p->equippedArmor->hpBonus;
        p->armorName = p->equippedArmor->name;
    } else {
        p->armorName = "평상복";
    }

    // HP Cap check?
    if (p->hp > p->maxHp) p->hp = p->maxHp;
}

void player_init(Player* p) {
    // Init Item System first
    item_system_init();

    // ★ 기본 위치
    p->x = 20; 
    p->y = 10;

    p->dirX = 0;
    p->dirY = 1;

    // ★ 초기 스탯 (Base)
    p->baseMaxHp = 10;
    p->hp = 10;
    
    p->baseAttackMin = 1;
    p->baseAttackMax = 3;

    inventory_init(&p->inventory);
    p->equippedWeapon = NULL;
    p->equippedArmor = NULL;

    p->item1 = ""; // Legacy
    
    p->attackCooldown = 0.0f;

    player_update_stats(p);
}

void player_use_item(Player* p, int index) {
    const Item* item = inventory_get(&p->inventory, index);
    if (!item) return;

    if (item->type == ITEM_WEAPON) {
        // Toggle Equip
        if (p->equippedWeapon == item) {
            p->equippedWeapon = NULL;
            ui_add_log("무기를 해제했습니다.");
        } else {
            p->equippedWeapon = item;
            char buf[128];
            snprintf(buf, sizeof(buf), "%s(을)를 장착했습니다.", item->name);
            ui_add_log(buf);
        }
        player_update_stats(p);
    }
    else if (item->type == ITEM_ARMOR) {
        // Toggle Equip
        if (p->equippedArmor == item) {
            p->equippedArmor = NULL;
            ui_add_log("방어구를 해제했습니다.");
        } else {
            p->equippedArmor = item;
            char buf[128];
            snprintf(buf, sizeof(buf), "%s(을)를 착용했습니다.", item->name);
            ui_add_log(buf);
        }
        player_update_stats(p);
    }
    else if (item->type == ITEM_CONSUMABLE) {
        // Apply Effect
        int used = 0;
        
        if (item->hpBonus > 0) {
            p->baseMaxHp += item->hpBonus; // Permanent or Temporary? Name says "Max HP +5" -> Permanent
             // If it's just potion "HP Potion" -> hpBonus=0 usually in my init?
             // Let's check init. HPPotion has hpBonus=0. "Max HP +5" has hpBonus=5.
             // Wait, Potion handling:
             // "HP Potion": desc="Recover 10". Logic needs to check name or add 'recoverAmount' to Item struct.
             // Simplification: Check name for now.
             
             if (strcmp(item->name, "HP 포션") == 0) {
                 p->hp += 10;
                 if (p->hp > p->maxHp) p->hp = p->maxHp;
                 ui_add_log("체력을 회복했습니다.");
                 used = 1;
             }
             else if (strcmp(item->name, "대형 HP 포션") == 0) {
                 p->hp += 30;
                 if (p->hp > p->maxHp) p->hp = p->maxHp;
                 ui_add_log("체력을 크게 회복했습니다.");
                 used = 1;
             }
             else {
                 // Permanent Stat Boost
                 p->baseMaxHp += item->hpBonus;
                 ui_add_log("최대 체력이 증가했습니다!");
                 used = 1;
             }
             
        }
        else if (item->attackBonus > 0) {
             p->baseAttackMin += item->attackBonus;
             p->baseAttackMax += item->attackBonus;
             ui_add_log("공격력이 증가했습니다!");
             used = 1;
        }
        else if (strcmp(item->name, "HP 포션") == 0) { // Fallback if hpBonus was 0
             p->hp += 10;
             if (p->hp > p->maxHp) p->hp = p->maxHp;
             ui_add_log("체력을 회복했습니다.");
             used = 1;
        }

        player_update_stats(p);

        // Consume (Remove from inventory)
        if (used) {
            inventory_remove_at(&p->inventory, index);
        }
    }
}


void player_move(Player* p, const Map* m, int cmd) {
    int nx = p->x;
    int ny = p->y;

    switch (cmd) {
    case 'w':
        p->dirX = 0; p->dirY = -1;
        ny--;
        break;

    case 's':
        p->dirX = 0; p->dirY = 1;
        ny++;
        break;

    case 'a':
        p->dirX = -1; p->dirY = 0;
        nx--;
        break;

    case 'd':
        p->dirX = 1;  p->dirY = 0;
        nx++;
        break;
    }

    // ★ 목표 위치에 적이 있는지 확인
    Enemy* targetEnemy = map_get_enemy_at((Map*)m, nx, ny);

    if (targetEnemy != NULL) {
        // ★ 적이 있으면 이동 불가 (공격은 Space 키로)
        return;
    }

    // 일반 이동
    if (map_is_walkable(m, nx, ny)) {
        p->x = nx;
        p->y = ny;
    }
}

void player_apply_item(Player* p, const char* itemType, const char* itemName) {
    // 1. Find Item
    const Item* it = item_get(itemName);
    if (!it) {
        char buf[128];
        snprintf(buf, sizeof(buf), "알 수 없는 아이템: %s", itemName);
        ui_add_log(buf);
        return;
    }

    // 2. Add to Inventory
    if (inventory_add(&p->inventory, it)) {
        char buf[128];
        snprintf(buf, sizeof(buf), "%s을(를) 획득했다!", itemName);
        ui_add_log(buf);
    } else {
        ui_add_log("인벤토리가 가득 찼습니다!");
    }
}
