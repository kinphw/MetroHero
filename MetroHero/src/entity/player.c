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
#include "../world/glyph.h" // Added for colors

// ★ Level Table Definition
#define MAX_LEVEL 10
typedef struct {
    int hp;
    int attackMin;
    int attackMax;
    int reqExp; // Total EXP required to reach NEXT level
} LevelData;

static const LevelData LEVEL_TABLE[MAX_LEVEL + 1] = {
    { 0, 0, 0, 0 }, // Lv 0 (Unused)
    { 10, 1, 3, 100 },   // Lv 1. Next Lv at 100 XP.
    { 15, 2, 4, 300 },   // Lv 2. Next Lv at 300 XP.
    { 25, 3, 6, 600 },   // Lv 3. Next Lv at 600 XP.
    { 40, 5, 8, 1000 },  // Lv 4.
    { 60, 7, 10, 1500 }, // Lv 5.
    { 80, 10, 14, 2200 }, // Lv 6.
    { 110, 13, 18, 3000 }, // Lv 7.
    { 150, 16, 22, 4000 }, // Lv 8.
    { 200, 20, 28, 5500 }, // Lv 9.
    { 300, 25, 35, 99999 }, // Lv 10 (Max)
};

void player_update_stats(Player* p) {
    // 1. Base Logic from Level Table
    int lv = p->level;
    if (lv < 1) lv = 1;
    if (lv > MAX_LEVEL) lv = MAX_LEVEL;

    // Set Base Stats
    p->baseMaxHp = LEVEL_TABLE[lv].hp;
    p->baseAttackMin = LEVEL_TABLE[lv].attackMin;
    p->baseAttackMax = LEVEL_TABLE[lv].attackMax;
    
    // Set Next Level EXP
    p->expNext = LEVEL_TABLE[lv].reqExp;

    p->maxHp = p->baseMaxHp;
    p->attackMin = p->baseAttackMin;
    p->attackMax = p->baseAttackMax;

    // 2. Equipment Bonus
    if (p->equippedWeapon) {
        p->attackMin += p->equippedWeapon->attackBonus;
        p->attackMax += p->equippedWeapon->attackBonus; // Bonus adds to both min and max
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

// ★ Add Experience Logic
void player_add_exp(Player* p, int amount) {
    if (amount <= 0) return;
    
    // Log Gain
    char buf[128];
    snprintf(buf, sizeof(buf), "%s+%d EXP%s", COLOR_BRIGHT_BLUE, amount, COLOR_RESET);
    ui_add_log(buf);

    p->exp += amount;
    
    // Check Level Up Loop
    while (p->level < MAX_LEVEL && p->exp >= p->expNext) {
        p->level++;
        
        // Level Up Effect
        ui_add_log(COLOR_BRIGHT_YELLOW "★ 레벨 업! ★" COLOR_RESET);
        
        // Stat Increase Feedback
        int oldHp = p->baseMaxHp;
        int oldAtt = p->baseAttackMin;
        
        player_update_stats(p);
        
        // Heal on Level Up?
        p->hp = p->maxHp; 
        
        char msg[128];
        snprintf(msg, sizeof(msg), "최대 체력 %d -> %d / 공격력 %d -> %d", 
                 oldHp, p->baseMaxHp, oldAtt, p->baseAttackMin);
        ui_add_log(msg);
        
        // Play Sound
        audio_play_sfx("level_up");
    }
    
    if (p->level >= MAX_LEVEL) {
        p->expNext = 0; // Max Level
    } else {
        // Ensure expNext is correct (handled in update_stats)
        // Check if expNext needs update? Yes, update_stats does it.
    }
}

void player_init(Player* p) {
    // Init Item System first
    item_system_init();

    // ★ 기본 위치
    p->x = 20; 
    p->y = 10;

    p->dirX = 0;
    p->dirY = 1;

    // ★ 초기 레벨 설정
    p->level = 1;
    p->exp = 0;
    
    // ★ Init Stats from Table
    player_update_stats(p);
    p->hp = p->maxHp; // Full HP start

    inventory_init(&p->inventory);
    p->equippedWeapon = NULL;
    p->equippedArmor = NULL;

    p->item1 = ""; // Legacy
    
    p->attackCooldown = 0.0f;

    // Load Sprite Sheet
    // 2 rows, 6 columns
    sprite_load(&p->spriteSheet, "assets/player/player_sheet.png", 2, 6);
    
    // Shield Sprite
    // 1 row, 4 columns
    sprite_load(&p->shieldSprite, "assets/player/player_shield.png", 1, 4);
    p->isDefending = 0;
    
    printf("[PLAYER] Init Complete. Shield Texture ID: %u\n", p->shieldSprite.texture.id);

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
            
            // ★ Full Heal on Equip (User Request)
            player_update_stats(p);
            p->hp = p->maxHp; 
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
                 if (p->hp >= p->maxHp) {
                     ui_add_log("이미 체력이 가득 찼습니다.");
                     return;
                 }
                 p->hp += 10;
                 if (p->hp > p->maxHp) p->hp = p->maxHp;
                 ui_add_log("체력을 회복했습니다.");
                 used = 1;
             }
             else if (strcmp(item->name, "대형 HP 포션") == 0) {
                 if (p->hp >= p->maxHp) {
                     ui_add_log("이미 체력이 가득 찼습니다.");
                     return;
                 }
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
        
        // ★ Auto-Equip Logic (User Request)
        if (it->type == ITEM_WEAPON || it->type == ITEM_ARMOR) {
            // 방금 추가된 아이템 인덱스
            int newIdx = p->inventory.count - 1;
            ui_add_log("(자동 장착)");
            player_use_item(p, newIdx);
        }
    } else {
        ui_add_log("인벤토리가 가득 찼습니다!");
    }
}
