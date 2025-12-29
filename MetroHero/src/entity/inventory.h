#ifndef INVENTORY_H
#define INVENTORY_H

#include "item.h"

#define MAX_INVENTORY_SLOTS 20

typedef struct {
    const Item* items[MAX_INVENTORY_SLOTS];
    int count;
} Inventory;

void inventory_init(Inventory* inv);
int inventory_add(Inventory* inv, const Item* item);
void inventory_remove_at(Inventory* inv, int index);
const Item* inventory_get(const Inventory* inv, int index);

#endif
