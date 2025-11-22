#include "chest.h"

void chest_init(Chest* c, int x, int y, const char* itemType, const char* itemName) {
    c->x = x;
    c->y = y;
    c->isOpened = 0;
    c->itemType = itemType;
    c->itemName = itemName;
}
