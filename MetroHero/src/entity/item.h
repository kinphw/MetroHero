#ifndef ITEM_H
#define ITEM_H

typedef enum {
    ITEM_WEAPON,
    ITEM_ARMOR,
    ITEM_CONSUMABLE,
    ITEM_ETC
} ItemType;

typedef struct {
    char* name;
    ItemType type;
    int attackBonus;    // 무기: 공격력
    int hpBonus;        // 방어구: 최대체력 증가
    char* desc;         // 설명
} Item;

// 전역 아이템 초기화 및 검색
void item_system_init(void);
const Item* item_get(const char* name);

#endif
