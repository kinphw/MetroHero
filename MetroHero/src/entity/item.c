#include "item.h"
#include <string.h>
#include <stdio.h>

#define MAX_ITEMS 64

static Item g_items[MAX_ITEMS];
static int g_itemCount = 0;

static void register_item(const char* name, ItemType type, int att, int hp, const char* desc) {
    if (g_itemCount >= MAX_ITEMS) return;
    
    Item* it = &g_items[g_itemCount++];
    it->name = _strdup(name);
    it->type = type;
    it->attackBonus = att;
    it->hpBonus = hp;
    it->desc = _strdup(desc);
}

void item_system_init(void) {
    g_itemCount = 0;

    // Weapons
    register_item("초보자 검", ITEM_WEAPON, 5, 0, "녹슬었지만 쓸만합니다.");
    register_item("강철 검", ITEM_WEAPON, 15, 0, "날카롭고 묵직한 검입니다.");
    register_item("마력의 검", ITEM_WEAPON, 30, 0, "푸른 빛이 감도는 검입니다.");

    // Armors (HP Bonus instead of Defense)
    register_item("가죽 갑옷", ITEM_ARMOR, 0, 10, "가볍고 질긴 가죽 갑옷입니다.");
    register_item("철 갑옷", ITEM_ARMOR, 0, 30, "튼튼한 철제 갑옷입니다.");
    register_item("마나 갑옷", ITEM_ARMOR, 0, 50, "마력이 깃든 갑옷입니다.");

    // Consumables
    register_item("HP 포션", ITEM_CONSUMABLE, 0, 0, "체력을 10 회복합니다.");
    register_item("대형 HP 포션", ITEM_CONSUMABLE, 0, 0, "체력을 30 회복합니다.");
    register_item("힘의 물약", ITEM_CONSUMABLE, 2, 0, "공격력을 영구적으로 2 올립니다.");
    register_item("민첩의 물약", ITEM_CONSUMABLE, 0, 0, "마시는 즉시 기분이 좋아집니다."); // 효과 미정
    register_item("스태미너 물약", ITEM_CONSUMABLE, 0, 0, "피로가 회복되는 느낌입니다.");
    register_item("신비한 원석", ITEM_ETC, 0, 0, "알 수 없는 힘이 느껴지는 돌입니다.");
    
    // Stage 3 Item
    register_item("최대 HP +5", ITEM_CONSUMABLE, 0, 5, "최대 체력을 5 늘려줍니다.");
    register_item("최대 HP +10", ITEM_CONSUMABLE, 0, 10, "최대 체력을 10 늘려줍니다.");
    register_item("공격력 +2", ITEM_CONSUMABLE, 2, 0, "공격력을 2 늘려줍니다.");
}

const Item* item_get(const char* name) {
    for (int i = 0; i < g_itemCount; i++) {
        if (strcmp(g_items[i].name, name) == 0) {
            return &g_items[i];
        }
    }
    return NULL;
}
