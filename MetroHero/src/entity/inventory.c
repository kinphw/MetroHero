#include "inventory.h"
#include <stdio.h>

void inventory_init(Inventory* inv) {
    inv->count = 0;
    for (int i = 0; i < MAX_INVENTORY_SLOTS; i++) {
        inv->items[i] = NULL;
    }
}

int inventory_add(Inventory* inv, const Item* item) {
    if (!item) return 0;
    if (inv->count >= MAX_INVENTORY_SLOTS) return 0; // Full

    inv->items[inv->count++] = item;
    return 1;
}

void inventory_remove_at(Inventory* inv, int index) {
    if (index < 0 || index >= inv->count) return;

    // Shift items
    for (int i = index; i < inv->count - 1; i++) {
        inv->items[i] = inv->items[i + 1];
    }
    inv->items[inv->count - 1] = NULL;
    inv->count--;
}

const Item* inventory_get(const Inventory* inv, int index) {
    if (index < 0 || index >= inv->count) return NULL;
    return inv->items[index];
}

// Check if inventory has item by name
int inventory_has_item(const Inventory* inv, const char* name) {
    if (!name) return 0;
    for (int i = 0; i < inv->count; i++) {
        if (inv->items[i] && strcmp(inv->items[i]->name, name) == 0) {
            return 1;
        }
    }
    return 0;
}

// Remove item by name (first occurrence)
void inventory_remove_item_by_name(Inventory* inv, const char* name) {
    if (!name) return;
    for (int i = 0; i < inv->count; i++) {
        if (inv->items[i] && strcmp(inv->items[i]->name, name) == 0) {
            inventory_remove_at(inv, i);
            return;
        }
    }
}
